use alloc::{borrow::Cow, sync::Arc, vec::Vec};
use syscall::{SIGKILL, SIGSTOP};
use core::{cmp::Ordering, mem::{self, size_of}, num::NonZeroUsize};
use spin::RwLock;

use crate::{
    arch::{interrupt::InterruptStack, paging::PAGE_SIZE}, common::aligned_box::AlignedBox, context::{self, arch, file::FileDescriptor, memory::AddrSpace}, cpu_set::{LogicalCpuId, LogicalCpuSet}, ipi::{ipi, IpiKind, IpiTarget}, memory::{allocate_p2frame, deallocate_p2frame, Enomem, Frame, RaiiFrame}, paging::{RmmA, RmmArch}, percpu::PercpuBlock, scheme::{CallerCtx, FileHandle, SchemeNamespace}, sync::WaitMap,
};

use crate::syscall::{
    data::SigAction,
    error::{Error, Result, EAGAIN, ESRCH},
    flag::{SigActionFlags, SIG_DFL},
};

/// Unique identifier for a context (i.e. `pid`).
use ::core::sync::atomic::AtomicUsize;

use super::{memory::{GrantFileRef, AddrSpaceWrapper}, empty_cr3};
int_like!(ContextId, AtomicContextId, usize, AtomicUsize);

/// The status of a context - used for scheduling
/// See `syscall::process::waitpid` and the `sync` module for examples of usage
#[derive(Clone, Debug)]
pub enum Status {
    Runnable,

    // TODO: Rename to SoftBlocked and move status_reason to this variant.
    /// Not currently runnable, typically due to some blocking syscall, but it can be trivially
    /// unblocked by e.g. signals.
    Blocked,

    /// Not currently runnable, and cannot be runnable until manually unblocked, depending on what
    /// reason.
    HardBlocked {
        reason: HardBlockedReason,
    },

    Stopped(usize),
    Exited(usize),
}

impl Status {
    pub fn is_runnable(&self) -> bool {
        matches!(self, Self::Runnable)
    }
    pub fn is_soft_blocked(&self) -> bool {
        matches!(self, Self::Blocked)
    }
}

#[derive(Clone, Debug)]
pub enum HardBlockedReason {
    AwaitingMmap { file_ref: GrantFileRef },
    // TODO: PageFaultOom?
    NotYetStarted,
    // TODO: ptrace_stop?
}

#[derive(Copy, Clone, Debug)]
pub struct WaitpidKey {
    pub pid: Option<ContextId>,
    pub pgid: Option<ContextId>,
}

impl Ord for WaitpidKey {
    fn cmp(&self, other: &WaitpidKey) -> Ordering {
        // If both have pid set, compare that
        if let Some(s_pid) = self.pid {
            if let Some(o_pid) = other.pid {
                return s_pid.cmp(&o_pid);
            }
        }

        // If both have pgid set, compare that
        if let Some(s_pgid) = self.pgid {
            if let Some(o_pgid) = other.pgid {
                return s_pgid.cmp(&o_pgid);
            }
        }

        // If either has pid set, it is greater
        if self.pid.is_some() {
            return Ordering::Greater;
        }

        if other.pid.is_some() {
            return Ordering::Less;
        }

        // If either has pgid set, it is greater
        if self.pgid.is_some() {
            return Ordering::Greater;
        }

        if other.pgid.is_some() {
            return Ordering::Less;
        }

        // If all pid and pgid are None, they are equal
        Ordering::Equal
    }
}

impl PartialOrd for WaitpidKey {
    fn partial_cmp(&self, other: &WaitpidKey) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl PartialEq for WaitpidKey {
    fn eq(&self, other: &WaitpidKey) -> bool {
        self.cmp(other) == Ordering::Equal
    }
}

impl Eq for WaitpidKey {}

/// A context, which identifies either a process or a thread
#[derive(Debug)]
pub struct Context {
    /// The ID of this context
    pub id: ContextId,
    /// The group ID of this context
    pub pgid: ContextId,
    /// The ID of the parent context
    pub ppid: ContextId,
    /// The ID of the session
    pub session_id: ContextId,
    /// The real user id
    pub ruid: u32,
    /// The real group id
    pub rgid: u32,
    /// The real namespace id
    pub rns: SchemeNamespace,
    /// The effective user id
    pub euid: u32,
    /// The effective group id
    pub egid: u32,
    /// The effective namespace id
    pub ens: SchemeNamespace,

    pub sig: SignalState,

    /// Process umask
    pub umask: usize,
    /// Status of context
    pub status: Status,
    pub status_reason: &'static str,
    /// Context running or not
    pub running: bool,
    /// Current CPU ID
    pub cpu_id: Option<LogicalCpuId>,
    /// Time this context was switched to
    pub switch_time: u128,
    /// Amount of CPU time used
    pub cpu_time: u128,
    /// Scheduler CPU affinity. If set, [`cpu_id`] can except [`None`] never be anything else than
    /// this value.
    pub sched_affinity: LogicalCpuSet,
    /// Keeps track of whether this context is currently handling a syscall. Only up-to-date when
    /// not running.
    pub inside_syscall: bool,

    #[cfg(feature = "syscall_debug")]
    pub syscall_debug_info: crate::syscall::debug::SyscallDebugInfo,

    /// Head buffer to use when system call buffers are not page aligned
    // TODO: Store in user memory?
    pub syscall_head: Option<RaiiFrame>,
    /// Tail buffer to use when system call buffers are not page aligned
    // TODO: Store in user memory?
    pub syscall_tail: Option<RaiiFrame>,
    /// Context is being waited on
    pub waitpid: Arc<WaitMap<WaitpidKey, (ContextId, usize)>>,
    /// Context should wake up at specified time
    pub wake: Option<u128>,
    /// The architecture specific context
    pub arch: arch::Context,
    /// Kernel FX - used to store SIMD and FPU registers on context switch
    pub kfx: AlignedBox<[u8], { arch::KFX_ALIGN }>,
    /// Kernel stack, if located on the heap.
    pub kstack: Option<Kstack>,
    /// Address space containing a page table lock, and grants. Normally this will have a value,
    /// but can be None while the context is being reaped or when a new context is created but has
    /// not yet had its address space changed. Note that these are only for user mappings; kernel
    /// mappings are universal and independent on address spaces or contexts.
    pub addr_space: Option<Arc<AddrSpaceWrapper>>,
    /// The name of the context
    // TODO: fixed size ArrayString?
    pub name: Cow<'static, str>,
    /// The open files in the scheme
    pub files: Arc<RwLock<Vec<Option<FileDescriptor>>>>,
    /// Signal actions
    pub actions: Arc<RwLock<Vec<(SigAction, usize)>>>,
    /// All contexts except kmain will primarily live in userspace, and enter the kernel only when
    /// interrupts or syscalls occur. This flag is set for all contexts but kmain.
    pub userspace: bool,
    /// A somewhat hacky way to initially stop a context when creating
    /// a new instance of the proc: scheme, entirely separate from
    /// signals or any other way to restart a process.
    pub ptrace_stop: bool,
    pub fmap_ret: Option<Frame>,
}

#[derive(Clone, Copy, Debug)]
pub struct SignalState {
    /// Bitset of pending signals.
    pub pending: u64,
    /// Bitset of procmasked signals.
    pub procmask: u64,

    /// A function pointer to the userspace signal handler.
    pub handler: Option<SignalHandler>,
}
#[derive(Clone, Copy, Debug)]
pub struct SignalHandler {
    pub handler: NonZeroUsize,
    pub altstack: Option<Altstack>,
}
#[derive(Clone, Copy, Debug)]
pub struct Altstack {
    pub base: NonZeroUsize,
    pub len: NonZeroUsize,
}

impl Context {
    pub fn new(id: ContextId) -> Result<Context> {
        let this = Context {
            id,
            pgid: id,
            ppid: ContextId::from(0),
            session_id: ContextId::from(0),
            ruid: 0,
            rgid: 0,
            rns: SchemeNamespace::from(0),
            euid: 0,
            egid: 0,
            ens: SchemeNamespace::from(0),
            sig: SignalState {
                pending: 0,
                procmask: !0,
                handler: None,
            },
            umask: 0o022,
            status: Status::HardBlocked { reason: HardBlockedReason::NotYetStarted },
            status_reason: "",
            running: false,
            cpu_id: None,
            switch_time: 0,
            cpu_time: 0,
            sched_affinity: LogicalCpuSet::all(),
            inside_syscall: false,
            syscall_head: Some(RaiiFrame::allocate()?),
            syscall_tail: Some(RaiiFrame::allocate()?),
            waitpid: Arc::new(WaitMap::new()),
            wake: None,
            arch: arch::Context::new(),
            kfx: AlignedBox::<[u8], { arch::KFX_ALIGN }>::try_zeroed_slice(crate::arch::kfx_size())?,
            kstack: None,
            addr_space: None,
            name: Cow::Borrowed(""),
            files: Arc::new(RwLock::new(Vec::new())),
            actions: Self::empty_actions(),
            userspace: false,
            ptrace_stop: false,
            fmap_ret: None,

            #[cfg(feature = "syscall_debug")]
            syscall_debug_info: crate::syscall::debug::SyscallDebugInfo::default(),
        };
        Ok(this)
    }

    /// Block the context, and return true if it was runnable before being blocked
    pub fn block(&mut self, reason: &'static str) -> bool {
        if self.status.is_runnable() {
            self.status = Status::Blocked;
            self.status_reason = reason;
            true
        } else {
            false
        }
    }

    pub fn hard_block(&mut self, reason: HardBlockedReason) -> bool {
        if self.status.is_runnable() {
            self.status = Status::HardBlocked { reason };

            true
        } else {
            false
        }
    }

    /// Unblock context, and return true if it was blocked before being marked runnable
    pub fn unblock(&mut self) -> bool {
        if self.unblock_no_ipi() {
            if let Some(cpu_id) = self.cpu_id {
                if cpu_id != crate::cpu_id() {
                    // Send IPI if not on current CPU
                    ipi(IpiKind::Wakeup, IpiTarget::Other);
                }
            }

            true
        } else {
            false
        }
    }

    /// Unblock context without IPI, and return true if it was blocked before being marked runnable
    pub fn unblock_no_ipi(&mut self) -> bool {
        if self.status.is_soft_blocked() {
            self.status = Status::Runnable;
            self.status_reason = "";

            true
        } else {
            false
        }
    }

    /// Add a file to the lowest available slot.
    /// Return the file descriptor number or None if no slot was found
    pub fn add_file(&self, file: FileDescriptor) -> Option<FileHandle> {
        self.add_file_min(file, 0)
    }

    /// Add a file to the lowest available slot greater than or equal to min.
    /// Return the file descriptor number or None if no slot was found
    pub fn add_file_min(&self, file: FileDescriptor, min: usize) -> Option<FileHandle> {
        let mut files = self.files.write();
        for (i, file_option) in files.iter_mut().enumerate() {
            if file_option.is_none() && i >= min {
                *file_option = Some(file);
                return Some(FileHandle::from(i));
            }
        }
        let len = files.len();
        if len < super::CONTEXT_MAX_FILES {
            if len >= min {
                files.push(Some(file));
                Some(FileHandle::from(len))
            } else {
                drop(files);
                self.insert_file(FileHandle::from(min), file)
            }
        } else {
            None
        }
    }

    /// Get a file
    pub fn get_file(&self, i: FileHandle) -> Option<FileDescriptor> {
        let files = self.files.read();
        if i.get() < files.len() {
            files[i.get()].clone()
        } else {
            None
        }
    }

    /// Insert a file with a specific handle number. This is used by dup2
    /// Return the file descriptor number or None if the slot was not empty, or i was invalid
    pub fn insert_file(&self, i: FileHandle, file: FileDescriptor) -> Option<FileHandle> {
        let mut files = self.files.write();
        if i.get() < super::CONTEXT_MAX_FILES {
            while i.get() >= files.len() {
                files.push(None);
            }
            if files[i.get()].is_none() {
                files[i.get()] = Some(file);
                Some(i)
            } else {
                None
            }
        } else {
            None
        }
    }

    /// Remove a file
    // TODO: adjust files vector to smaller size if possible
    pub fn remove_file(&self, i: FileHandle) -> Option<FileDescriptor> {
        let mut files = self.files.write();
        if i.get() < files.len() {
            files[i.get()].take()
        } else {
            None
        }
    }

    pub fn addr_space(&self) -> Result<&Arc<AddrSpaceWrapper>> {
        self.addr_space.as_ref().ok_or(Error::new(ESRCH))
    }
    pub fn set_addr_space(
        &mut self,
        addr_space: Option<Arc<AddrSpaceWrapper>>,
    ) -> Option<Arc<AddrSpaceWrapper>> {
        if let (Some(ref old), Some(ref new)) = (&self.addr_space, &addr_space) && Arc::ptr_eq(old, new) {
            return addr_space;
        };

        if self.id == super::context_id() {
            // TODO: Share more code with context::arch::switch_to.
            let this_percpu = PercpuBlock::current();

            if let Some(ref prev_addrsp) = self.addr_space {
                assert!(Arc::ptr_eq(&this_percpu.current_addrsp.borrow().as_ref().unwrap(), prev_addrsp));
                prev_addrsp.acquire_read().used_by.atomic_clear(this_percpu.cpu_id);
            }

            let _old_addrsp = core::mem::replace(&mut *this_percpu.current_addrsp.borrow_mut(), addr_space.clone());

            if let Some(ref new) = addr_space {
                let new_addrsp = new.acquire_read();
                new_addrsp.used_by.atomic_set(this_percpu.cpu_id);

                unsafe {
                    new_addrsp.table.utable.make_current();
                }
            } else {
                unsafe {
                    crate::paging::RmmA::set_table(rmm::TableKind::User, empty_cr3());
                }
            }
        } else {
            assert!(!self.running);
        }

        core::mem::replace(&mut self.addr_space, addr_space)
    }
    pub fn empty_actions() -> Arc<RwLock<Vec<(SigAction, usize)>>> {
        Arc::new(RwLock::new(vec![(
            SigAction {
                sa_handler: unsafe { mem::transmute(SIG_DFL) },
                sa_mask: 0,
                sa_flags: SigActionFlags::empty(),
            },
            0
        ); 128]))
    }
    pub fn caller_ctx(&self) -> CallerCtx {
        CallerCtx {
            pid: self.id.into(),
            uid: self.euid,
            gid: self.egid,
        }
    }

    fn can_access_regs(&self) -> bool {
        self.userspace
    }

    pub fn regs(&self) -> Option<&InterruptStack> {
        if !self.can_access_regs() {
            return None;
        }
        let Some(ref kstack) = self.kstack else {
            return None;
        };
        Some(unsafe { &*kstack.initial_top().sub(size_of::<InterruptStack>()).cast() })
    }
    pub fn regs_mut(&mut self) -> Option<&mut InterruptStack> {
        if !self.can_access_regs() {
            return None;
        }
        let Some(ref mut kstack) = self.kstack else {
            return None;
        };
        Some(unsafe { &mut *kstack.initial_top().sub(size_of::<InterruptStack>()).cast() })
    }
}

impl SignalState {
    pub fn deliverable(&self) -> u64 {
        const CANT_BLOCK: u64 = (1 << (SIGKILL - 1)) | (1 << (SIGSTOP - 1));
        self.pending & (CANT_BLOCK | !self.procmask)
    }
}

/// Wrapper struct for borrowing the syscall head or tail buf.
#[derive(Debug)]
pub struct BorrowedHtBuf {
    inner: Option<RaiiFrame>,
    head_and_not_tail: bool,
}
impl BorrowedHtBuf {
    pub fn head() -> Result<Self> {
        Ok(Self {
            inner: Some(
                context::current()?
                    .write()
                    .syscall_head
                    .take()
                    .ok_or(Error::new(EAGAIN))?,
            ),
            head_and_not_tail: true,
        })
    }
    pub fn tail() -> Result<Self> {
        Ok(Self {
            inner: Some(
                context::current()?
                    .write()
                    .syscall_tail
                    .take()
                    .ok_or(Error::new(EAGAIN))?,
            ),
            head_and_not_tail: false,
        })
    }
    pub fn buf(&self) -> &[u8; PAGE_SIZE] {
        unsafe {
            &*(RmmA::phys_to_virt(
                self.inner
                    .as_ref()
                    .expect("must succeed")
                    .get()
                    .start_address(),
            )
            .data() as *const [u8; PAGE_SIZE])
        }
    }
    pub fn buf_mut(&mut self) -> &mut [u8; PAGE_SIZE] {
        unsafe {
            &mut *(RmmA::phys_to_virt(
                self.inner
                    .as_mut()
                    .expect("must succeed")
                    .get()
                    .start_address(),
            )
            .data() as *mut [u8; PAGE_SIZE])
        }
    }
    pub fn frame(&self) -> Frame {
        self.inner.as_ref().expect("must succeed").get()
    }
    /*
    pub fn use_for_slice(&mut self, raw: UserSlice) -> Result<Option<&[u8]>> {
        if raw.len() > self.buf().len() {
            return Ok(None);
        }
        raw.copy_to_slice(&mut self.buf_mut()[..raw.len()])?;
        Ok(Some(&self.buf()[..raw.len()]))
    }
    pub fn use_for_string(&mut self, raw: UserSlice) -> Result<&str> {
        let slice = self.use_for_slice(raw)?.ok_or(Error::new(ENAMETOOLONG))?;
        core::str::from_utf8(slice).map_err(|_| Error::new(EINVAL))
    }
    pub unsafe fn use_for_struct<T>(&mut self) -> Result<&mut T> {
        if mem::size_of::<T>() > PAGE_SIZE || mem::align_of::<T>() > PAGE_SIZE {
            return Err(Error::new(EINVAL));
        }
        self.buf_mut().fill(0_u8);
        Ok(unsafe { &mut *self.buf_mut().as_mut_ptr().cast() })
    }
    */
}
impl Drop for BorrowedHtBuf {
    fn drop(&mut self) {
        let Ok(context) = context::current() else {
            return;
        };
        let Some(inner) = self.inner.take() else {
            return;
        };
        match context.write() {
            mut context => {
                (if self.head_and_not_tail {
                    &mut context.syscall_head
                } else {
                    &mut context.syscall_tail
                })
                .get_or_insert(inner);
            }
        }
    }
}

pub struct Kstack {
    /// naturally aligned, order 4
    base: Frame,
}
impl Kstack {
    pub fn new() -> Result<Self, Enomem> {
        Ok(Self {
            base: allocate_p2frame(4).ok_or(Enomem)?,
        })
    }
    pub fn initial_top(&self) -> *mut u8 {
        unsafe {
            (RmmA::phys_to_virt(self.base.start_address()).data() as *mut u8).add(PAGE_SIZE << 4)
        }
    }
    pub fn len(&self) -> usize {
        PAGE_SIZE << 4
    }
}

const _: () = {
    if PAGE_SIZE << 4 != arch::KSTACK_SIZE {
        panic!();
    }
    if arch::KSTACK_ALIGN > (PAGE_SIZE << 4) {
        panic!();
    }
};

impl Drop for Kstack {
    fn drop(&mut self) {
        unsafe {
            deallocate_p2frame(self.base, 4)
        }
    }
}
impl core::fmt::Debug for Kstack {
    fn fmt(&self, f: &mut core::fmt::Formatter<'_>) -> core::fmt::Result {
        write!(f, "[kstack at {:?}]", self.base)
    }
}

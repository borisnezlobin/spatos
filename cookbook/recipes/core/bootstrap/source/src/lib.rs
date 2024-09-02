#![no_std]
#![feature(
    asm_const,
    core_intrinsics,
)]

#[cfg(target_arch = "aarch64")]
#[path = "aarch64.rs"]
pub mod arch;

#[cfg(target_arch = "x86")]
#[path = "i686.rs"]
pub mod arch;

#[cfg(target_arch = "x86_64")]
#[path = "x86_64.rs"]
pub mod arch;

pub mod exec;
pub mod initfs;
pub mod start;

extern crate alloc;

use syscall::data::Map;
use syscall::flag::MapFlags;

#[panic_handler]
fn panic_handler(info: &core::panic::PanicInfo) -> ! {
    use core::fmt::Write;

    struct Writer;

    impl Write for Writer {
        fn write_str(&mut self, s: &str) -> core::fmt::Result {
            syscall::write(1, s.as_bytes()).map_err(|_| core::fmt::Error).map(|_| ())
        }
    }

    let _ = writeln!(&mut Writer, "{}", info);
    core::intrinsics::abort();
}

#[cfg(target_pointer_width = "32")]
const HEAP_OFF: usize = 0x4000_0000;

#[cfg(target_pointer_width = "64")]
const HEAP_OFF: usize = 0x4000_0000_0000;

struct Allocator;
#[global_allocator]
static ALLOCATOR: Allocator = Allocator;

static mut HEAP: Option<linked_list_allocator::Heap> = None;
static mut HEAP_TOP: usize = HEAP_OFF + SIZE;
const SIZE: usize = 1024 * 1024;
const HEAP_INCREASE_BY: usize = SIZE;

unsafe impl alloc::alloc::GlobalAlloc for Allocator {
    unsafe fn alloc(&self, layout: core::alloc::Layout) -> *mut u8 {
        let heap = HEAP.get_or_insert_with(|| {
            HEAP_TOP = HEAP_OFF + SIZE;
            let _ = syscall::fmap(!0, &Map { offset: 0, size: SIZE, address: HEAP_OFF, flags: MapFlags::PROT_WRITE | MapFlags::PROT_READ | MapFlags::MAP_PRIVATE | MapFlags::MAP_FIXED_NOREPLACE })
                .expect("failed to map initial heap");
            linked_list_allocator::Heap::new(HEAP_OFF as *mut u8, SIZE)
        });

        match heap.allocate_first_fit(layout) {
            Ok(p) => p.as_ptr(),
            Err(_) => {
                if layout.size() > HEAP_INCREASE_BY || layout.align() > 4096 {
                    return core::ptr::null_mut();
                }

                let _ = syscall::fmap(!0, &Map { offset: 0, size: HEAP_INCREASE_BY, address: HEAP_TOP, flags: MapFlags::PROT_WRITE | MapFlags::PROT_READ | MapFlags::MAP_PRIVATE | MapFlags::MAP_FIXED_NOREPLACE })
                    .expect("failed to extend heap");
                heap.extend(HEAP_INCREASE_BY);
                HEAP_TOP += HEAP_INCREASE_BY;

                return self.alloc(layout);
            }
        }
    }
    unsafe fn dealloc(&self, ptr: *mut u8, layout: core::alloc::Layout) {
        HEAP.as_mut().unwrap().deallocate(core::ptr::NonNull::new(ptr).unwrap(), layout)
    }
}

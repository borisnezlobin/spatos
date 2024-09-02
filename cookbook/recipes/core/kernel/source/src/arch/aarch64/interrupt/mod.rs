//! Interrupt instructions

use core::arch::asm;

#[macro_use]
pub mod handler;

pub mod exception;
pub mod irq;
pub mod syscall;
pub mod trace;

use crate::cpu_set::LogicalCpuId;

pub use self::{handler::InterruptStack, trace::stack_trace};

/// Clear interrupts
#[inline(always)]
pub unsafe fn disable() {
    asm!("msr daifset, #2");
}

/// Set interrupts
#[inline(always)]
pub unsafe fn enable() {
    asm!("msr daifclr, #2");
}

/// Set interrupts and halt
/// This will atomically wait for the next interrupt
/// Performing enable followed by halt is not guaranteed to be atomic, use this instead!
#[inline(always)]
pub unsafe fn enable_and_halt() {
    asm!("msr daifclr, #2");
    asm!("wfi");
}

/// Set interrupts and nop
/// This will enable interrupts and allow the IF flag to be processed
/// Simply enabling interrupts does not gurantee that they will trigger, use this instead!
#[inline(always)]
pub unsafe fn enable_and_nop() {
    asm!("msr daifclr, #2");
    asm!("nop");
}

/// Halt instruction
#[inline(always)]
pub unsafe fn halt() {
    asm!("wfi");
}

/// Pause instruction
/// Safe because it is similar to a NOP, and has no memory effects
#[inline(always)]
pub fn pause() {
    unsafe { asm!("nop") };
}

pub fn available_irqs_iter(cpu_id: LogicalCpuId) -> impl Iterator<Item = u8> + 'static {
    0..0
}

pub fn bsp_apic_id() -> Option<u32> {
    //TODO
    None
}

#[inline]
pub fn is_reserved(cpu_id: LogicalCpuId, index: u8) -> bool {
    //TODO
    true
}

#[inline]
pub fn set_reserved(cpu_id: LogicalCpuId, index: u8, reserved: bool) {
    //TODO
}

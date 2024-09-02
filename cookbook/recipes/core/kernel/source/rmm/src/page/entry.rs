use core::marker::PhantomData;

use crate::{
    Arch,
    PageFlags,
    PhysicalAddress,
};

#[derive(Clone, Copy, Debug)]
pub struct PageEntry<A> {
    data: usize,
    phantom: PhantomData<A>,
}

impl<A: Arch> PageEntry<A> {
    #[inline(always)]
    pub fn new(data: usize) -> Self {
        Self { data, phantom: PhantomData }
    }

    #[inline(always)]
    pub fn data(&self) -> usize {
        self.data
    }

    #[inline(always)]
    pub fn address(&self) -> Result<PhysicalAddress, PhysicalAddress> {
        let addr = PhysicalAddress(self.data & A::ENTRY_ADDRESS_MASK);

        if self.present() {
            Ok(addr)
        } else {
            Err(addr)
        }
    }

    #[inline(always)]
    pub fn flags(&self) -> PageFlags<A> {
        unsafe { PageFlags::from_data(self.data & A::ENTRY_FLAGS_MASK) }
    }
    #[inline(always)]
    pub fn set_flags(&mut self, flags: PageFlags<A>) {
        self.data &= !A::ENTRY_FLAGS_MASK;
        self.data |= flags.data();
    }

    #[inline(always)]
    pub fn present(&self) -> bool {
        self.data & A::ENTRY_FLAG_PRESENT != 0
    }
}

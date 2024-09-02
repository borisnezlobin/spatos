use core::marker::PhantomData;

use crate::{
    Arch,
    FrameAllocator,
    FrameCount,
    FrameUsage,
    MemoryArea,
    PhysicalAddress,
};

pub struct BumpAllocator<A> {
    areas: &'static [MemoryArea],
    offset: usize,
    abs_offset: PhysicalAddress,
    _marker: PhantomData<fn() -> A>,
}

impl<A: Arch> BumpAllocator<A> {
    pub fn new(areas: &'static [MemoryArea], offset: usize) -> Self {
        Self {
            areas,
            offset: 0,
            abs_offset: areas[0].base,
            _marker: PhantomData,
        }
    }
    pub fn areas(&self) -> &'static [MemoryArea] {
        self.areas
    }
    /// Returns one semifree and the fully free areas. The offset is the number of bytes after
    /// which the first area is free.
    pub fn free_areas(&self) -> (&'static [MemoryArea], usize) {
        let mut areas = self.areas;
        let mut offset = self.offset;

        loop {
            let Some(area) = areas.first() else {
                return (&[], 0);
            };

            if offset > area.size {
                areas = &areas[1..];
                offset -= area.size;
            } else {
                return (areas, offset);
            }
        }
    }
    pub fn abs_offset(&self) -> PhysicalAddress {
        self.abs_offset
    }

    pub fn offset(&self) -> usize {
        self.offset
    }
}

impl<A: Arch> FrameAllocator for BumpAllocator<A> {
    unsafe fn allocate(&mut self, count: FrameCount) -> Option<PhysicalAddress> {
        let mut offset = self.offset;
        for area in self.areas.iter() {
            if offset < area.size {
                if area.size - offset < count.data() * A::PAGE_SIZE {
                    /*
                    // The area may be too small for this alloc request. In that case, skip to the
                    // next area.
                    self.offset += area.size - offset;
                    offset = 0;
                    continue;
                    */
                    return None;
                }

                let page_phys = area.base.add(offset);
                let page_virt = A::phys_to_virt(page_phys);
                A::write_bytes(page_virt, 0, count.data() * A::PAGE_SIZE);
                self.offset += count.data() * A::PAGE_SIZE;

                self.abs_offset = page_phys;
                return Some(page_phys);
            }
            offset -= area.size;
        }
        None
    }

    unsafe fn free(&mut self, _address: PhysicalAddress, _count: FrameCount) {
        unimplemented!("BumpAllocator::free not implemented");
    }

    unsafe fn usage(&self) -> FrameUsage {
        let mut total = 0;
        for area in self.areas.iter() {
            total += area.size >> A::PAGE_SHIFT;
        }
        let used = self.offset >> A::PAGE_SHIFT;
        FrameUsage::new(FrameCount::new(used), FrameCount::new(total))
    }
}

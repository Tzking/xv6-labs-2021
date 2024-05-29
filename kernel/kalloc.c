// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "memlayout.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

extern void panic(char *s);

void kfree(void *pa);
void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
    struct run *next;
};

struct {
    struct spinlock lock;
    struct spinlock reflock;
    struct run *freelist;
    uint *refcount;
} kmem;

void kinit() {
    initlock(&kmem.lock, "kmem");
    initlock(&kmem.reflock, "ref");
    uint64 rc_pages = ((PHYSTOP - (uint64)end) >> 12) + 1;
    rc_pages = ((rc_pages * sizeof(uint)) >> 12) + 1;
    kmem.refcount = (uint *)end;
    uint64 rc_offset = rc_pages << 12;
    freerange(end + rc_offset, (void *)PHYSTOP);
}

// 将地址转换为页号
inline int kgetrefidx(void *pa) {
    return ((char *)pa - (char *)PGROUNDDOWN((uint64)end)) >> 12;
}

void freerange(void *pa_start, void *pa_end) {
    char *p;
    p = (char *)PGROUNDUP((uint64)pa_start);
    for (; p + PGSIZE <= (char *)pa_end; p += PGSIZE) {
        kmem.refcount[kgetrefidx(p)] = 1;
        kfree(p);
    }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa) {
    struct run *r;

    if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
        panic("kfree");

    acquire(&kmem.lock);
    if (--kmem.refcount[kgetrefidx(pa)]) {
        release(&kmem.lock);
        return;
    } 
        release(&kmem.lock);
        // Fill with junk to catch dangling refs.
        memset(pa, 1, PGSIZE);
        r = (struct run *)pa;

        acquire(&kmem.lock);
        r->next = kmem.freelist;
        kmem.freelist = r;
        release(&kmem.lock);
}

int kgetref(void *pa) {
    return kmem.refcount[kgetrefidx(pa)];
}

void kaddref(void *pa) {
    kmem.refcount[kgetrefidx(pa)]++;
}

inline void acquire_ref(void) {
    acquire(&kmem.reflock);
}

inline void release_ref(void) {
    release(&kmem.reflock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *kalloc(void) {
    struct run *r;
    acquire(&kmem.lock);
    r = kmem.freelist;
    if (r) {
        kmem.freelist = r->next;
        kmem.refcount[kgetrefidx((void *)r)] = 1;
    }
    release(&kmem.lock);

    if (r)
        memset((char *)r, 5, PGSIZE); // fill with junk
    return (void *)r;
}

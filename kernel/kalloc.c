// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
  struct kmem *next;
} kmem[NCPU];


void
kinit()
{
  char s[]="kmem0";
  for (int i = 0; i < NCPU; i++)
  {
    initlock(&kmem[i].lock, s);
    printf("init lock %d\n",i);
    s[4]=s[4]+1;
  }
  freerange(end, (void*)PHYSTOP);
}

void
kfree_switch(void *pa,int cpu_no)
{
  struct run *r;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem[cpu_no].lock);
  r->next = kmem[cpu_no].freelist;
  kmem[cpu_no].freelist = r;
  release(&kmem[cpu_no].lock);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(int i=0; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    {
      kfree_switch(p,i%NCPU);
      i++;
    }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)


void kfree(void *pa){
  push_off();
  int cpu_no=cpuid()%NCPU;
  pop_off();

  kfree_switch(pa,cpu_no);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc_switch(int cpu_no)
{
  struct run *r;

  acquire(&kmem[cpu_no].lock);
  r = kmem[cpu_no].freelist;
  if(r)
    kmem[cpu_no].freelist = r->next;
  release(&kmem[cpu_no].lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;

}

void * kalloc(void)
{
  push_off();
  int cpu_no=cpuid()%NCPU;
  pop_off();

  return kalloc_switch(cpu_no);
}

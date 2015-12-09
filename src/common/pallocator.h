#ifndef CEPH_PLACEMENT_ALLOCATOR_H
#define CEPH_PLACEMENT_ALLOCATOR_H

#include "include/atomic.h"
#include <assert.h>

class PlacementAllocatorBase
{
 public:
  virtual void* allocate(size_t sz) = 0;
  virtual void deallocate() = 0;
  virtual ~PlacementAllocatorBase() {};
};

template<bool sync>
class PlacementAllocatorBaseSync : public PlacementAllocatorBase
{
 protected:
   atomic_t cnt;

  void sync_inc() { cnt.inc(); }

 public:
  PlacementAllocatorBaseSync() : cnt(1) {}

  void deallocate() { if(!cnt.dec()) delete this; }

  ~PlacementAllocatorBaseSync() { assert(!cnt.read()); }
};

template<>
class PlacementAllocatorBaseSync<false> : public PlacementAllocatorBase
{
 protected:
   int cnt;

  void sync_inc() { cnt++; }

 public:
  PlacementAllocatorBaseSync() : cnt(1) {}

  void deallocate() { if(!--cnt) delete this; }

  ~PlacementAllocatorBaseSync() { assert(!cnt); }
};

template <int Ts, bool sync = false>
class PlacementAllocator: public PlacementAllocatorBaseSync<sync>
{
  char heap[Ts];
  char *ptr;

 public:
  PlacementAllocator() : ptr(heap + Ts) { };

  void* allocate(size_t sz)
  {
    PlacementAllocatorBaseSync<sync>::sync_inc();

    assert(ptr - sz >= heap);

    ptr -= sz;

    return ptr;
  }
};

inline
void* operator new(size_t sz, PlacementAllocatorBase& a)
{
  return a.allocate(sz);
}

#endif

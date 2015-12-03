// -*- mode:C++; tab-width:8; c-basic-offset:2; indent-tabs-mode:t -*-
// vim: ts=8 sw=2 smarttab
/*
 * Ceph - scalable distributed file system
 *
 * Copyright (C) 2015 Evgeniy Firsov <evgeniy.firsov@sandisk.com>
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */
#ifndef CEPH_STL_ALLOCATOR_H
#define CEPH_STL_ALLOCATOR_H

#include <sys/types.h>
#include <stdlib.h>
#include "common/likely.h"

#include <map>
#include <list>

namespace ceph {

class StlAllocatorBase { };

inline
bool operator==(const StlAllocatorBase& a, const StlAllocatorBase& b) { return &a == &b; }
inline
bool operator!=(const StlAllocatorBase& a, const StlAllocatorBase& b) { return &a != &b; }

template <class Tp, int N>
class StlAllocator : public StlAllocatorBase {
  char _heap[sizeof(Tp) * N];
  Tp* _ptr;

 public:
  typedef 	Tp  value_type;
  typedef 	Tp* pointer;
  typedef const Tp* const_pointer;
  typedef 	Tp& reference;
  typedef const Tp& const_reference;

  StlAllocator() : _ptr((Tp*)_heap + N) { };
  StlAllocator(const StlAllocatorBase& a __attribute__((__unused__))) : StlAllocator() { };

  template<class Tn> struct rebind { typedef StlAllocator<Tn, N> other; };

  Tp* allocate(size_t n)
  {
    if(unlikely(_ptr <= (Tp*)_heap))
      return (Tp*)malloc(n * sizeof(Tp));

    _ptr -= n;

    return _ptr;
  }

  void deallocate(Tp* p, size_t n __attribute__((__unused__)))
  {
    if(unlikely(p < (Tp*)_heap || p >= (Tp*)_heap + N))
      free(p);

    //TODO: implement local deallocation
  }

  template<typename... _Args>
  void construct(Tp* p, _Args&&... __args) { new ((void*) p) Tp(std::forward<_Args>(__args)...); }

  void destroy(Tp* p) { ((Tp*)p)->~Tp(); }
};

template<class Key, class T, class Compare = std::less<Key>, int cnt = 4> using ceph_map =
  std::map<Key, T, Compare, StlAllocator<std::pair<Key, T>, cnt> >;

template<class T, int cnt = 4> using ceph_list =
  std::list<T, StlAllocator<T, cnt> >;

}

#endif


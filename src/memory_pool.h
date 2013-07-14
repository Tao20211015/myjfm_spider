/*******************************************************************************
 * memory_pool.h - The memory pool implementation
 * This is for the big memory(>1k & < 8M) requirment
 * mainly used for the web page cache
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _MEMORY_POOL_H_
#define _MEMORY_POOL_H_

#include "config.h"
#include "mutex.h"

_START_MYJFM_NAMESPACE_

class MemoryPool {
public:
  static MemoryPool* get_instance();
  ~MemoryPool();
  
  void* get_memory(int);
  RES_CODE put_memory(void*);
  
  RES_CODE get_size(int, int&);

private:
  MemoryPool();
  MemoryPool& operator=(const MemoryPool&);
  
  RES_CODE adjust_size(int, int&);
  RES_CODE free_memory(int size);
  Map<int, Vector<void*> > _memories;
  
  mutable Mutex _mutex;
};

_END_MYJFM_NAMESPACE_

#endif


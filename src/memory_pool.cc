/*******************************************************************************
 * memory_pool.cc - The memory pool implementation
 * This is for the big memory(>1k & < 8M) requirment
 * mainly used for the web page cache
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>
#include <stdlib.h>

#include "config.h"
#include "memory_pool.h"
#include "mutex.h"

_START_MYJFM_NAMESPACE_

MemoryPool* MemoryPool::get_instance() {
  static MemoryPool mp;
  return &mp;
}

MemoryPool::MemoryPool() {
  _memories[1024 >> 3] = Vector<void*>();
  _memories[1024 >> 2] = Vector<void*>();
  _memories[1024 >> 1] = Vector<void*>();
  _memories[1024] = Vector<void*>();
  _memories[1024 << 1] = Vector<void*>();
  _memories[1024 << 2] = Vector<void*>();
  _memories[1024 << 3] = Vector<void*>();
  _memories[1024 << 4] = Vector<void*>();
  _memories[1024 << 5] = Vector<void*>();
  _memories[1024 << 6] = Vector<void*>();
  _memories[1024 << 7] = Vector<void*>();
  _memories[1024 << 8] = Vector<void*>();
  _memories[1024 << 9] = Vector<void*>();
  _memories[1024 << 10] = Vector<void*>();
  _memories[1024 << 11] = Vector<void*>();
  _memories[1024 << 12] = Vector<void*>();
  _memories[1024 << 13] = Vector<void*>();
}

MemoryPool::~MemoryPool() {
  free_memory(1024 >> 3);
  free_memory(1024 >> 2);
  free_memory(1024 >> 1);
  free_memory(1024);
  free_memory(1024 << 1);
  free_memory(1024 << 2);
  free_memory(1024 << 3);
  free_memory(1024 << 4);
  free_memory(1024 << 5);
  free_memory(1024 << 6);
  free_memory(1024 << 7);
  free_memory(1024 << 8);
  free_memory(1024 << 9);
  free_memory(1024 << 10);
  free_memory(1024 << 11);
  free_memory(1024 << 12);
  free_memory(1024 << 13);
  _memories.clear();
}

RES_CODE MemoryPool::free_memory(uint32_t size) {
  Vector<void*>& v = _memories[size];
  Vector<void*>::iterator itr;

  for (itr = v.begin(); itr != v.end(); ++itr) {
    void* buf = (char*)*itr - 4;
    free(buf);
  }

  v.clear();

  return S_OK;
}

void* MemoryPool::get_memory(uint32_t size) {
  void* ret = NULL;
  uint32_t new_size;
  adjust_size(size, new_size);

  if (new_size <= (1024 << 13)) {
    Mutex::ScopeGuard guard(&_mutex);
    Vector<void*>& v = _memories[new_size];

    if (v.size() > 0) {
      ret = v.back();
      v.pop_back();
    }
  }

  if (!ret) {
    ret = malloc(new_size + 4);

    if (ret) {
      uint32_t* pi = (uint32_t*)ret;
      *pi = new_size;
      ret = (char*)ret + 4;
    }
  }

  return ret;
}

RES_CODE MemoryPool::put_memory(void* memory) {
  if (!memory) {
    return S_FAIL;
  }

  uint32_t* pi = (uint32_t*)((char*)memory - 4);

  if (*pi > (1024 << 13)) {
    free(pi);
  } else {
    Mutex::ScopeGuard guard(&_mutex);
    _memories[*pi].push_back(memory);
  }

  return S_OK;
}

RES_CODE MemoryPool::adjust_size(uint32_t size, uint32_t& new_size) {
  if (size <= (1024 >> 3)) {
    new_size = 1024 >> 3;
  } else if (size <= (1024 >> 2)) {
    new_size = 1024 >> 2;
  } else if (size <= (1024 >> 1)) {
    new_size = 1024 >> 1;
  } else if (size <= 1024) {
    new_size = 1024;
  } else if (size <= (1024 << 1)) {
    new_size = 1024 << 1;
  } else if (size <= (1024 << 2)) {
    new_size = 1024 << 2;
  } else if (size <= (1024 << 3)) {
    new_size = 1024 << 3;
  } else if (size <= (1024 << 4)) {
    new_size = 1024 << 4;
  } else if (size <= (1024 << 5)) {
    new_size = 1024 << 5;
  } else if (size <= (1024 << 6)) {
    new_size = 1024 << 6;
  } else if (size <= (1024 << 7)) {
    new_size = 1024 << 7;
  } else if (size <= (1024 << 8)) {
    new_size = 1024 << 8;
  } else if (size <= (1024 << 9)) {
    new_size = 1024 << 9;
  } else if (size <= (1024 << 10)) {
    new_size = 1024 << 10;
  } else if (size <= (1024 << 11)) {
    new_size = 1024 << 11;
  } else if (size <= (1024 << 12)) {
    new_size = 1024 << 12;
  } else if (size <= (1024 << 13)) {
    new_size = 1024 << 13;
  } else {
    new_size = size;
  }

  return S_OK;
}

RES_CODE MemoryPool::get_size(uint32_t size, uint32_t& new_size) {
  Mutex::ScopeGuard guard(&_mutex);

  new_size = _memories[size].size();

  return S_OK;
}

_END_MYJFM_NAMESPACE_


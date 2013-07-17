/*******************************************************************************
 * squeue.h - thread-safe queue implementation
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _SQUEUE_H_
#define _SQUEUE_H_

#include <stdint.h>

#include "config.h"
#include "mutex.h"
#include "shared.h"
#include "conditional.h"

_START_MYJFM_NAMESPACE_

template <class T>
class SQueue : public Shared {
public:
  SQueue() {
    _cond.init(&_mutex);
  }

  ~SQueue() {}

  // can not use reference
  // because the caller may pass stack variable to this callee
  RES_CODE push(T t) {
    Mutex::ScopeGuard guard(&_mutex);
    _queue.push(t);
    _cond.signal();

    return S_OK;
  }

  RES_CODE pop(T& t) {
    Mutex::ScopeGuard guard(&_mutex);
    while (_queue.empty()) {
      _cond.wait();
    }

    t = _queue.front();
    _queue.pop();

    return S_OK;
  }

  RES_CODE size(uint32_t& s) {
    Mutex::ScopeGuard guard(&_mutex);
    s = _queue.size();

    return S_OK;
  }

private:
  // disallow copy constructor
  SQueue(const SQueue&);
  SQueue& operator=(const SQueue&);
  Queue<T> _queue;
  Mutex _mutex;
  Conditional _cond;
};

_END_MYJFM_NAMESPACE_

#endif


/*******************************************************************************
 * scope_guard.h - lock and unlock the mutex smartly
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _SCOPE_GUARD_H_
#define _SCOPE_GUARD_H_

#include <stdlib.h>

#include "config.h"

_START_MYJFM_NAMESPACE_

// use this template class to implement the smart mutex lock and unlock
template <class Lock>
class ScopeGuard {
public:
  ScopeGuard(Lock* lock) : _lock(lock) {
    if (_lock != NULL) {
      _lock->lock();
    }
  }

  ~ScopeGuard() {
    if (_lock != NULL) {
      _lock->unlock();
    }
  }

private:
  Lock* _lock;
};

_END_MYJFM_NAMESPACE_

#endif


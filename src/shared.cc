/*******************************************************************************
 * shared.cc - all classes inherited from this class can use sharedpointer
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include "config.h"
#include "shared.h"
#include "mutex.h"

_START_MYJFM_NAMESPACE_

Shared::Shared() : _count(0) {}

RES_CODE Shared::add_ref() {
  Mutex::ScopeGuard guard(&_mutex);
  _count++;

  return S_OK;
}

// the flag indicates whether the _count is decreased successfully
// this can avoid that two threads destruct the object at the sametime
unsigned int Shared::dec_ref(bool* flag) {
  Mutex::ScopeGuard guard(&_mutex);
  if (_count > 0) {
    if (flag) {
      *flag = true;
    }
    _count--;
  } else {
    if (flag) {
      *flag = false;
    }
  }
  return _count;
}

_END_MYJFM_NAMESPACE_


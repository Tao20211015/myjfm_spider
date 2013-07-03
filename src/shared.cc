#include "config.h"
#include "shared.h"
#include "scope_guard.h"

_START_MYJFM_NAMESPACE_

Shared::Shared() : _count(0) {}

RES_CODE Shared::add_ref() {
  ScopeGuard<Mutex> t(&_mutex);
  _count++;

  return S_OK;
}

// the flag indicates whether the _count is decreased successfully
// this can avoid that two threads destruct the object at the sametime
unsigned int Shared::dec_ref(bool* flag) {
  ScopeGuard<Mutex> t(&_mutex);
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


#include "config.h"
#include "shared.h"
#include "scopeguard.h"

_START_MYJFM_NAMESPACE_

Shared::Shared() : _count(0) {}

void Shared::add_ref() {
  Scopeguard<Mutex> t(&_mutex);
  _count++;
}

// the flag indicates whether the _count is decreased successfully
// this can avoid that two threads destruct the object at the sametime
unsigned int Shared::dec_ref(bool* flag) {
  Scopeguard<Mutex> t(&_mutex);
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


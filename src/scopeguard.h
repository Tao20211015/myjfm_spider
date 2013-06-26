#ifndef _SCOPEGUARD_H_
#define _SCOPEGUARD_H_

#include <stdlib.h>

#include "config.h"

_START_MYJFM_NAMESPACE_

// use this template class to implement the smart mutex lock and unlock
template <class Lock>
class Scopeguard {
public:
  Scopeguard(Lock* lock) : _lock(lock) {
    if (_lock != NULL) {
      _lock->lock();
    }
  }

  ~Scopeguard() {
    if (_lock != NULL) {
      _lock->unlock();
    }
  }
private:
  Lock* _lock;
};

_END_MYJFM_NAMESPACE_

#endif


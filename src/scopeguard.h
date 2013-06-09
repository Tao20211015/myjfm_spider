#ifndef _SCOPEGUARD_H_
#define _SCOPEGUARD_H_

#include <stdlib.h>

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

#endif


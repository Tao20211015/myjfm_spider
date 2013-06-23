#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include "config.h"
#include <pthread.h>
#include <stdlib.h>

_START_MYJFM_NAMESPACE_

class RWlock {
#define RUN_FUNC_IF_HAS_INIT(func) do { \
  if (_has_init && func(&_rwlock) != 0) { \
    abort(); \
  } \
} while (0)

public:
  RWlock() {
    _has_init = 1;
    pthread_rwlock_init(&_rwlock, NULL);
  }

  ~RWlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_rwlock_destroy);
  }

  inline void rdlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_rwlock_rdlock);
  }

  inline void wrlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_rwlock_wrlock);
  }

  // success if return 0
  // fail otherwise
  inline int try_rdlock() {
    if (!_has_init) {
      return 1;
    }

    return pthread_rwlock_tryrdlock(&_rwlock);
  }

  // success if return 0
  // fail otherwise
  inline int try_wrlock() {
    if (!_has_init) {
      return 1;
    }

    return pthread_rwlock_trywrlock(&_rwlock);
  }

  inline void unlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_rwlock_unlock);
  }

private:
  volatile int _has_init;
  // disallow copy constructor
  RWlock(const RWlock&);
  RWlock& operator=(const RWlock&);
  pthread_rwlock_t _rwlock;

#undef RUN_FUNC_IF_HAS_INIT
};

_END_MYJFM_NAMESPACE_

#endif


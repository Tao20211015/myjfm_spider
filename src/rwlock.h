#ifndef _RWLOCK_H_
#define _RWLOCK_H_

#include <pthread.h>
#include <stdlib.h>

#include "config.h"

_START_MYJFM_NAMESPACE_

class RWlock {
#define RUN_FUNC_IF_HAS_INIT(func) do { \
  if (!_has_init) { \
    Cerr << "[FATAL] rwlock has not been initialized" << Endl; \
    abort(); \
  } \
  \
  if (func(&_rwlock) != 0) { \
    Cerr << "[FATAL] " << #func << "() failed" << Endl; \
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

  inline RES_CODE rdlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_rwlock_rdlock);

    return S_OK;
  }

  inline RES_CODE wrlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_rwlock_wrlock);

    return S_OK;
  }

  inline RES_CODE try_rdlock() {
    if (!_has_init) {
      Cerr << "[FATAL] rwlock has not been initialized" << Endl;
      abort();
    }

    if (pthread_rwlock_tryrdlock(&_rwlock) == 0) {
      return S_OK;
    } else {
      return S_FAIL;
    }
  }

  inline RES_CODE try_wrlock() {
    if (!_has_init) {
      Cerr << "[FATAL] rwlock has not been initialized" << Endl;
      abort();
    }

    if (pthread_rwlock_trywrlock(&_rwlock) == 0) {
      return S_OK;
    } else {
      return S_FAIL;
    }
  }

  inline RES_CODE unlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_rwlock_unlock);

    return S_OK;
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


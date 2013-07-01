#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>
#include <stdlib.h>

#include "config.h"
#include "log.h"

_START_MYJFM_NAMESPACE_

class Mutex {
#define RUN_FUNC_IF_HAS_INIT(func) do { \
  if (!_has_init) { \
    LOG(FATAL, "mutex has not been initialized"); \
    abort(); \
  } \
  \
  if (func(&_mutex) != 0) { \
    String s = #func; \
    s += "() failed"; \
    LOG(FATAL, s.c_str()); \
    abort(); \
  } \
} while (0)

public:
  friend class Conditional;

  Mutex() {
    _has_init = 1;
    pthread_mutex_init(&_mutex, NULL);
  }

  ~Mutex() {
    RUN_FUNC_IF_HAS_INIT(pthread_mutex_destroy);
  }

  inline RES_CODE lock() {
    RUN_FUNC_IF_HAS_INIT(pthread_mutex_lock);

    return S_OK;
  }

  inline RES_CODE try_lock() {
    if (!_has_init) {
      LOG(FATAL, "mutex has not been initialized");
      abort();
    }

    if (pthread_mutex_trylock(&_mutex) == 0) {
      return S_OK;
    } else {
      return S_FAIL;
    }
  }

  inline RES_CODE unlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_mutex_unlock);

    return S_OK;
  }

private:
  volatile int _has_init;
  // disallow copy constructor
  Mutex(const Mutex&);
  Mutex& operator=(const Mutex&);
  pthread_mutex_t _mutex;

#undef RUN_FUNC_IF_HAS_INIT
};

_END_MYJFM_NAMESPACE_

#endif


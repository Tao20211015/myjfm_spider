#ifndef _MUTEX_H_
#define _MUTEX_H_

#include "config.h"
#include <pthread.h>
#include <stdlib.h>

_START_MYJFM_NAMESPACE_

class Mutex {
#define RUN_FUNC_IF_HAS_INIT(func) do { \
  if (_has_init && func(&_mutex) != 0) { \
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

  inline void lock() {
    RUN_FUNC_IF_HAS_INIT(pthread_mutex_lock);
  }

  // success if return 0
  // fail otherwise
  inline int try_lock() {
    if (!_has_init) {
      return 1;
    }

    return pthread_mutex_trylock(&_mutex);
  }

  inline void unlock() {
    RUN_FUNC_IF_HAS_INIT(pthread_mutex_lock);
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


#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "config.h"
#include <semaphore.h>
#include <stdlib.h>

_START_MYJFM_NAMESPACE_

// This is the semaphore class which can only be shared between threads
class Semaphore {
#define RUN_FUNC_IF_HAS_INIT(func) do { \
  if (_has_init && func(&_semaphore) != 0) { \
    abort(); \
  } \
} while (0)

public:
  // initv: the init value of the semaphore
  Semaphore(int initv = 0) {
    _has_init = 1;
    initv = initv < 0 ? 0 : initv;
    // the value of the second argument is 0 which means that this is an
    // inter-thread shared semaphore
    if (_has_init && sem_init(&_semaphore, 0, initv)) {
      abort();
    }
  }

  ~Semaphore() {
    RUN_FUNC_IF_HAS_INIT(sem_destroy);
  }

  inline void wait() {
    RUN_FUNC_IF_HAS_INIT(sem_wait);
  }

  inline void post() {
    RUN_FUNC_IF_HAS_INIT(sem_post);
  }

private:
  volatile int _has_init;
  Semaphore(const Semaphore&);
  Semaphore& operator=(const Semaphore&);
  sem_t _semaphore;

#undef RUN_FUNC_IF_HAS_INIT
};

_END_MYJFM_NAMESPACE_

#endif


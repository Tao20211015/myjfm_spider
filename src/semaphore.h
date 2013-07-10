/*******************************************************************************
 * semaphore.h - encapsulated the linux semaphore API
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include <semaphore.h>
#include <stdlib.h>

#include "config.h"
#include "log.h"

_START_MYJFM_NAMESPACE_

// This is the semaphore class which can only be shared between threads
class Semaphore {
#define RUN_FUNC_IF_HAS_INIT(func) do { \
  if (!_has_init) { \
    LOG(FATAL, "semaphore has not been initialized"); \
    abort(); \
  } \
  \
  if (func(&_semaphore) != 0) { \
    LOG(FATAL, "%s() failed", #func); \
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

  inline RES_CODE wait() {
    RUN_FUNC_IF_HAS_INIT(sem_wait);

    return S_OK;
  }

  inline RES_CODE post() {
    RUN_FUNC_IF_HAS_INIT(sem_post);

    return S_OK;
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


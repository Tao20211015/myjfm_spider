#ifndef _CONDITIONAL_H_
#define _CONDITIONAL_H_

#include <pthread.h>
#include <stdlib.h>

#include "config.h"
#include "mutex.h"

_START_MYJFM_NAMESPACE_

class Conditional {
public:
  enum _enum_state {
    UNCONSTRUCTED = 0, 
    CONSTRUCTED, 
    INITIALIZED
  };

  Conditional() {
    _state = CONSTRUCTED;
    _mutex = NULL;
    if (pthread_cond_init(&_cond, NULL)) {
      Cerr << "[FATAL] pthread_cond_init() failed" << Endl;
      abort();
    }
  }

  ~Conditional() {
    if (_state != UNCONSTRUCTED && 
        pthread_cond_destroy(&_cond)) {
      Cerr << "[FATAL] pthread_cond_destroy() failed" << Endl;
      abort();
    }
  }

  inline RES_CODE init(Mutex* mutex) {
    if (!mutex) {
      Cerr << "[FATAL] NULL pointer! Conditional.init() failed" << Endl;
      abort();
    }
    _state = INITIALIZED;
    _mutex = mutex;
    return S_OK;
  }

  inline RES_CODE wait() {
    if (_state == INITIALIZED && 
        pthread_cond_wait(&_cond, &(_mutex->_mutex))) {
      Cerr << "[FATAL] pthread_cond_wait() failed" << Endl;
      abort();
    }

    return S_OK;
  }

  inline RES_CODE signal() {
    if (_state == INITIALIZED && 
        pthread_cond_signal(&_cond)) {
      Cerr << "[FATAL] pthread_cond_signal() failed" << Endl;
      abort();
    }

    return S_OK;
  }

private:
  volatile _enum_state _state;
  // disallow copy constructor
  Conditional(const Conditional&);
  Conditional& operator=(const Conditional&);
  Mutex* _mutex;
  pthread_cond_t _cond;
};

_END_MYJFM_NAMESPACE_

#endif


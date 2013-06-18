#ifndef _CONDITIONAL_H_
#define _CONDITIONAL_H_

#include "config.h"
#include "mutex.h"
#include <pthread.h>
#include <stdlib.h>

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
      abort();
    }
  }

  ~Conditional() {
    if (_state != UNCONSTRUCTED && 
        pthread_cond_destroy(&_cond)) {
      abort();
    }
  }

  inline void init(Mutex* mutex) {
    if (!mutex) {
      abort();
    }
    _state = INITIALIZED;
    _mutex = mutex;
  }

  inline void wait() {
    if (_state == INITIALIZED && 
        pthread_cond_wait(&_cond, _mutex->_mutex)) {
      abort();
    }
  }

  inline void signal() {
    if (_state == INITIALIZED && 
        pthread_cond_signal(&_cond)) {
      abort();
    }
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


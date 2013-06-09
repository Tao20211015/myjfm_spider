#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>
#include <stdlib.h>

#define RUN_FUNC_IF_HAS_INIT(func) do { \
  if (_has_init && func(&_mutex) != 0) { \
    abort(); \
  } \
} while (0)

class Mutex {
public:
  Mutex();
  ~Mutex();
  inline void lock();
  // success if return 0
  // fail otherwise
  inline int try_lock();
  inline void unlock();
private:
  volatile int _has_init;
  // disallow copy constructor
  Mutex(const Mutex&);
  Mutex& operator=(const Mutex&);
  pthread_mutex_t _mutex;
};

Mutex::Mutex() {
  _has_init = 1;
  pthread_mutex_init(&_mutex, NULL);
}

Mutex::~Mutex() {
  RUN_FUNC_IF_HAS_INIT(pthread_mutex_destroy);
}

void Mutex::lock() {
  RUN_FUNC_IF_HAS_INIT(pthread_mutex_lock);
}

int Mutex::try_lock() {
  if (!_has_init) {
    return 1;
  }
  return pthread_mutex_trylock(&_mutex);
}

void Mutex::unlock() {
  RUN_FUNC_IF_HAS_INIT(pthread_mutex_lock);
}

#endif


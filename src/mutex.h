#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <pthread.h>

class Mutex {
public:
  Mutex();
  ~Mutex();
  int lock();
  int try_lock();
  int unlock();
private:
  // can not copy and assignment
  Mutex(const Mutex&);
  Mutex& operator=(const Mutex&);
  pthread_mutex_t _mutex;
};

#endif


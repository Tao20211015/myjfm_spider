#include "mutex.h"
#include <pthread.h>
#include <stdlib.h>

Mutex::Mutex() {
  pthread_mutex_init(&_mutex, NULL);
}

Mutex::~Mutex() {
  pthread_mutex_destroy(&_mutex);
}

int Mutex::lock() {
  return pthread_mutex_lock(&_mutex);
}

int Mutex::try_lock() {
  return pthread_mutex_trylock(&_mutex);
}

int Mutex::unlock() {
  return pthread_mutex_unlock(&_mutex);
}


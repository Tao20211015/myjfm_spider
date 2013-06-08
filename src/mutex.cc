#include "mutex.h"

Mutex::Mutex() {
}

Mutex::~Mutex() {
}

int Mutex::lock() {
  return 1;
}

int Mutex::try_lock() {
  return 1;
}

int Mutex::unlock() {
  return 1;
}


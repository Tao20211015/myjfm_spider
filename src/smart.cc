#include "smart.h"
#include "scopeguard.h"

Smart::Smart() : _count(0) {}

void Smart::add_ref() {
  ScopeGuard<Mutex> t(&_mutex);
  _count++;
}

unsigned int Smart::dec_ref() {
  ScopeGuard<Mutex> t(&_mutex);
  if (_count > 0) {
    _count--;
  }
  return _count--;
}


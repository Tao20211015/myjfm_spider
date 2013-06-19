#ifndef _SQUEUE_H_
#define _SQUEUE_H_

#include "config.h"
#include "scopeguard.h"
#include "mutex.h"
#include "conditional.h"
#include <queue>

_START_MYJFM_NAMESPACE_

template <class T>
class Squeue : public Shared{
public:
  Squeue() {
    _cond.init(&_mutex);
  }

  ~Squeue() {}

  // can not use reference
  // because the caller may pass stack variable to this callee
  void push(T t) {
    Scopeguard<Mutex> lock(&_mutex);
    _queue.push(t);
    _cond.signal();
  }

  void pop(T& t) {
    Scopeguard<Mutex> lock(&_mutex);
    while (_queue.empty()) {
      _cond.wait();
    }
    t = _queue.front();
  }

  int size() {
    Scopeguard<Mutex> lock(&_mutex);
    return _queue.size();
  }

private:
  // disallow copy constructor
  Squeue(const Squeue&);
  Squeue& operator=(const Squeue&);
  std::queue<T> _queue;
  Mutex _mutex;
  Conditional _cond;
};

_END_MYJFM_NAMESPACE_

#endif


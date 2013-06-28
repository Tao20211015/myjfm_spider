#ifndef _SQUEUE_H_
#define _SQUEUE_H_

#include "config.h"
#include "mutex.h"
#include "shared.h"
#include "scopeguard.h"
#include "conditional.h"

_START_MYJFM_NAMESPACE_

template <class T>
class Squeue : public Shared {
public:
  Squeue() {
    _cond.init(&_mutex);
  }

  ~Squeue() {}

  // can not use reference
  // because the caller may pass stack variable to this callee
  RES_CODE push(T t) {
    Scopeguard<Mutex> lock(&_mutex);
    _queue.push(t);
    _cond.signal();

    return S_OK;
  }

  RES_CODE pop(T& t) {
    Scopeguard<Mutex> lock(&_mutex);
    while (_queue.empty()) {
      _cond.wait();
    }

    t = _queue.front();
    _queue.pop();

    return S_OK;
  }

  RES_CODE size(int& s) {
    Scopeguard<Mutex> lock(&_mutex);
    s = _queue.size();

    return S_OK;
  }

private:
  // disallow copy constructor
  Squeue(const Squeue&);
  Squeue& operator=(const Squeue&);
  Queue<T> _queue;
  Mutex _mutex;
  Conditional _cond;
};

_END_MYJFM_NAMESPACE_

#endif


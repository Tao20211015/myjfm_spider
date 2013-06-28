#include "config.h"
#include "task.h"
#include "thread.h"
#include "threadpool.h"
#include "threadtask.h"
#include "sharedpointer.h"

_START_MYJFM_NAMESPACE_

Threadpool::Threadpool(int n) : _n(n), _state(CONSTRUCTED) {}

Threadpool::~Threadpool() {
  stop();
}

RES_CODE Threadpool::init() {
  int retry = 0;

  if (_state == CONSTRUCTED) {
    int i;
    for (i = 0; i < _n; ++i) {
      if (add_worker()) {
        if (retry >= MAX_RETRY) {
          return S_FAIL;
        } else {
          retry++;
          i--;
        }
      }
    }
    return S_OK;
  }

  return S_NOT_CONSTRUCTED;
}

RES_CODE Threadpool::stop() {
  int i;
  for (i = 0; i < _threads.size(); ++i) {
    _threads[i]->stop_blocking();
  }

  return S_OK;
}

RES_CODE Threadpool::size(int& s) {
  s = _n;
  return S_OK;
}

RES_CODE Threadpool::add_task(Sharedpointer<Task> task) {
  if (!task.is_null()) {
    return _tasks.push(task);
  }

  return S_BAD_ARG;
}

RES_CODE Threadpool::get_task(Sharedpointer<Task>& task) {
  return _tasks.pop(task);
}

RES_CODE Threadpool::add_worker() {
  Sharedpointer<Threadtask> threadtask(new Threadtask(this));
  Sharedpointer<Thread> thread = Threadfactory::create_thread(threadtask);

  if (thread.is_null()) {
    return S_THREAD_CREATE_FAILED;
  }

  _threads.push_back(thread);

  return S_OK;
}

_END_MYJFM_NAMESPACE_


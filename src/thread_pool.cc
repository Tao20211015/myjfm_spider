#include "config.h"
#include "task.h"
#include "thread.h"
#include "thread_pool.h"
#include "thread_task.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

ThreadPool::ThreadPool(int n) : _n(n), _state(CONSTRUCTED) {}

ThreadPool::~ThreadPool() {
  stop();
}

RES_CODE ThreadPool::init() {
  int retry = 0;

  if (_state == CONSTRUCTED) {
    int i;
    for (i = 0; i < _n; ++i) {
      if (add_worker() != S_OK) {
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

RES_CODE ThreadPool::stop() {
  int i;
  for (i = 0; i < _threads.size(); ++i) {
    _threads[i]->stop_blocking();
  }

  return S_OK;
}

RES_CODE ThreadPool::size(int& s) {
  s = _n;
  return S_OK;
}

RES_CODE ThreadPool::add_task(SharedPointer<Task> task) {
  if (!task.is_null()) {
    return _tasks.push(task);
  }

  return S_BAD_ARG;
}

RES_CODE ThreadPool::get_task(SharedPointer<Task>& task) {
  return _tasks.pop(task);
}

RES_CODE ThreadPool::add_worker() {
  SharedPointer<ThreadTask> threadtask(new ThreadTask(this));
  SharedPointer<Thread> thread = ThreadFactory::create_thread(threadtask);

  if (thread.is_null()) {
    return S_THREAD_CREATE_FAILED;
  }

  _threads.push_back(thread);

  return S_OK;
}

_END_MYJFM_NAMESPACE_


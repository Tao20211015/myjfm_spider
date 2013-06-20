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

int Threadpool::init() {
  int retry = 0;
  if (_state == CONSTRUCTED) {
    int i;
    for (i = 0; i < _n; ++i) {
      if (add_worker()) {
        if (retry >= MAX_RETRY) {
          return 1;
        } else {
          retry++;
          i--;
        }
      }
    }
    return 0;
  }
  return 1;
}

void Threadpool::stop() {
  int i;
  for (i = 0; i < _threads.size(); ++i) {
    _threads[i]->stop_blocking();
  }
}

size_t Threadpool::size() {
  return _n;
}

void Threadpool::add_task(Sharedpointer<Task> task) {
  if (!task.is_null()) {
    _tasks.push(task);
  }
}

void Threadpool::get_task(Sharedpointer<Task>& task) {
  _tasks.pop(task);
}

int Threadpool::add_worker() {
  Sharedpointer<Threadtask> threadtask(new Threadtask(this));
  Sharedpointer<Thread> thread = Threadfactory::create_thread(threadtask);
  if (thread.is_null()) {
    return 1;
  }
  _threads.push_back(thread);
  return 0;
}

_END_MYJFM_NAMESPACE_


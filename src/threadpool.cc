#include "config.h"
#include "task.h"
#include "threadpool.h"
#include "threadtask.h"
#include "sharedpointer.h"

_START_MYJFM_NAMESPACE_

Threadpool::Threadpool(int n) : _n(n), _state(CONSTRUCTED) {}

Threadpool::~Threadpool() {
  stop();
}

void Threadpool::run() {
  if (_state == CONSTRUCTED) {
    int i;
    for (i = 0; i < _n; ++i) {
      add_worker();
    }
  }
}

void Threadpool::add_task(Sharedpointer<Task> task) {
  if (!task.is_null()) {
    _tasks.push(task);
  }
}

void Threadpool::get_one_task(Sharedpointer<Task>& task) {
  _tasks.pop(task);
}

_END_MYJFM_NAMESPACE_


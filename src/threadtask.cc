#include "config.h"
#include "task.h"
#include "threadtask.h"
#include "threadpool.h"
#include "sharedpointer.h"

_START_MYJFM_NAMESPACE_

Threadtask::Threadtask(Threadpool* threadpool) : _threadpool(threadpool) {}

// This functor get the task from the _tasks
// It never stops unless the parent thread canceled it
RES_CODE Threadtask::operator()(void* arg) {
  if (_threadpool) {
    for(;;) {
      Sharedpointer<Task> task;
      // if there is no task, it will block
      _threadpool->get_task(task);
      if (!task.is_null()) {
        (*task)();
      }
    }
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_


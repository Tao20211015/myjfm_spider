#include "config.h"
#include "task.h"
#include "thread_task.h"
#include "thread_pool.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

ThreadTask::ThreadTask(ThreadPool* threadpool) : _threadpool(threadpool) {}

// This functor get the task from the _tasks
// It never stops unless the parent thread canceled it
RES_CODE ThreadTask::operator()(void* arg) {
  if (_threadpool) {
    for(;;) {
      SharedPointer<Task> task;
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


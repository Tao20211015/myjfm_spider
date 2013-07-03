#ifndef _THREAD_TASK_H_
#define _THREAD_TASK_H_

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class ThreadPool;

class ThreadTask : public Task {
public:
  ThreadTask(ThreadPool* threadpool = NULL);
  virtual RES_CODE operator()(void* arg = NULL);

private:
  ThreadPool* _threadpool;
};

_END_MYJFM_NAMESPACE_

#endif


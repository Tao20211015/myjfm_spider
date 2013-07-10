/*******************************************************************************
 * thread_task.h - thread task implementation
 * thread can only execute thread task, thread task get various kinds of tasks,
 * and then executes it
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

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


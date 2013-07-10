/*******************************************************************************
 * scheduler_task.h - the scheduler module implementation
 * each scheduler is a thread. It is responsible for the url scheduling and all
 * the other jobs.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _SCHEDULER_TASK_H_
#define _SCHEDULER_TASK_H_

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class SchedulerTask : public Task {
public:
  SchedulerTask(int);
  ~SchedulerTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _id;
  int _downloader_num;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _downloader_queue;

  RES_CODE init();
};

_END_MYJFM_NAMESPACE_

#endif


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
};

_END_MYJFM_NAMESPACE_

#endif


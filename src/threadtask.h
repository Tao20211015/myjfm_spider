#ifndef _THREADTASK_H_
#define _THREADTASK_H_

#include "config.h"
#include "task.h"

_START_MYJFM_NAMESPACE_

class Threadpool;

class Threadtask : public Task {
public:
  Threadtask(Threadpool* threadpool = NULL);
  virtual int operator()(void* arg = NULL);

private:
  Threadpool* _threadpool;
};

_END_MYJFM_NAMESPACE_

#endif


#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "config.h"
#include "shared.h"
#include "task.h"
#include "thread.h"
#include "squeue.h"
#include "threadtask.h"

_START_MYJFM_NAMESPACE_

class Threadpool : public Shared {
#define MAX_RETRY 10

  enum _enum_state {
    UNCONSTRUCTED, 
    CONSTRUCTED = 0x1111
  };

public:
  Threadpool(int n);
  ~Threadpool();

  RES_CODE init();

  RES_CODE stop();

  RES_CODE size(int&);

  RES_CODE add_task(Sharedpointer<Task> task);

  RES_CODE get_task(Sharedpointer<Task>& task);

private:
  int _n;
  int _state;
  Vector< Sharedpointer<Thread> > _threads;
  Squeue< Sharedpointer<Task> > _tasks;

  RES_CODE add_worker();
};

_END_MYJFM_NAMESPACE_

#endif


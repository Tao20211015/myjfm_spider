#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "config.h"
#include "shared.h"
#include "task.h"
#include "thread.h"
#include "squeue.h"
#include "thread_task.h"

_START_MYJFM_NAMESPACE_

class ThreadPool : public Shared {
#define MAX_RETRY 10

  enum _enum_state {
    UNCONSTRUCTED, 
    CONSTRUCTED = 0x1111
  };

public:
  ThreadPool(int n);
  ~ThreadPool();

  RES_CODE init();

  RES_CODE stop();

  RES_CODE size(int&);

  RES_CODE add_task(SharedPointer<Task> task);

  RES_CODE get_task(SharedPointer<Task>& task);

private:
  int _n;
  int _state;
  Vector< SharedPointer<Thread> > _threads;
  SQueue< SharedPointer<Task> > _tasks;

  RES_CODE add_worker();
};

_END_MYJFM_NAMESPACE_

#endif


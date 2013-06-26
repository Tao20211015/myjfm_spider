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

  int init();

  void stop();

  size_t size();

  void add_task(Sharedpointer<Task> task);

  void get_task(Sharedpointer<Task>& task);

private:
  int _n;
  int _state;
  Vector< Sharedpointer<Thread> > _threads;
  Squeue< Sharedpointer<Task> > _tasks;

  // return: 0, success; 1, error
  int add_worker();
};

_END_MYJFM_NAMESPACE_

#endif


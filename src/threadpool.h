#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "config.h"
#include "shared.h"
#include "task.h"
#include "thread.h"
#include "squeue.h"
#include "threadtask.h"
#include <vector>

_START_MYJFM_NAMESPACE_

class Threadpool : public Shared {
  enum _enum_state {
    UNCONSTRUCTED, 
    CONSTRUCTED = 0x1111
  };

public:
  Threadpool(int n);
  ~Threadpool();

  void run();

  void join();

  void stop();

  void add_task(Sharedpointer<Task> task);

  void get_one_task(Sharedpointer<Task>& task);

private:
  int _n;
  int _state;
  std::vector< Sharedpointer<Thread> > _threads;
  Squeue< Sharedpointer<Task> > _tasks;

  void add_worker();
};

_END_MYJFM_NAMESPACE_

#endif


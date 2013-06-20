#ifndef _THREAD_H_
#define _THREAD_H_

#include "config.h"
#include "shared.h"
#include "task.h"
#include "semaphore.h"
#include "sharedpointer.h"
#include <pthread.h>

_START_MYJFM_NAMESPACE_

// the function body that the thread will use to execute
typedef void* (*thread_task_func)(void*);

class Thread : public Shared {
  enum _enum_state {
    INIT, 
    RUNNING, 
    JOINED, 
    STOP
  };

public:
  // run the task
  Thread(const Sharedpointer<Task>& task, int detached = 0);

  // run the task routine body
  Thread(thread_task_func, void* arg, int detached = 0);

  ~Thread();

  // start the execution
  int start();

  // stop the execution
  int stop();

  // wait this thread until exiting
  int join();

  // get the thread id via pthread_self()
  pthread_t gettid() const;

private:
  static void* thread_core_shell(void*);

  int _use_task;
  Sharedpointer<Task> _task;

  thread_task_func _task_func;
  void* _func_arg;

  pthread_t _tid;

  _enum_state _state;

  int _detached;

  // syncronize between main thread and child thread
  Semaphore _semaphore;
};

// use this factory to generate threads
class Threadfactory {
public:
  static Sharedpointer<Thread> create_thread(Sharedpointer<Task> task);

private:
  Threadfactory(const Threadfactory&);
  Threadfactory& operator=(const Threadfactory&);
};

_END_MYJFM_NAMESPACE_

#endif


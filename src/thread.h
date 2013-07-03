#ifndef _THREAD_H_
#define _THREAD_H_

#include <pthread.h>

#include "config.h"
#include "shared.h"
#include "task.h"
#include "semaphore.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

// the function body that the thread will use to execute
typedef void* (*thread_task_func)(void*);

class Thread : public Shared {
  enum _enum_state {
    INIT, 
    RUNNING, 
    STOP
  };

public:
  // run the task
  Thread(const SharedPointer<Task>& task, int detached = 0);

  // run the task routine body
  Thread(thread_task_func, void* arg, int detached = 0);

  ~Thread();

  // start the execution
  RES_CODE start();

  // stop the execution
  // invoke the pthread_cancel() and return immediately
  // do not wait the thread finishing
  RES_CODE stop_nonblocking();

  // stop the execution
  // invoke the pthread_cancel() and then invoke the
  // pthread_join(), so will block until this thread finished
  RES_CODE stop_blocking();

  // wait this thread until exiting
  RES_CODE join();

  // get the thread id via pthread_self()
  RES_CODE gettid(pthread_t&) const;

private:
  static void* thread_core_shell(void*);

  int _use_task;
  SharedPointer<Task> _task;

  thread_task_func _task_func;
  void* _func_arg;

  pthread_t _tid;

  _enum_state _state;

  int _detached;

  // syncronize between main thread and child thread
  Semaphore _semaphore;
};

// use this factory to generate threads
class ThreadFactory {
public:
  static SharedPointer<Thread> create_thread(SharedPointer<Task> task);

private:
  ThreadFactory(const ThreadFactory&);
  ThreadFactory& operator=(const ThreadFactory&);
};

_END_MYJFM_NAMESPACE_

#endif


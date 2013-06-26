#include "config.h"
#include "thread.h"

_START_MYJFM_NAMESPACE_

Thread::Thread(const Sharedpointer<Task>& task, int detached) : 
  _use_task(1), 
  _task(task), 
  _task_func(NULL), 
  _func_arg(NULL), 
  _tid(-1), 
  _state(INIT), 
  _detached(detached) 
{}

Thread::Thread(thread_task_func func, void* arg, int detached) : 
  _use_task(0), 
  _task_func(func), 
  _func_arg(arg), 
  _tid(-1), 
  _state(INIT), 
  _detached(detached)
{}

Thread::~Thread() {
  // wait until the thread execution is finished
  join();
  _state = STOP;
}

int Thread::start() {
  int res = 1;
  if (_state != INIT) {
    return res;
  }

  pthread_attr_t attr;                                                       
  res = pthread_attr_init(&attr);                                            
  if (res) {                                                            
    return res;                                                          
  }                                                                          

  res = pthread_attr_setdetachstate(&attr,                                   
      _detached ? PTHREAD_CREATE_DETACHED : PTHREAD_CREATE_JOINABLE); 
  if (res) {                                                            
    pthread_attr_destroy(&attr);                                           
    return res;                                                          
  }                                                                          
  
  // create thread
  // use the shell function to encapsulate the runner and func
  res = pthread_create(&_tid, &attr, thread_core_shell, this);  

  if (!res) {
    _semaphore.wait();
  }

  pthread_attr_destroy(&attr);                                               

  if (!res) {                                                            
    _state = RUNNING;
  }                                                                          

  return res;
}

int Thread::join() {
  int res = 1;
  if (_state != RUNNING) {
    return res;
  }
  if (pthread_self() != _tid) {
    int tres = pthread_join(_tid, NULL);
    if (!tres) {
      _state = STOP;
    }
    res = tres;
  }
  return res;
}

int Thread::stop_nonblocking() {
  switch (_state) {
    case INIT:
    case STOP:
      return 1;
    case RUNNING:
      if (pthread_cancel(_tid)) {
        return 1;
      }
      _state = STOP;
      return 0;
    default:
      return 1;
  }
}

int Thread::stop_blocking() {
  switch (_state) {
    case INIT:
    case STOP:
      return 1;
    case RUNNING:
      if (pthread_cancel(_tid)) {
        return 1;
      }
      if (join()) {
        return 1;
      }
      return 0;
    default:
      return 1;
  }
}

pthread_t Thread::gettid() const {
  return _tid;
}

void* Thread::thread_core_shell(void* arg) {
  Thread* tthis = (Thread*)arg;

  int use_task = tthis->_use_task;
  Sharedpointer<Task> task = tthis->_task;
  thread_task_func task_func = tthis->_task_func;
  void* func_arg = tthis->_func_arg;

  tthis->_semaphore.post();

  if (use_task && !task.is_null()) {
    // actually, it will execute Threadtask()
    (*task)();
  } else if (!use_task && task_func) {
    task_func(func_arg);
  }
  return NULL;
}

Sharedpointer<Thread> Threadfactory::create_thread(Sharedpointer<Task> task) {
  Sharedpointer<Thread> thread;
  if (!task.is_null()) {
    Sharedpointer<Thread> t(new Thread(task));
    if (!(t->start())) {
      thread = t;
    }
  }
  return thread;
}

_END_MYJFM_NAMESPACE_


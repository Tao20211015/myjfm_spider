#include "config.h"
#include "task.h"
#include "thread.h"
#include "thread_pool.h"
#include "shared_pointer.h"

_START_MYJFM_NAMESPACE_

class PrintTask : public Task {
public:
  PrintTask(String& value) : _value(value) {}
  virtual RES_CODE operator()(void* arg = NULL) {
    Cout << _value << Endl;
    return S_OK;
  }

private:
  String _value;
};

_END_MYJFM_NAMESPACE_

void* print_func(void* value) {
  Cout << *((String*)value) << Endl;
  return (void*)0;
}

int main() {
  typedef _MYJFM_NAMESPACE_::Thread \
    Thread;
  typedef _MYJFM_NAMESPACE_::ThreadPool \
    ThreadPool;
  typedef _MYJFM_NAMESPACE_::SharedPointer<ThreadPool> \
    ThreadPoolP;
  typedef _MYJFM_NAMESPACE_::PrintTask \
    PrintTask;
  typedef _MYJFM_NAMESPACE_::SharedPointer<PrintTask> \
    PrintTaskP;

  ThreadPoolP threadpool(new ThreadPool(2));
  threadpool->init();
  String string1 = "This is the first task";
  String string2 = "This is the second task";
  PrintTaskP task1(new PrintTask(string1));
  PrintTaskP task2(new PrintTask(string2));
  threadpool->add_task(task1);
  threadpool->add_task(task2);


  String string3 = "This is the third task";
  Thread thread3(print_func, (void*)&string3);
  thread3.start();

  String string4 = "This is the fourth task";
  PrintTaskP task4(new PrintTask(string4));
  Thread thread4(task4);
  thread4.start();

  return 0;
}


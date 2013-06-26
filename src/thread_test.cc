#include "config.h"
#include "task.h"
#include "thread.h"
#include "threadpool.h"
#include "sharedpointer.h"

_START_MYJFM_NAMESPACE_

class Printtask : public Task {
public:
  Printtask(String& value) : _value(value) {}
  virtual int operator()(void* arg = NULL) {
    Cout << _value << Endl;
    return 0;
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
  typedef _MYJFM_NAMESPACE_::Threadpool \
    Threadpool;
  typedef _MYJFM_NAMESPACE_::Sharedpointer<Threadpool> \
    Threadpool_p;
  typedef _MYJFM_NAMESPACE_::Printtask \
    Printtask;
  typedef _MYJFM_NAMESPACE_::Sharedpointer<Printtask> \
    Printtask_p;

  Threadpool_p threadpool(new Threadpool(2));
  threadpool->init();
  String string1 = "This is the first task";
  String string2 = "This is the second task";
  Printtask_p task1(new Printtask(string1));
  Printtask_p task2(new Printtask(string2));
  threadpool->add_task(task1);
  threadpool->add_task(task2);


  String string3 = "This is the third task";
  Thread thread3(print_func, (void*)&string3);
  thread3.start();

  String string4 = "This is the fourth task";
  Printtask_p task4(new Printtask(string4));
  Thread thread4(task4);
  thread4.start();

  return 0;
}


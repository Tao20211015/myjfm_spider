#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "sharedpointer.h"
#include "scheduletask.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

Scheduletask::Scheduletask(int id) : _id(id) {}

Scheduletask::~Scheduletask() {}

int Scheduletask::operator()(void* arg) {
  int i = 0;
  for (;;) {
    Cout << "[" << i++ << "] [Scheduler]: I'm thread " << _id << Endl;
    sleep(1);
  }

  return 0;
}

#if 0
int Scheduletask::operator()(void* arg) {
  return 0;
}
#endif

_END_MYJFM_NAMESPACE_


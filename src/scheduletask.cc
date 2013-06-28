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

RES_CODE Scheduletask::operator()(void* arg) {
  int i = 0;
  for (;;) {
    Cout << "[" << i++ << "] [Scheduler]: I'm thread " << _id << Endl;
    sleep(1);
  }

  return S_OK;
}

#if 0
RES_CODE Scheduletask::operator()(void* arg) {
  return S_OK;
}
#endif

_END_MYJFM_NAMESPACE_


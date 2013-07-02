#include <stdlib.h>
#include <unistd.h>

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
  char buffer[100];
  sprintf(buffer, "%s %d", "#Scheduler#", _id);
  for (;;) {
    LOG(INFO, buffer);
    usleep(1000);
  }

  return S_OK;
}

#if 0
RES_CODE Scheduletask::operator()(void* arg) {
  return S_OK;
}
#endif

_END_MYJFM_NAMESPACE_


#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "shared_pointer.h"
#include "scheduler_task.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

SchedulerTask::SchedulerTask(int id) : _id(id) {}

SchedulerTask::~SchedulerTask() {}

RES_CODE SchedulerTask::operator()(void* arg) {
  for (;;) {
    LOG(INFO, "#Scheduler# %d", _id);
    usleep(1000);
  }

  return S_OK;
}

#if 0
RES_CODE SchedulerTask::operator()(void* arg) {
  return S_OK;
}
#endif

_END_MYJFM_NAMESPACE_


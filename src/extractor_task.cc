#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "global.h"
#include "extractor_task.h"
#include "log.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

ExtractorTask::ExtractorTask(int id) : _id(id) {}

ExtractorTask::~ExtractorTask() {}

RES_CODE ExtractorTask::operator()(void* arg) {
  for (;;) {
    LOG(INFO, "#Extractor# %d", _id);
    usleep(1000);
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_


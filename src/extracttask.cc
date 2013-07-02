#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "sharedpointer.h"
#include "extracttask.h"
#include "log.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

Extracttask::Extracttask(int id) : _id(id) {}

Extracttask::~Extracttask() {}

RES_CODE Extracttask::operator()(void* arg) {
  char buffer[100];
  sprintf(buffer, "%s %d", "#Extractor#", _id);
  for (;;) {
    LOG(INFO, buffer);
    usleep(1000);
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_


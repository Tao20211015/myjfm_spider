#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "sharedpointer.h"
#include "extracttask.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

Extracttask::Extracttask(int id) : _id(id) {}

Extracttask::~Extracttask() {}

int Extracttask::operator()(void* arg) {
  int i = 0;
  for (;;) {
    Cout << "[" << i++ << "] [Extractor]: I'm thread " << _id << Endl;
    sleep(1);
  }

  return 0;
}

_END_MYJFM_NAMESPACE_


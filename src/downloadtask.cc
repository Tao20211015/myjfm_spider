#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "sharedpointer.h"
#include "downloadtask.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

Downloadtask::Downloadtask(int id) : _id(id) {}

Downloadtask::~Downloadtask() {}

#if 0
int Downloadtask::operator()(void* arg) {
  int i = 0;
  for (;;) {
    Cout << "[" << i++ << "] [Downloader]: I'm thread " << _id << Endl;
    sleep(1);
  }

  return 0;
}
#endif

int Downloadtask::operator()(void* arg) {
  Sharedpointer<Squeue<Url> > queue = glob->get_downloader_queue(_id);
  Url url;
  queue->pop(url);
  return 0;
}

_END_MYJFM_NAMESPACE_


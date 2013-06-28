#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "sharedpointer.h"
#include "downloadtask.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

Downloadtask::Downloadtask(int id) : 
  _id(id), 
  _url_queue(Sharedpointer<Squeue<Sharedpointer<Url> > >(NULL))
{}

Downloadtask::~Downloadtask() {}

RES_CODE Downloadtask::operator()(void* arg) {
   glob->get_downloader_queue(_id, _url_queue);

  Sharedpointer<Url> url_p;
  _url_queue->pop(url_p);

  return S_OK;
}

_END_MYJFM_NAMESPACE_


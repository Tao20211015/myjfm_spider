#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "shared_pointer.h"
#include "downloader_task.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

DownloaderTask::DownloaderTask(int id) : 
  _id(id), 
  _url_queue(SharedPointer<SQueue<SharedPointer<Url> > >(NULL))
{}

DownloaderTask::~DownloaderTask() {}

RES_CODE DownloaderTask::operator()(void* arg) {
   glob->get_downloader_queue(_id, _url_queue);

  SharedPointer<Url> url_p;
  _url_queue->pop(url_p);

  return S_OK;
}

_END_MYJFM_NAMESPACE_


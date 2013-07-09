#include <stdlib.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "event_loop.h"
#include "shared_pointer.h"
#include "downloader_task.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

DownloaderTask::DownloaderTask(int id) : 
  _id(id), 
  _dns_cache(SharedPointer<DnsCache>(NULL)), 
  _url_queue(SharedPointer<SQueue<SharedPointer<Url> > >(NULL)), 
  _event_loop(SharedPointer<EventLoop>(NULL))
{}

DownloaderTask::~DownloaderTask() {}

RES_CODE DownloaderTask::operator()(void* arg) {
  set_url_queue();
  init_dns_cache();
  init_event_loop();

  /*
  SharedPointer<Url> url_p;
  _url_queue->pop(url_p);
  */

  return S_OK;
}

RES_CODE DownloaderTask::set_url_queue() {
  return glob->get_downloader_queue(_id, _url_queue);
}

RES_CODE DownloaderTask::init_dns_cache() {
  SharedPointer<DnsCache> tmp_cache(new DnsCache());
  _dns_cache = tmp_cache;

  if (_dns_cache.is_null()) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE DownloaderTask::init_event_loop() {
  SharedPointer<EventLoop> tmp_event_loop(new EventLoop());
  _event_loop = tmp_event_loop;

  if (_event_loop.is_null()) {
    return S_FAIL;
  }

  return _event_loop->init();
}

_END_MYJFM_NAMESPACE_


#ifndef _DOWNLOADER_TASK_H_
#define _DOWNLOADER_TASK_H_

#include "config.h"
#include "task.h"
#include "dns_cache.h"
#include "shared_pointer.h"
#include "event_loop.h"
#include "callback.h"

_START_MYJFM_NAMESPACE_

// when some socket can be read, then executes the read callback functor
class ReadCallback : public Callback {
public:
  ReadCallback(int fd) : _fd(fd) {}

  ~ReadCallback() {};

  virtual RES_CODE operator()(void* arg = NULL) {
    return S_OK;
  }

private:
  int _fd;
};

// when some socket can be written, then executes the write callback functor
class WriteCallback : public Callback {
public:
  WriteCallback(int fd) : _fd(fd) {}

  ~WriteCallback() {};

  virtual RES_CODE operator()(void* arg = NULL) {
    return S_OK;
  }

private:
  int _fd;
};

// when some socket occurred an error, then executes the error callback functor
class ErrorCallback : public Callback {
public:
  ErrorCallback(int fd) : _fd(fd) {}

  ~ErrorCallback() {};

  virtual RES_CODE operator()(void* arg = NULL) {
    return S_OK;
  }

private:
  int _fd;
};

class DownloaderTask : public Task {
public:
  DownloaderTask(int id);
  ~DownloaderTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  RES_CODE set_url_queue();
  RES_CODE init_dns_cache();
  RES_CODE init_event_loop();

  int _id;
  SharedPointer<DnsCache> _dns_cache;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
  SharedPointer<EventLoop> _event_loop;
};

_END_MYJFM_NAMESPACE_

#endif


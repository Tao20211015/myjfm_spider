/*******************************************************************************
 * downloader_task.h - the downloader module implementation
 * each downloader is a thread. It creates TCP connections to the sites and 
 * download the web pages, then send all the web pages to the extractor.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

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
  ReadCallback(int);
  ~ReadCallback() {}

  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _fd;
};

// when some socket can be written, then executes the write callback functor
class WriteCallback : public Callback {
public:
  WriteCallback(int fd);
  ~WriteCallback() {}

  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _fd;
};

// when some socket occurred an error, then executes the error callback functor
class ErrorCallback : public Callback {
public:
  ErrorCallback(int fd);
  ~ErrorCallback() {}

  virtual RES_CODE operator()(void* arg = NULL);

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
  RES_CODE create_connection(String&, short&, int&);

  int _id;
  SharedPointer<DnsCache> _dns_cache;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
  SharedPointer<EventLoop> _event_loop;
  Map<String, int> _sock_fd_map;
};

_END_MYJFM_NAMESPACE_

#endif


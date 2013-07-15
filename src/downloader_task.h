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
#include "page.h"

_START_MYJFM_NAMESPACE_

#define MAX_NUM_OF_RETRIES 3
#define MAX_PAGE_SIZE (1024 << 13) // (8M)
#define MAX_HEADER_SIZE (1024 << 2) // (4K)

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
  RES_CODE init();
  RES_CODE main_loop();

  RES_CODE init_url_queue();
  RES_CODE init_dns_cache();
  RES_CODE init_event_loop();

  RES_CODE create_connection(String&, short&, int&);
  RES_CODE generate_http_request(String&, String&, String&);
  RES_CODE set_timeout(int);
  RES_CODE send_http_request(int, String&);
  RES_CODE recv_http_response_header(int, String&, char*, int&);
  RES_CODE analysis_http_response_header(char*, HttpResponseHeader&);
  RES_CODE recv_http_response_body(int);

  int _id;
  SharedPointer<DnsCache> _dns_cache;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
  SharedPointer<EventLoop> _event_loop;
  Map<String, int> _sock_fd_map;
  int _create_connection_timeout;
  int _send_timeout;
  int _recv_timeout;
};

_END_MYJFM_NAMESPACE_

#endif


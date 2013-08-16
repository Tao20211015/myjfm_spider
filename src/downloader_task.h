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

#include <stdint.h>

#include "config.h"
#include "task.h"
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
  explicit ReadCallback(int fd);
  ~ReadCallback() {}

  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _fd;
};

// when some socket can be written, then executes the write callback functor
class WriteCallback : public Callback {
public:
  explicit WriteCallback(int fd);
  ~WriteCallback() {}

  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _fd;
};

// when some socket occurred an error, then executes the error callback functor
class ErrorCallback : public Callback {
public:
  explicit ErrorCallback(int fd);
  ~ErrorCallback() {}

  virtual RES_CODE operator()(void* arg = NULL);

private:
  int _fd;
};

class DownloaderTask : public Task {
public:
  explicit DownloaderTask(uint32_t id);
  ~DownloaderTask();
  virtual RES_CODE operator()(void* arg = NULL);

private:
  RES_CODE init();
  RES_CODE main_loop();

  RES_CODE init_url_queue();
  RES_CODE init_extractor_queue();
  RES_CODE init_event_loop();

  RES_CODE create_connection(Vector<uint32_t>&, uint16_t&, int&);
  RES_CODE generate_http_request(String&, String&, String&);
  RES_CODE set_timeout(int);
  RES_CODE send_http_request(int, String&);
  RES_CODE recv_http_response_header(int, char*, uint32_t&);
  RES_CODE analysis_http_response_header(char*, HttpResponseHeader&);
  RES_CODE recv_http_response_body(int, uint32_t, int32_t, SharedPointer<Page>&);
  RES_CODE recv_by_chunk(int, SharedPointer<Page>&);
  RES_CODE recv_by_content_length(int, uint32_t, SharedPointer<Page>&);
  RES_CODE recv_chunk_size(int, uint32_t&);
  RES_CODE recv_and_discard(int);
  RES_CODE recvn(int, uint32_t, char*);
  RES_CODE put_page_into_extractor(SharedPointer<Url>&, SharedPointer<Page>&);

  uint32_t _id;
  uint32_t _extractor_num;
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;
  Vector<SharedPointer<SQueue<SharedPointer<Page> > > > _extractor_queues;
  SharedPointer<EventLoop> _event_loop;
  Map<String, int> _sock_fd_map;
  uint32_t _create_connection_timeout;
  uint32_t _send_timeout;
  uint32_t _recv_timeout;
};

_END_MYJFM_NAMESPACE_

#endif


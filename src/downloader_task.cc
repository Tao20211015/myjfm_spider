/*******************************************************************************
 * downloader_task.cc - the downloader module implementation
 * each downloader is a thread. It creates TCP connections to the sites and 
 * download the web pages, then send all the web pages to the extractor.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "config.h"
#include "global.h"
#include "url.h"
#include "squeue.h"
#include "utility.h"
#include "event_loop.h"
#include "shared_pointer.h"
#include "downloader_task.h"
#include "memory_pool.h"
#include "page.h"

extern _MYJFM_NAMESPACE_::Global* glob;

_START_MYJFM_NAMESPACE_

ReadCallback::ReadCallback(int fd) : _fd(fd) {}

RES_CODE ReadCallback::operator()(void* arg) {
  return S_OK;
}

WriteCallback::WriteCallback(int fd) : _fd(fd) {}

RES_CODE WriteCallback::operator()(void* arg) {
  return S_OK;
}

ErrorCallback::ErrorCallback(int fd) : _fd(fd) {}

RES_CODE ErrorCallback::operator()(void* arg) {
  return S_OK;
}

DownloaderTask::DownloaderTask(int id) : 
  _id(id), 
  _dns_cache(NULL), 
  _url_queue(NULL), 
  _event_loop(NULL), 
  _create_connection_timeout(0), 
  _send_timeout(0), 
  _recv_timeout(0) {
  _sock_fd_map.clear();
}

DownloaderTask::~DownloaderTask() {
  //_event_loop->stop();
  _sock_fd_map.clear();
}

RES_CODE DownloaderTask::operator()(void* arg) {
  if (init() != S_OK) {
    return S_FAIL;
  }

  return main_loop();
  /*
  for (;;) {
    LOG(WARNING, "[%d]", _id);
    usleep(10000);
  }

  return S_OK;
  */
}

RES_CODE DownloaderTask::init() {
  if (init_url_queue() != S_OK) {
    return S_FAIL;
  }

  if (init_dns_cache() != S_OK) {
    return S_FAIL;
  }

  /*
  if (init_event_loop() != S_OK) {
    return S_FAIL;
  }
  */

  glob->get_create_connection_timeout(_create_connection_timeout);
  glob->get_send_timeout(_send_timeout);
  glob->get_recv_timeout(_recv_timeout);

  return S_OK;
}

RES_CODE DownloaderTask::init_url_queue() {
  return glob->get_downloader_queue(_id, _url_queue);
}

RES_CODE DownloaderTask::init_dns_cache() {
  glob->get_dns_cache(_dns_cache);
  if (_dns_cache.is_null()) {
    return S_FAIL;
  }
  /*
  SharedPointer<DnsCache> tmp_cache(new DnsCache());
  _dns_cache = tmp_cache;

  if (_dns_cache.is_null()) {
    return S_FAIL;
  }
  */

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

RES_CODE DownloaderTask::main_loop() {
  for (;;) {
    LOG(WARNING, "[%d] fetch one url", _id);

    // get the url from global synchronous url queue
    SharedPointer<Url> url_p;
    _url_queue->pop(url_p);

    Protocol proto;
    String site;
    String str_port;
    short port = 0;
    String file;
    int retries = 0;

    int valid = 0;
    int fd = -1;

    // if the url is not valid, then drop it
    url_p->is_valid(valid);
    if (!valid) {
      continue;
    }

    url_p->get_protocol(proto);
    if (proto != HTTP) {
      continue;
    }

    url_p->get_site(site);
    if (site.length() <= 0) {
      continue;
    }

    url_p->get_port(str_port);
    if (str_port.length() <= 0 || 
        Utility::str2integer(str_port, port) != S_OK || 
        port <= 0) {
      continue;
    }

    url_p->get_file(file);

    // if the retry times are more than MAX_NUM_OF_RETRIES, 
    // then drop it
    url_p->get_retries(retries);
    if (retries >= MAX_NUM_OF_RETRIES) {
      Map<String, int>::iterator iter = _sock_fd_map.find(site);
      if (iter != _sock_fd_map.end()) {
        close(iter->second);
        _sock_fd_map.erase(iter);
      }

      LOG(WARNING, 
          "[%d] %s:%d retry times are more than %d, drop it", 
          _id, site.c_str(), port, MAX_NUM_OF_RETRIES);
      continue;
    }

    // if the TCP connection with this site has been established, then reuse it
    Map<String, int>::iterator itr = _sock_fd_map.find(site);
    if (itr == _sock_fd_map.end()) { // not connected
      Vector<String> ips;
      if (_dns_cache->find(site, ips) == S_OK) { // dns cached
        if (ips.size() <= 0) { // can't find the site's ip
          //we should not retry
          continue;
        }

        if (create_connection(ips[0], port, fd) == S_OK) {
          _sock_fd_map[site] = fd;
        } else { // can not create the TCP connection
          // retry
          url_p->inc_retries();
          _url_queue->push(url_p);
          continue;
        }
      } else {
        LOG(WARNING, "[%d] query dns for %s", _id, site.c_str());
        IPTYPE iptype = IP_DUMMY;
        if (DnsCache::dns_query(site, ips, iptype) != S_OK) { //dns query failed
          LOG(WARNING, "[%d] query dns for %s failed", _id, site.c_str());
          // retry
          url_p->inc_retries();
          _url_queue->push(url_p);
          continue;
        }
        
        LOG(WARNING, "[%d] query dns for %s successfully", _id, site.c_str());

        if (iptype != IPv4) { // only support ipv4 for now
          // should not retry
          continue;
        }

        if (ips.size() <= 0) { // can't find the site's ip
          // should not retry
          continue;
        }

        _dns_cache->insert(site, ips);

        // pick up the first ip
        if (create_connection(ips[0], port, fd) == S_OK) {
          _sock_fd_map[site] = fd;
        } else { // cannot create the TCP connection
          // retry
          url_p->inc_retries();
          _url_queue->push(url_p);
          continue;
        }
      }
    } else { // connected
      fd = itr->second;
    }

    String request = "";
    if (generate_http_request(site, file, request) != S_OK) {
      continue;
    }

    LOG(WARNING, "[%d] %s", _id, request.c_str());

    if (set_timeout(fd) != S_OK) {
      continue;
    }

    if (send_http_request(fd, request) != S_OK) {
      LOG(WARNING, "[%d]: send_http_request() error", _id);
      // do not retry
      continue;
    }

    MemoryPool* memory_pool = MemoryPool::get_instance();
    char *header = (char*)(memory_pool->get_memory(MAX_HEADER_SIZE));

    int header_len = 0;
    if (recv_http_response_header(fd, site, header, header_len) != S_OK) {
      // do not retry
      memory_pool->put_memory(header);
      continue;
    }

    LOG(WARNING, "[%d] %s", _id, header);

    HttpResponseHeader http_response_header;
    if (analysis_http_response_header(header, http_response_header) != S_OK) {
      memory_pool->put_memory(header);
      continue;
    }

    LOG(WARNING, "[%d] Analysis http response header successfully!", _id);
    int http_version = 0;
    int status_code = 0;
    int content_length = -1;
    String content_type = "";
    http_response_header.get_http_version(http_version);
    http_response_header.get_status_code(status_code);
    http_response_header.get_content_length(content_length);
    http_response_header.get_content_type(content_type);
    LOG(WARNING, "[%d] http version %d", _id, http_version);
    LOG(WARNING, "[%d] status code %d", _id, status_code);
    LOG(WARNING, "[%d] content length %d", _id, content_length);
    LOG(WARNING, "[%d] content type %s", _id, content_type.c_str());
  }

  return S_OK;
}

RES_CODE DownloaderTask::create_connection(String& ip, short& port, int& fd) {
  fd_set fdset;
  int mask;
  struct sockaddr_in stat_addr;
  struct timeval tm = {(time_t)_create_connection_timeout, (suseconds_t)0};

  bzero((char*)&stat_addr, sizeof(stat_addr));
  stat_addr.sin_family = AF_INET;
  stat_addr.sin_port = htons(port);
  stat_addr.sin_addr.s_addr = inet_addr(ip.c_str());

  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fd = -1;
    return S_FAIL;
  }

  mask = fcntl(fd, F_GETFL, 0);
  if (mask < 0) {
    fd = -1;
    return S_FAIL;
  }

  if (fcntl(fd, F_SETFL, mask | O_NONBLOCK) < 0) {
    fd = -1;
    return S_FAIL;
  }

  if (connect(fd, (struct sockaddr*)&stat_addr, sizeof(struct sockaddr)) == 0) {
    if (fcntl(fd, F_SETFL, mask & ~O_NONBLOCK) < 0) {
      close(fd);
      fd = -1;
      return S_FAIL;
    }

    return S_OK;
  }

  FD_ZERO(&fdset);
  FD_SET(fd, &fdset);

  int ret = select(fd + 1, NULL, &fdset, NULL, &tm);
  if (ret <= 0) { // timeout(0) or select error(<0)
    fcntl(fd, F_SETFL, mask & ~O_NONBLOCK);
    close(fd);
    fd = -1;
    return S_FAIL;
  } else {
    if (fcntl(fd, F_SETFL, mask & ~O_NONBLOCK) < 0) {
      close(fd);
      fd = -1;
      return S_FAIL;
    }

    return S_OK;
  }
}

RES_CODE DownloaderTask::generate_http_request(String& site, 
    String& file, String& request) {
  if (site.length() <= 0) {
    return S_FAIL;
  }

  request = "GET /";
  request += file + " HTTP/1.1\r\nHost: " + site + "\r\n";

  return glob->get_request_header(request);
}

RES_CODE DownloaderTask::set_timeout(int fd) {
  int timeout = 0;
  struct timeval tm = {(time_t)0, (suseconds_t)0};

  if (glob->get_send_timeout(timeout) != S_OK) {
    return S_FAIL;
  }

  tm.tv_sec = (time_t)timeout;

  if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, &tm, sizeof(tm))) {
    return S_FAIL;
  }

  if (glob->get_recv_timeout(timeout) != S_OK) {
    return S_FAIL;
  }

  tm.tv_sec = (time_t)timeout;

  if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tm, sizeof(tm))) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE DownloaderTask::send_http_request(int fd, String& request) {
  if (write(fd, request.c_str(), request.length()) != request.length()) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE DownloaderTask::recv_http_response_header(int fd, 
    String&site, char* header, int& len) {
  if (fd < 0 || !header) {
    return S_FAIL;
  }

  len = 0;
  int ret = 0;
  int is_end = 0;

  while (is_end != 4 && len < MAX_HEADER_SIZE) {
    ret = read(fd, header, 1);
    if (ret <= 0) {
      close(fd);
      _sock_fd_map.erase(site);
      return S_FAIL;
    }

    if (*header == '\r') {
      ++is_end;
    } else if (*header == '\n') {
      ++is_end;
      ++header;
      ++len;
    } else {
      is_end = 0;
      ++header;
      ++len;
    }
  }

  if (is_end == 4 && len < MAX_HEADER_SIZE) {
    header -= 2;
    *header = '\0';
    len -= 2;
    return S_OK;
  } else {
    len = 0;
    return S_FAIL;
  }
}

RES_CODE DownloaderTask::analysis_http_response_header(
    char* header, HttpResponseHeader& http_response_header) {
  return http_response_header.analysis(header);
}

RES_CODE DownloaderTask::recv_http_response_body(int fd) {
  return S_OK;
}

_END_MYJFM_NAMESPACE_


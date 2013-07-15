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
  RES_CODE res_code = S_OK;

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
      res_code = _dns_cache->find(site, ips);
      if (res_code == S_OK) { // dns cached
        if (ips.size() <= 0) { // can't find the site's ip
          //we should not retry
          continue;
        }

        res_code = create_connection(ips, port, fd);
        if (res_code == S_OK) {
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
        res_code = DnsCache::dns_query(site, ips, iptype);
        if (res_code != S_OK) { //dns query failed
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
        res_code = create_connection(ips, port, fd);
        if (res_code == S_OK) {
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
    res_code = generate_http_request(site, file, request);
    if (res_code != S_OK) {
      continue;
    }

    LOG(WARNING, "[%d] %s", _id, request.c_str());

    res_code = set_timeout(fd);
    if (res_code != S_OK) {
      // retry
      url_p->inc_retries();
      _url_queue->push(url_p);
      continue;
    }

    res_code = send_http_request(fd, request);
    if (res_code == S_FAIL) {
      LOG(WARNING, "[%d]: send_http_request() error", _id);
      // do not retry
      continue;
    } else if (res_code == S_SHOULD_CLOSE_CONNECTION) { // the socket is broken
      // close the socket and retry
      LOG(WARNING, "[%d]: send_http_request() error close connection", _id);
      close(fd);
      _sock_fd_map.erase(site);
      url_p->inc_retries();
      _url_queue->push(url_p);
      continue;
    }

    MemoryPool* memory_pool = MemoryPool::get_instance();
    char *header = (char*)(memory_pool->get_memory(MAX_HEADER_SIZE));

    int header_len = 0;
    res_code = recv_http_response_header(fd, header, header_len);
    if (res_code == S_FAIL) {
      // do not retry
      memory_pool->put_memory(header);
      continue;
    } else if (res_code == S_SHOULD_CLOSE_CONNECTION) { // the socket is broken
      memory_pool->put_memory(header);
      // close the socket and retry
      close(fd);
      _sock_fd_map.erase(site);
      url_p->inc_retries();
      _url_queue->push(url_p);
      continue;
    }

    LOG(WARNING, "[%d] %s", _id, header);

    HttpResponseHeader http_response_header;
    res_code = analysis_http_response_header(header, http_response_header);
    if (res_code != S_OK) {
      // do not retry
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
    LOG(WARNING, "[%d] status code %d", _id, status_code);
    LOG(WARNING, "[%d] content length %d", _id, content_length);
    memory_pool->put_memory(header);
    
    SharedPointer<Page> page(NULL);
    res_code = recv_http_response_body(fd, status_code, content_length, page);
    if (res_code == S_SHOULD_CLOSE_CONNECTION) {
      LOG(WARNING, "[%d] ******read() error******", _id);
      close(fd);
      _sock_fd_map.erase(site);
      url_p->inc_retries();
      _url_queue->push(url_p);
      continue;
    } else if (res_code == S_FAIL) {
      LOG(WARNING, "[%d] ******recv body error******", _id);
      continue;
    } else if (res_code == S_OK) {
      char* content = NULL;
      page->get_page_content(content);
      if (content) {
        LOG(WARNING, "[%d] !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!", _id);
      }
    }
  }

  return S_OK;
}

RES_CODE DownloaderTask::create_connection(Vector<String>& ips, 
    short& port, int& fd) {
  String ip = "";
  int num_of_ips = ips.size();

  if (num_of_ips <= 0) {
    fd = -1;
    return S_FAIL;
  } else if (num_of_ips == 1) {
    ip = ips[0];
  } else {
    srand((unsigned)time(NULL));
    ip = ips[rand() % num_of_ips];
  }

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
  if (fd < 0 || request.length() <= 0) {
    LOG(WARNING, "[%d] fd < 0 or request.length() <= 0", _id);
    return S_FAIL;
  }

  int length = request.length();
  const char* buffer = request.c_str();

  while (1) {
    int write_len = write(fd, buffer, length);
    if (write_len == -1) {
      return S_SHOULD_CLOSE_CONNECTION;
    } else if (write_len == length) {
      return S_OK;
    } else {
      length -= write_len;
      buffer += write_len;
    }
  }

  return S_OK;
}

RES_CODE DownloaderTask::recv_http_response_header(int fd, 
    char* header, int& len) {
  if (fd < 0 || !header) {
    return S_FAIL;
  }

  len = 0;
  int ret = 0;
  int is_end = 0;

  while (is_end != 4 && len < MAX_HEADER_SIZE) {
    ret = read(fd, header, 1);
    if (ret <= 0) {
      return S_SHOULD_CLOSE_CONNECTION;
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

RES_CODE DownloaderTask::analysis_http_response_header(char* header, 
    HttpResponseHeader& http_response_header) {
  return http_response_header.analysis(header);
}

RES_CODE DownloaderTask::recv_http_response_body(int fd, 
    int status_code, int content_length, SharedPointer<Page>& page) {
  if (fd < 0) {
    return S_FAIL;
  }

  // do not process other status temporarily
  if (status_code != 200) {
    return S_FAIL;
  }

  if (content_length == -1) {
    return recv_by_chunk(fd, page);
  } else if (content_length > 0) {
    page = SharedPointer<Page>(new Page(content_length));
    return recv_by_content_length(fd, content_length, page);
  } else {
    return S_FAIL;
  }
}

RES_CODE DownloaderTask::recv_by_chunk(int fd, 
    SharedPointer<Page>& page) {
  SharedPointer<Page> buffer(new Page(0));
  if (buffer.is_null()) {
    return S_SHOULD_CLOSE_CONNECTION;
  }

  while (1) {
    int chunk_size = 0;
    if (recv_chunk_size(fd, chunk_size) != S_OK) {
      return S_SHOULD_CLOSE_CONNECTION;
    }

    if (chunk_size == 0) {
      if (recv_and_discard(fd) != S_OK) {
        return S_SHOULD_CLOSE_CONNECTION;
      }

      break;
    }

    SharedPointer<Page> chunk(new Page(chunk_size + 2));
    char* chunk_content = NULL;
    chunk->get_page_content(chunk_content);
    if (recvn(fd, chunk_size + 2, chunk_content) != S_OK) {
      return S_SHOULD_CLOSE_CONNECTION;
    }

    int buffer_size = 0;
    buffer->get_page_size(buffer_size);

    char* buffer_content = NULL;
    buffer->get_page_content(buffer_content);

    SharedPointer<Page> tmp_buffer(new Page(buffer_size + chunk_size));
    if (tmp_buffer.is_null()) {
      return S_SHOULD_CLOSE_CONNECTION;
    }

    char* tmp_buffer_content = NULL;
    tmp_buffer->get_page_content(tmp_buffer_content);

    if (buffer_content) {
      memcpy(tmp_buffer_content, buffer_content, buffer_size);
    }

    memcpy(tmp_buffer_content + buffer_size, chunk_content, chunk_size);
    buffer = tmp_buffer;
  }

  page = buffer;

  return S_OK;
}

RES_CODE DownloaderTask::recv_by_content_length(int fd, 
    int content_length, SharedPointer<Page>& page) {
  if (fd <= 0 || content_length < 0 || page.is_null()) {
    return S_SHOULD_CLOSE_CONNECTION;
  }

  char* buffer = NULL;
  page->get_page_content(buffer);
  if (!buffer) {
    return S_SHOULD_CLOSE_CONNECTION;
  }

  if (recvn(fd, content_length, buffer) != S_OK) {
    return S_SHOULD_CLOSE_CONNECTION;
  }

  return S_OK;
}

RES_CODE DownloaderTask::recv_chunk_size(int fd, int& size) {
  if (fd <= 0) {
    return S_FAIL;
  }

  int ret = 0;
  char buffer;
  size = 0;
  int flag = 0;

  while (1) {
    ret = read(fd, &buffer, 1);
    if (ret <= 0) {
      return S_FAIL;
    }

    if (Utility::is_hex_digit(buffer)) {
      if (buffer >= '0' && buffer <= '9') {
        size = size * 16 + buffer - '0';
      } else if (buffer >= 'a' && buffer <= 'z') {
        size = size * 16 + buffer - 'a';
      } else {
        size = size * 16 + buffer - 'A';
      }
    } else {
      break;
    }
  }

  if (buffer == '\r') {
    flag = 1;
  }

  while (1) {
    ret = read(fd, &buffer, 1);
    if (ret <= 0) {
      return S_FAIL;
    }

    if (buffer == '\r') {
      flag = 1;
    } else if (buffer == '\n' && flag == 1) {
      return S_OK;
    } else {
      flag = 0;
    }
  }
}

RES_CODE DownloaderTask::recv_and_discard(int fd) {
  if (fd <= 0) {
    return S_FAIL;
  }

  char buffer;
  int flag = 0;

  while (1) {
    int ret = read(fd, &buffer, 1);
    if (ret <= 0) {
      return S_FAIL;
    }

    if (buffer == '\r') {
      flag = 1;
    } else if (buffer == '\n' && flag == 1) {
      return S_OK;
    } else {
      flag = 0;
    }
  }
}

RES_CODE DownloaderTask::recvn(int fd, int n, char* buffer) {
  if (fd < 0 || n <= 0 || !buffer) {
    return S_FAIL;
  }

  int remain_n = n;

  while (1) {
    int read_len = read(fd, buffer, remain_n);
    if (read_len <= 0) { // some error occured
      return S_FAIL;
    } else if (read_len == remain_n) {
      return S_OK;
    } else {
      remain_n -= read_len;
      buffer += read_len;
    }
  }
}

_END_MYJFM_NAMESPACE_


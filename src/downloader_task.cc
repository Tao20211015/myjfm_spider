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
  _event_loop(NULL) {
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

  return S_OK;
}

RES_CODE DownloaderTask::init_url_queue() {
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
    String args;
    int retries = 0;

    int fd = -1;

    // if the url is not valid, then drop it
    int valid = 0;
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
    url_p->get_args(args);

    // if the retry times are more than MAX_NUM_OF_RETRIES, 
    // then drop it
    url_p->get_retries(retries);
    if (retries >= MAX_NUM_OF_RETRIES) {
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
          continue;
        }

        if (ips.size() <= 0) { // can't find the site's ip
          continue;
        }

        _dns_cache->insert(site, ips);

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

    String request = "GET ";
    request += "/" + file + "?" + args + " HTTP/1.1\r\nHost: ";
    request += "http://" + site;
    if (port != 80) {
      request += str_port;
    }

    String request_header = "";
    glob->get_request_header(request_header);
    request += request_header;
    LOG(WARNING, "[%d] %s", _id, request.c_str());
  }

  return S_OK;
}

RES_CODE DownloaderTask::create_connection(String& ip, short& port, int& fd) {
  struct sockaddr_in stat_addr;
  bzero((char*)&stat_addr, sizeof(stat_addr));

  stat_addr.sin_family = AF_INET;
  stat_addr.sin_port = htons(port);
  stat_addr.sin_addr.s_addr = inet_addr(ip.c_str());
  
  if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    fd = -1;
    return S_FAIL;
  }
  
  fcntl(fd, F_SETFL, O_NONBLOCK);
  if (connect(fd, (struct sockaddr*)&stat_addr, 
        sizeof(struct sockaddr_in)) == 0) {
    LOG(WARNING, "[%d] %s:%d connection established immediately", 
        _id, ip.c_str(), port);
    return S_OK;
  } else {
    fd_set fdset;
    FD_ZERO(&fdset);
    FD_SET(fd, &fdset);

    int timeout = 0;
    glob->get_create_connection_timeout(timeout);

    struct timeval tm = {(time_t)timeout, (suseconds_t)0};

    int ret = select(fd + 1, NULL, &fdset, NULL, &tm);
    if (ret == 0) { // timeout
      LOG(WARNING, 
          "[%d] %s:%d connection timeout1", _id, ip.c_str(), port);
      close(fd);
      fd = -1;
      return S_FAIL;
    } else if (ret > 0) {
      int error = -1;
      int len = sizeof(int);
      getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, (socklen_t*)&len);
      if (error == 0) {
        LOG(WARNING, 
            "[%d] %s:%d connection established within timeout", 
            _id, ip.c_str(), port);
        return S_OK;
      } else {
        LOG(WARNING, 
            "[%d] %s:%d connection timeout2", _id, ip.c_str(), port);
        close(fd);
        fd = -1;
        return S_FAIL;
      }
    } else {
      LOG(WARNING, 
          "[%d] %s:%d connection timeout2", _id, ip.c_str(), port);
      close(fd);
      fd = -1;
      return S_FAIL;
    }
  }
}

_END_MYJFM_NAMESPACE_


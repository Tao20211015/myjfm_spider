/*******************************************************************************
 * downloader_task.cc - the downloader module implementation
 * each downloader is a thread. It creates TCP connections to the sites and 
 * download the web pages, then send all the web pages to the extractor.
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdlib.h>

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
  _dns_cache(SharedPointer<DnsCache>(NULL)), 
  _url_queue(SharedPointer<SQueue<SharedPointer<Url> > >(NULL)), 
  _event_loop(SharedPointer<EventLoop>(NULL)) {
  _sock_fd_map.clear();
}

DownloaderTask::~DownloaderTask() {
  //_event_loop->stop();
  _sock_fd_map.clear();
}

RES_CODE DownloaderTask::operator()(void* arg) {
  set_url_queue();
  init_dns_cache();
  //init_event_loop();

  for (;;) {
    // get the url from global synchronous url queue
    SharedPointer<Url> url_p;
    _url_queue->pop(url_p);

    Protocol proto;
    String site;
    String str_port;
    short port = 0;
    String file;
    String args;

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

    // if the TCP connection with this site has been established, then reuse it
    Map<String, int>::iterator itr = _sock_fd_map.find(site);
    if (itr == _sock_fd_map.end()) { // not connected
      Vector<String> ips;
      if (_dns_cache->find(site, ips) == S_OK) { // dns cached
        if (ips.size() <= 0) {
          continue;
        }

        if (create_connection(ips[0], port, fd) == S_OK) {
          _sock_fd_map[site] = fd;
        } else {
          continue;
        }
      } else {
        IPTYPE iptype = IP_DUMMY;
        if (DnsCache::dns_query(site, ips, iptype) != S_OK) {
          continue;
        }

        if (iptype != IPv4) { // only support ipv4 for now
          continue;
        }

        if (ips.size() <= 0) {
          continue;
        }

        _dns_cache->insert(site, ips);

        if (create_connection(ips[0], port, fd) == S_OK) {
          _sock_fd_map[site] = fd;
        } else {
          continue;
        }
      }
    } else { // connected
      fd = itr->second;
    }

    LOG(WARNING, 
        "http://%s:%d/%s?%s", site.c_str(), port, file.c_str(), args.c_str());
  }

  return S_OK;
}

RES_CODE DownloaderTask::create_connection(String& ip, short& port, int& fd) {
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


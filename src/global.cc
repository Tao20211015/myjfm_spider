/*******************************************************************************
 * global.cc - all the global variables are packeged into one class
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "config.h"
#include "global.h"
#include "utility.h"
#include "logger.h"

_START_MYJFM_NAMESPACE_

#define CHECK_HAS_INIT() \
  do { \
    if (!_has_init) { \
      Cerr << "[FATAL] glob has not been initialized" << Endl; \
      abort(); \
    } \
  } while (0)

#if 0
Map<String, String> Global::_MIME;
#endif

Global::Global() : 
  _has_init(false), 
  _config_file(""), 
  _cur_path(""), 
  _save_path(""), 
  _log_path(""), 
  _err_path(""), 
  _depth(5), 
  _logger(NULL), 
  _downloader_num(5), 
  _extractor_num(5), 
  _scheduler_num(1), 
  _to_be_shutdown(0), 
  _url_queue(NULL), 
  _downloader_threadpool(NULL), 
  _extractor_threadpool(NULL), 
  _scheduler_threadpool(NULL), 
  _dns_timeout(10), 
  _create_connection_timeout(2), 
  _send_timeout(5), 
  _recv_timeout(30), 
  _request_header(""), 
  _user_agent("myjfm_spider"), 
  _sender("myjfm_spider@xxx.com"), 
  _dns_cache(NULL), 
  _name_server("") {
  _file_types.clear();
  _seed_urls.clear();
  _downloader_queues.clear();
}

Global::~Global() {
  // write all the logs that are still in the buffer onto the disk
  // and close the log file
  delete _logger;
}

RES_CODE Global::init(String& v_cur_path, String& config_file_name) {
  if (_has_init) {
    Cerr << "[ERROR] init() failed. the glob has been initialized" << Endl;
    return S_HAS_INIT;
  }

  if (config_file_name == "") {
    Cerr << "[FATAL] init() failed. config_file_name is empty" << Endl;
    abort();
  }

  // It should be initialized here and JUST ONCE!!!
  _has_init = true;

  _cur_path = v_cur_path;
  _config_file = config_file_name;
  _depth = 5;
  _downloader_num = 5;
  _extractor_num = 5;
  _scheduler_num = 1;

  _seed_urls.clear();

  load_default_file_types();

  parse_config();

  if (check_name_server() != S_OK) {
    Cerr << "[FATAL] init() failed. name server is empty" << Endl;
    abort();
  }

  assemble_request_header();

  // initialize the url queue
  _url_queue = 
    SharedPointer<SQueue<SharedPointer<Url> > >
    (new SQueue<SharedPointer<Url> >());

  int i = 0;
  for (i = 0; i < _seed_urls.size(); ++i) {
    SharedPointer<Url> url_p(new Url(_seed_urls[i]));
    _url_queue->push(url_p);
  }

  // initialize all the downloader queues
  for (i = 0; i < _downloader_num; ++i) {
    _downloader_queues.push_back(
        SharedPointer<SQueue<SharedPointer<Url> > >
        (new SQueue<SharedPointer<Url> >()));
  }

  // initialize the dns cache
  SharedPointer<DnsCache> tmp_dns_cache(new DnsCache());
  _dns_cache = tmp_dns_cache;

  _logger = new Logger(10000);
  _logger->init();

  return S_OK;
}

RES_CODE Global::load_default_file_types() {
  CHECK_HAS_INIT();

  _file_types.clear();
  _file_types.push_back(".htm");
  _file_types.push_back(".html");

  return S_OK;
}

RES_CODE Global::assemble_request_header() {
  CHECK_HAS_INIT();

  _request_header = "User-Agent: ";
  _request_header += _user_agent + " " + _sender;
  _request_header += "\r\nAccept: */*\r\n";
  _request_header += "Accept-Encoding: gzip,deflate,sdch\r\n";
  _request_header += "Accept-Language: en-US,en;q=0.8\r\n";
  _request_header += "Connection: keep-alive\r\n\r\n";

  return S_OK;
}

RES_CODE Global::parse_config() {
  CHECK_HAS_INIT();

  if (_config_file == "") {
    Cerr << "[FATAL] parse_config() failed. _config_file is empty" << Endl;
    abort();
  } else {
    FILE* config_file_p = fopen(_config_file.c_str(), "r");

    if (config_file_p == NULL) {
      Cerr << "[FATAL] fopen() failed." << Endl;
      Cerr << "[FATAL] can not open the file " << _config_file << Endl;
      abort();
    }

#define MAX_BUF_LEN 1024
    char buffer[MAX_BUF_LEN];
    while (fgets(buffer, MAX_BUF_LEN - 1, config_file_p)) { 
#undef MAX_BUF_LEN
      Utility::trim(buffer);
      if (buffer[0] == '\0' || buffer[0] == '#') {
        continue;
      }

      String line = buffer;
      String separator = " \t";
      Vector<String> key_and_value;
      Utility::split(line, separator, key_and_value);

      if (key_and_value.size() < 2) {
        continue;
      }

      if (key_and_value[0] == "SAVEPATH") {
        set_save_path(key_and_value[1]);
      } else if (key_and_value[0] == "LOGFILEPATH") {
        set_log_path(key_and_value[1]);
      } else if (key_and_value[0] == "ERRFILEPATH") {
        set_err_path(key_and_value[1]);
      } else if (key_and_value[0] == "DEPTH") {
        set_depth(key_and_value[1]);
      } else if (key_and_value[0] == "DOWNLOADERS") {
        set_downloader_num(key_and_value[1]);
      } else if (key_and_value[0] == "EXTRACTORS") {
        set_extractor_num(key_and_value[1]);
      } else if (key_and_value[0] == "SCHEDULERS") {
        set_scheduler_num(key_and_value[1]);
      } else if (key_and_value[0] == "FILETYPES") {
        set_file_types(key_and_value);
      } else if (key_and_value[0] == "SEEDURLS") {
        set_seed_urls(key_and_value);
      } else if (key_and_value[0] == "DNS_TIMEOUT") {
        set_dns_timeout(key_and_value[1]);
      } else if (key_and_value[0] == "CREATE_CONNECTION_TIMEOUT") {
        set_create_connection_timeout(key_and_value[1]);
      } else if (key_and_value[0] == "SEND_TIMEOUT") {
        set_send_timeout(key_and_value[1]);
      } else if (key_and_value[0] == "RECV_TIMEOUT") {
        set_recv_timeout(key_and_value[1]);
      } else if (key_and_value[0] == "NAME_SERVER") {
        set_name_server(key_and_value[1]);
      } else {
        continue;
      }
    }

    if (_seed_urls.empty()) {
      fclose(config_file_p);
      Cerr << "[FATAL] The _seed_urls is empty. Stop crawling" << Endl;
      exit(1);
    }

    fclose(config_file_p);
  }

  return S_OK;
}

RES_CODE Global::check_name_server() {
  if (_name_server.length() <= 0) {
    return S_FAIL;
  }

  return S_OK;
}

RES_CODE Global::set_seed_urls(Vector<String>& seed_urls) {
  _seed_urls.clear();

  int i;
  for (i = 1; i < seed_urls.size(); ++i) {
    _seed_urls.push_back(seed_urls[i]);
  }

  return S_OK;
}

RES_CODE Global::set_file_types(Vector<String>& file_types) {
  _file_types.clear();

  int i;
  for (i = 1; i < file_types.size(); ++i) {
    _file_types.push_back(file_types[i]);
  }

  return S_OK;
}

RES_CODE Global::set_downloader_num(String& downloader_num) {
  CHECK_HAS_INIT();
  _downloader_num = atoi(downloader_num.c_str());

  return S_OK;
}

RES_CODE Global::get_downloader_num(int& num) {
  CHECK_HAS_INIT();
  num = _downloader_num;

  return S_OK;
}

RES_CODE Global::set_extractor_num(String& extractor_num) {
  CHECK_HAS_INIT();
  _extractor_num = atoi(extractor_num.c_str());

  return S_OK;
}

RES_CODE Global::get_extractor_num(int& num) {
  CHECK_HAS_INIT();
  num = _extractor_num;

  return S_OK;
}

RES_CODE Global::set_scheduler_num(String& scheduler_num) {
  CHECK_HAS_INIT();
  _scheduler_num = atoi(scheduler_num.c_str());

  return S_OK;
}

RES_CODE Global::get_scheduler_num(int& num) {
  CHECK_HAS_INIT();
  num = _scheduler_num;

  return S_OK;
}

RES_CODE Global::set_downloader_threadpool(SharedPointer<ThreadPool>& ptr) {
  if (ptr.is_null()) {
    return S_BAD_ARG;
  }

  _downloader_threadpool = ptr;

  return S_OK;
}

RES_CODE Global::get_downloader_threadpool(SharedPointer<ThreadPool>& ptr) {
  ptr = _downloader_threadpool;
  return S_OK;
}

RES_CODE Global::set_extractor_threadpool(SharedPointer<ThreadPool>& ptr) {
  if (ptr.is_null()) {
    return S_BAD_ARG;
  }

  _extractor_threadpool = ptr;

  return S_OK;
}

RES_CODE Global::get_extractor_threadpool(SharedPointer<ThreadPool>& ptr) {
  ptr = _extractor_threadpool;
  return S_OK;
}

RES_CODE Global::set_scheduler_threadpool(SharedPointer<ThreadPool>& ptr) {
  if (ptr.is_null()) {
    return S_BAD_ARG;
  }

  _scheduler_threadpool = ptr;

  return S_OK;
}

RES_CODE Global::get_scheduler_threadpool(SharedPointer<ThreadPool>& ptr) {
  ptr = _scheduler_threadpool;
  return S_OK;
}

RES_CODE Global::get_cur_path(String& cur_path) {
  CHECK_HAS_INIT();
  cur_path = _cur_path;

  return S_OK;
}

RES_CODE Global::set_cur_path(String& cur_path) {
  CHECK_HAS_INIT();
  _cur_path = cur_path;

  return S_OK;
}

RES_CODE Global::get_save_path(String& save_path) {
  CHECK_HAS_INIT();
  save_path = _save_path;

  return S_OK;
}

RES_CODE Global::set_save_path(String& path) {
  CHECK_HAS_INIT();
  _save_path = path;

  return S_OK;
}

RES_CODE Global::get_log_path(String& path) {
  CHECK_HAS_INIT();
  path = _log_path;

  return S_OK;
}

RES_CODE Global::set_log_path(String& path) {
  CHECK_HAS_INIT();
  _log_path = path;

  return S_OK;
}

RES_CODE Global::get_err_path(String& path) {
  CHECK_HAS_INIT();
  path = _err_path;

  return S_OK;
}

RES_CODE Global::set_err_path(String& path) {
  CHECK_HAS_INIT();
  _err_path = path;

  return S_OK;
}

RES_CODE Global::get_depth(int& dep) {
  CHECK_HAS_INIT();
  dep = _depth;

  return S_OK;
}

RES_CODE Global::set_depth(String& dep) {
  CHECK_HAS_INIT();
  _depth = atoi(dep.c_str());

  return S_OK;
}

RES_CODE Global::get_downloader_queue(int id, 
    SharedPointer<SQueue<SharedPointer<Url> > >& queue) {
  CHECK_HAS_INIT();

  int downloader_num = 0;
  get_downloader_num(downloader_num);

  if (id >= downloader_num || id < 0) {
    queue = SharedPointer<SQueue<SharedPointer<Url> > >(NULL);
    return S_OUT_RANGE;
  }

  queue = _downloader_queues[id];

  return S_OK;
}

RES_CODE Global::get_url_queue(
    SharedPointer<SQueue<SharedPointer<Url> > >& queue) {
  queue = _url_queue;

  return S_OK;
}

RES_CODE Global::get_logger(Logger*& logger) {
  CHECK_HAS_INIT();
  logger = _logger;

  return S_OK;
}

int Global::get_to_be_shutdown() {
  CHECK_HAS_INIT();
  return _to_be_shutdown;
}

RES_CODE Global::set_to_be_shutdown(int v) {
  CHECK_HAS_INIT();
  _to_be_shutdown = v;
  return S_OK;
}

RES_CODE Global::get_request_header(String& request_header) {
  CHECK_HAS_INIT();
  request_header += _request_header;

  return S_OK;
}

RES_CODE Global::set_dns_timeout(String& timeout) {
  CHECK_HAS_INIT();
  _dns_timeout = atoi(timeout.c_str());

  return S_OK;
}

RES_CODE Global::set_create_connection_timeout(String& timeout) {
  CHECK_HAS_INIT();
  _create_connection_timeout = atoi(timeout.c_str());

  return S_OK;
}

RES_CODE Global::set_send_timeout(String& timeout) {
  CHECK_HAS_INIT();
  _send_timeout = atoi(timeout.c_str());

  return S_OK;
}

RES_CODE Global::set_recv_timeout(String& timeout) {
  CHECK_HAS_INIT();
  _recv_timeout = atoi(timeout.c_str());

  return S_OK;
}

RES_CODE Global::set_name_server(String& name_server) {
  CHECK_HAS_INIT();
  _name_server = name_server;

  return S_OK;
}

RES_CODE Global::get_dns_timeout(int& timeout) {
  CHECK_HAS_INIT();
  timeout = _dns_timeout;

  return S_OK;
}

RES_CODE Global::get_create_connection_timeout(int& timeout) {
  CHECK_HAS_INIT();
  timeout = _create_connection_timeout;

  return S_OK;
}

RES_CODE Global::get_send_timeout(int& timeout) {
  CHECK_HAS_INIT();
  timeout = _send_timeout;

  return S_OK;
}

RES_CODE Global::get_recv_timeout(int& timeout) {
  CHECK_HAS_INIT();
  timeout = _recv_timeout;

  return S_OK;
}

RES_CODE Global::get_dns_cache(SharedPointer<DnsCache>& dns_cache) {
  CHECK_HAS_INIT();
  dns_cache = _dns_cache;

  return S_OK;
}

RES_CODE Global::get_name_server(String& name_server) {
  CHECK_HAS_INIT();
  name_server = _name_server;

  return S_OK;
}

#undef CHECK_HAS_INIT

_END_MYJFM_NAMESPACE_


/*******************************************************************************
 * global.h - all the global variables are packeged into one class
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"
#include "url.h"
#include "dns_cache.h"
#include "squeue.h"
#include "shared_pointer.h"
#include "thread_pool.h"

_START_MYJFM_NAMESPACE_

class Logger;

class Global {
public:
  Global();
  ~Global();

  RES_CODE init(String&, String&);
  RES_CODE parse_config();

  // for consistency, we use reference to bring back what we want
  RES_CODE get_cur_path(String&);
  RES_CODE get_log_path(String&);
  RES_CODE get_err_path(String&);
  RES_CODE set_log_path(String& path);
  RES_CODE set_err_path(String& path);
  RES_CODE get_save_path(String&);
  RES_CODE get_depth(int&);

  RES_CODE get_downloader_num(int&);
  RES_CODE get_extractor_num(int&);
  RES_CODE get_scheduler_num(int&);

  RES_CODE get_downloader_queue(int, 
      SharedPointer<SQueue<SharedPointer<Url> > >&);

  RES_CODE get_dns_cache(SharedPointer<DnsCache>&);

  RES_CODE set_downloader_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE set_extractor_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE set_scheduler_threadpool(SharedPointer<ThreadPool>&);

  RES_CODE get_downloader_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_extractor_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_scheduler_threadpool(SharedPointer<ThreadPool>&);

  RES_CODE get_url_queue(SharedPointer<SQueue<SharedPointer<Url> > >&);

  RES_CODE get_logger(Logger*&);

  int get_to_be_shutdown();
  RES_CODE set_to_be_shutdown(int);

  RES_CODE get_request_header(String&);

  RES_CODE get_dns_timeout(int&);
  RES_CODE get_create_connection_timeout(int&);
  RES_CODE get_send_timeout(int&);
  RES_CODE get_recv_timeout(int&);

  RES_CODE get_name_server(String&);

private:
  RES_CODE load_default_file_types();

  RES_CODE assemble_request_header();

  RES_CODE check_name_server();

  RES_CODE set_cur_path(String&);
  RES_CODE set_seed_urls(Vector<String>&);
  RES_CODE set_file_types(Vector<String>&);
  RES_CODE set_save_path(String&);
  RES_CODE set_depth(String&);

  RES_CODE set_downloader_num(String&);
  RES_CODE set_extractor_num(String&);
  RES_CODE set_scheduler_num(String&);

  RES_CODE set_dns_timeout(String&);
  RES_CODE set_create_connection_timeout(String&);
  RES_CODE set_send_timeout(String&);
  RES_CODE set_recv_timeout(String&);

  RES_CODE set_name_server(String&);

  //Mutex _mutex;
  // if there exists multi-threads, should guarantee consistency by mutex
  volatile bool _has_init;

  String _config_file;

  // current work path
  String _cur_path;

  // the path saved all the web pages and all the indexes
  String _save_path;

  String _log_path;

  String _err_path;

  // the depth of downloading recursively
  int _depth;

  Logger *_logger;

  int _downloader_num;
  int _extractor_num;
  int _scheduler_num;

  int _to_be_shutdown;

  // the types of file which downloader can download
  Vector<String> _file_types;

  // the seed urls read from configure file
  Vector<String> _seed_urls;

  // url queue. This queue is used by all threads.
  // All download threads will produce urls, and, 
  // the scheduler threads consume the urls.
  SharedPointer<SQueue<SharedPointer<Url> > > _url_queue;

  // downloader queue. Each download thread has one queue.
  // The queue is shared between this downloader and all scheduler threads.
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _downloader_queues;

  SharedPointer<ThreadPool> _downloader_threadpool;
  SharedPointer<ThreadPool> _extractor_threadpool;
  SharedPointer<ThreadPool> _scheduler_threadpool;

  int _dns_timeout;
  int _create_connection_timeout;
  int _send_timeout;
  int _recv_timeout;

  String _request_header;
  String _user_agent;
  String _sender;

#if 0
  static Map<String, String> _MIME;
#endif
  SharedPointer<DnsCache> _dns_cache;

  String _name_server;
};

_END_MYJFM_NAMESPACE_

#endif


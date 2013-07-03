#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"
#include "url.h"
#include "squeue.h"
#include "shared_pointer.h"
#include "thread_pool.h"

_START_MYJFM_NAMESPACE_

class Logger;

class Global {
public:
  Global();
  ~Global();

  RES_CODE init(String& v_cur_path, String& config_file_name);
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
  RES_CODE set_downloader_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE set_extractor_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE set_scheduler_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_downloader_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_extractor_threadpool(SharedPointer<ThreadPool>&);
  RES_CODE get_scheduler_threadpool(SharedPointer<ThreadPool>&);

  RES_CODE get_logger(Logger*&);
  int get_to_be_shutdown();
  RES_CODE set_to_be_shutdown(int);

private:
  RES_CODE load_default_file_types();
  RES_CODE set_cur_path(String&);
  RES_CODE set_seed_urls(Vector<String>& seed_urls);
  RES_CODE set_file_types(Vector<String>& file_types);
  RES_CODE set_save_path(String& path);
  RES_CODE set_depth(String& dep);
  RES_CODE set_downloader_num(String& downloader_num);
  RES_CODE set_extractor_num(String& extractor_num);
  RES_CODE set_scheduler_num(String& scheduler_num);

  // current work path
  String _cur_path;

  //Mutex _mutex;
  // if there exists multi-threads, should guarantee consistency by mutex
  volatile bool _has_init;

  String _config_file;

  // the path saved all the web pages and all the indexes
  String _save_path;

  String _log_path;

  String _err_path;

  Logger *_logger;

  // the depth of downloading recursively
  int _depth;

  int _downloader_num;
  int _extractor_num;
  int _scheduler_num;

  // the types of file which downloader can download
  Vector<String> _file_types;

  // the seed urls read from configure file
  Vector<String> _seed_urls;

  // url queue. This queue is used by all threads.
  // All download threads will produce urls, and, 
  // the scheduler threads consume the urls.
  SharedPointer<SQueue<SharedPointer<Url> > > _urls_queue;

  // downloader queue. Each download thread has one queue.
  // The queue is shared between this downloader and all scheduler threads.
  Vector<SharedPointer<SQueue<SharedPointer<Url> > > > _downloader_queues;

  int _to_be_shutdown;

  SharedPointer<ThreadPool> _downloader_threadpool;
  SharedPointer<ThreadPool> _extractor_threadpool;
  SharedPointer<ThreadPool> _scheduler_threadpool;
};

_END_MYJFM_NAMESPACE_

#endif


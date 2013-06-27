#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"
#include "url.h"
#include "squeue.h"
#include "sharedpointer.h"


_START_MYJFM_NAMESPACE_

class Global {
public:
  Global();
  ~Global();
  void init(String& v_cur_path, String& config_file_name);
  void parse_config();
  void get_save_path(String&);
  int get_downloader_num();
  int get_extractor_num();
  int get_scheduler_num();
  Sharedpointer<Squeue<Url> > get_downloader_queue(int);

private:
  void load_default_file_types();
  void set_seed_urls(Vector<String>& seed_urls);
  void set_file_types(Vector<String>& file_types);
  void set_save_path(String& path);
  void set_depth(String& dep);
  void set_downloader_num(String& downloader_num);
  void set_extractor_num(String& extractor_num);
  void set_scheduler_num(String& scheduler_num);

  // current work path
  String _cur_path;

  //Mutex _mutex;
  // if there exists multi-threads, should guarantee consistency by mutex
  volatile bool _has_init;
  String _config_file;

  // the path saved all the web pages and all the indexes
  String _save_path;

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
  Sharedpointer<Squeue<Url> > _urls_queue;

  // downloader queue. Each download thread has one queue.
  // The queue is shared between this downloader and all scheduler threads.
  Vector<Sharedpointer<Squeue<Url> > > _downloader_queues;
};

_END_MYJFM_NAMESPACE_

#endif


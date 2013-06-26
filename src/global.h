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

private:
  void load_default_file_types();
  void set_seed_urls(Vector<String>& seed_urls);
  void set_file_types(Vector<String>& file_types);
  void set_save_path(String& path);
  String& get_save_path();
  void set_depth(String& dep);
  void set_thread_num(String& thread_num);

  // current work path
  String _cur_path;

  //Mutex _mutex;
  // if there exists multi-threads, should guarantee consistency by mutex
  volatile bool _has_init;
  String _config_file;

  // the path saved all the web pages and all the indexes
  String _save_path;
  int _depth;
  int _thread_num;
  Vector<String> _file_types;
  Vector<String> _seed_urls;

  // thread safe queue. This queue is used by all threads.
  // All child threads will produce urls, and, 
  // only the master thread consume the urls.
  Sharedpointer<Squeue<Url> > _urls_queue;
};

_END_MYJFM_NAMESPACE_

#endif


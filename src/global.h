#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include "config.h"
#include <iostream>
#include <string>

#define Cerr std::cerr
#define Cout std::cout
#define Cin std::cin
#define Endl std::endl

#define String std::string
#define CHARS2STR(a) String(a)

_START_MYJFM_NAMESPACE_

class Global {
public:
  Global();
  ~Global();
  void init(String& v_cur_path, String& config_file_name);
  void parse_config();
  void set_save_path(String path);
  String& get_save_path();
  void set_depth(String& dep);
private:
  // current work path
  String _cur_path;

  //Mutex _mutex;
  // if there exists multi-threads, should guarantee consistency by mutex
  volatile bool _has_init;
  String _config_file;

  // the path saved all the web pages and all the indexes
  String _save_path;
  int _depth;
};

_END_MYJFM_NAMESPACE_

#endif


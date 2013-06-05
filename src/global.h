#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <iostream>
#include <string>

#define Cerr std::cerr
#define Cout std::cout
#define Cin std::cin
#define Endl std::endl

#define String std::string
#define CHARS2STR(a) String(a)

class Global {
public:
  Global();
  ~Global();
  void init(String& v_cur_path, String& config_file_name);
  void parse_config();
private:
  // current work path
  String cur_path;

  //Mutex _mutex;
  // if there exists multi-threads, should guarantee consistency by mutex
  volatile bool has_init;
  String config_file;

  // the path saved all the web pages and all the indexes
  String save_path;
};

#endif


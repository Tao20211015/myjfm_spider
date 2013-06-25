#include "config.h"
#include "global.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>

_START_MYJFM_NAMESPACE_

class Utility;

Global::Global() : 
  _has_init(false), 
  _config_file(""), 
  _cur_path(""), 
  _depth(5), 
  _thread_num(5) {
  _file_types.clear();
  _seed_urls.clear();
}

Global::~Global() {
}

void Global::init(String& v_cur_path, String& config_file_name) {
  if (_has_init) {
    return;
  }

  if (config_file_name == "") {
    exit(1);
  }

  // It should be initialized here and JUST ONCE!!!
  _has_init = true;

  _cur_path = v_cur_path;
  _config_file = config_file_name;
  _depth = 5;
  _thread_num = 5;

  _seed_urls.clear();

  load_default_file_types();

  parse_config();
}

void Global::load_default_file_types() {
  _file_types.clear();
  _file_types.push_back(".htm");
  _file_types.push_back(".html");
}

void Global::parse_config() {
  ASSERT(_has_init);

  if (_config_file == "") {
    exit(1);
  } else {
    FILE* config_file_p = fopen(_config_file.c_str(), "r");

    if (config_file_p == NULL) {
      Cerr << "[Error] Configure file in your home directory can't be read." 
        << Endl << "[Error] You must specify the correct configure file." 
        << Endl << "[Error] Exit..." << Endl;
      exit(1);
    }

#define MAX_BUF_LEN 1024
    char buffer[MAX_BUF_LEN];
    while (fgets(buffer, MAX_BUF_LEN - 1, config_file_p)) {
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
      } else if (key_and_value[0] == "DEPTH") {
        set_depth(key_and_value[1]);
      } else if (key_and_value[0] == "THREADS") {
        set_thread_num(key_and_value[1]);
      } else if (key_and_value[0] == "FILETYPES") {
        set_file_types(key_and_value);
      } else if (key_and_value[0] == "SEEDURLS") {
        set_seed_urls(key_and_value);
      } else {
        continue;
      }
    }

    if (_seed_urls.empty()) {
      exit(1);
    }
#undef MAX_BUF_LEN
  }
}

void Global::set_seed_urls(Vector<String>& seed_urls) {
  _seed_urls.clear();
  int i;
  for (i = 1; i < seed_urls.size(); ++i) {
    _seed_urls.push_back(seed_urls[i]);
  }
}

void Global::set_file_types(Vector<String>& file_types) {
  _file_types.clear();
  int i;
  for (i = 1; i < file_types.size(); ++i) {
    _file_types.push_back(file_types[i]);
  }
}

void Global::set_thread_num(String& thread_num) {
  ASSERT(_has_init);
  _thread_num = atoi(thread_num.c_str());
}

void Global::set_save_path(String& path) {
  ASSERT(_has_init);
  _save_path = path;
}

String& Global::get_save_path() {
  ASSERT(_has_init);
  return _save_path;
}

void Global::set_depth(String& dep) {
  ASSERT(_has_init);
  _depth = atoi(dep.c_str());
}

_END_MYJFM_NAMESPACE_


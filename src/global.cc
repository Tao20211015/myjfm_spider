#include "config.h"
#include "global.h"
#include "utility.h"

#include <stdio.h>
#include <stdlib.h>

_START_MYJFM_NAMESPACE_

class Utility;

Global::Global() {
  _has_init = false;
  _config_file = "";
  _cur_path = "";
  _depth = 5;
}

Global::~Global() {
}

void Global::init(String& v_cur_path, String& config_file_name) {
  if (_has_init) {
    return;
  }

  // It should be initialized here and JUST ONCE!!!
  _has_init = true;

  _cur_path = v_cur_path;
  _config_file = config_file_name;
  _depth = 5;
  parse_config();
}

void Global::parse_config() {
  ASSERT(_has_init);

  if (_config_file == "") {
  } else {
    FILE* config_file_p = fopen(_config_file.c_str(), "r");

    if (config_file_p == NULL) {
      Cerr << "Warning: Configure file in your home directory can't be read." 
        << Endl 
        << "         Will use the default configure options instead." 
        << Endl;
      _config_file = "";
    }

#define MAX_BUF_LEN 1024
    char buffer[MAX_BUF_LEN];
    while (fgets(buffer, MAX_BUF_LEN - 1, config_file_p)) {
      Utility::trim(buffer);
      if (buffer[0] == '\0' || buffer[0] == '#') {
        continue;
      }

      String line = buffer;
      String separator = " ";
      Vector<String> key_and_value;
      Utility::split(line, separator, key_and_value);

      if (key_and_value.size() != 2 || 
          key_and_value[0].length() <= 0 || 
          key_and_value[1].length() <= 0) {
        continue;
      }

      if (key_and_value[0] == "SAVEPATH") {
        set_save_path(key_and_value[1]);
      } else if (key_and_value[0] == "DEPTH") {
        set_depth(key_and_value[1]);
      } else {
        continue;
      }
    }
#undef MAX_BUF_LEN
  }
}

void Global::set_save_path(String path) {
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


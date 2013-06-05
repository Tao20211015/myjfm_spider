#include "utility.h"
#include "global.h"

#include <stdio.h>
#include <stdlib.h>

Global::Global() {
  has_init = false;
  config_file = "";
  cur_path = "";
}

Global::~Global() {
}

void Global::init(String& v_cur_path, String& config_file_name) {
  if (has_init) {
    return;
  }

  // It should be initialized here and JUST ONCE!!!
  has_init = true;

  cur_path = v_cur_path;
  config_file = config_file_name;
  parse_config();
}

void Global::parse_config() {
  ASSERT(has_init);

  if (config_file == "") {
  } else {
    FILE* config_file_p = fopen(config_file.c_str(), "r");
    if (config_file_p == NULL) {
      Cerr << "Warning: Configure file in your home directory can't be read." 
        << Endl 
        << "         Will use the default configure options instead." 
        << Endl;
      config_file = "";
    }

#define MAX_BUF_LEN 1024
    char buffer[MAX_BUF_LEN];
    while (fgets(buffer, MAX_BUF_LEN - 1, config_file_p)) {
      char* line = trim(buffer);
      if (!line || *line == '#') {
        continue;
      }
      String sline = line, key, value;
      split_to_kv(sline, key, value);
      if (!key.length() || !value.length()) {
        continue;
      }

      if (key == "SAVEPATH") {
        set_save_path(value);
      } else if (key == "DEPTH") {
        set_depth(value);
      } else {
        continue;
      }
    }
#undef MAX_BUF_LEN
  }
}

void Global::set_save_path(String path) {
  ASSERT(has_init);
  save_path = path;
}

String& Global::get_save_path() {
  ASSERT(has_init);
  return save_path;
}


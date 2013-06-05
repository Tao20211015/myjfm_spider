#include "global.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// the global variable
// contains many information of the server
// e.x. the configure options
// before access this object, you must invoke the
// init(config_file) member function to init the object
Global glob;

void usage(char *argv0) {
  Cerr << "Usage: " << argv0 << " [-f configure_file_name]" << Endl;
}

void load_config(String cur_path, String config_file_name) {
  int res = access(config_file_name.c_str(), F_OK);
  if (res != 0) {
    Cerr << "Warning: The configure file '" 
      << config_file_name << "'is not exist." << Endl 
      << " Will use the default configure options instead." << Endl;
    config_file_name = "";
  }
  // init the object
  glob.init(cur_path, config_file_name);
}

void parse_args(int argc, char *argv[]) {
  String full_config_file_name;
#define MAX_BUF_LEN 1024
  char buffer[MAX_BUF_LEN];
  getcwd(buffer, MAX_BUF_LEN);
#undef MAX_BUF_LEN
  String cur_path(buffer);
  if (argc == 1) {
    full_config_file_name = cur_path + "/myjfmspider.conf";
    load_config(cur_path, full_config_file_name);
  } else if (argc == 3 && !strcmp(argv[1], "-f")) {
    strcpy(buffer, argv[2]);
    if (buffer[0] == '.' && buffer[1] == '/') {
      full_config_file_name = cur_path + &(buffer[1]);
    } else if (buffer[0] == '~' && buffer[1] == '/') {
      full_config_file_name = CHARS2STR(getenv("HOME")) + &(buffer[1]);
    } else if (buffer[0] == '/') {
      full_config_file_name = buffer;
    } else {
      full_config_file_name = cur_path + "/" + buffer;
    }
    load_config(cur_path, full_config_file_name);
  } else if (argc == 2 && 
      (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
    usage(argv[0]);
    exit(0);
  } else {
    usage(argv[0]);
    exit(1);
  }
}

int main(int argc, char *argv[]) {
  parse_args(argc, argv);
  return 0;
}


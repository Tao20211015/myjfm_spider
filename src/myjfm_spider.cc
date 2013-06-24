#include "config.h"
#include "global.h"
#include "utility.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// the global variable
// contains many information of the server
// e.x. the configure options
// before access this object, you must invoke the
// init(config_file) member function to init the object
_MYJFM_NAMESPACE_::Global glob;

void usage(char *argv0) {
  Cerr << "[Usage] " << argv0 << " [-f configure_file_name]" << Endl;
}

static void load_config(String cur_path, String config_file_name) {
  int res = access(config_file_name.c_str(), F_OK);
  if (res != 0) {
    Cerr << "[Error] The configure file '" 
      << config_file_name << "'is not exist." << Endl
      << "[Error] You must specify the configure file using -f option or " << 
      "put the configure file into current directory." << Endl;
    exit(1);
  }

  // init the object
  glob.init(cur_path, config_file_name);
}

void parse_args(int argc, char *argv[]) {
#define MAX_BUF_LEN 1024
  char buffer[MAX_BUF_LEN];
  getcwd(buffer, MAX_BUF_LEN);
#undef MAX_BUF_LEN
  String cur_path(buffer);

  if (argc == 1) {
    String full_config_file_name = cur_path + "/myjfmspider.conf";
    load_config(cur_path, full_config_file_name);
  } else if (argc == 3 && !strcmp(argv[1], "-f")) {
    load_config(cur_path, 
        _MYJFM_NAMESPACE_::Utility::get_file_full_path(CHARS2STR(argv[2])));
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


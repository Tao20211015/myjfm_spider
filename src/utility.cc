#include "global.h"
#include <string.h>

char *trim(char *str) {
  if (!str || !(*str)) {
    return NULL;
  }
  char *head = str;
  while (*head == ' ') {
    head++;
  }
  if (*head == '\0') {
    return NULL;
  }
  char *tail = head;
  while (*tail) {
    tail++;
  }
  tail--;
  while (*tail == ' ') {
    tail--;
  }
  *(++tail) = '\0';
  return head;
}

void split_to_kv(String& line, String& key, String& value) {
#define MAX_BUF_LEN 1024
  char buffer[MAX_BUF_LEN];
  memcpy(buffer, line.c_str(), line.length());
  char* pivot = buffer;
  while (*pivot && *pivot != ' ') {
    pivot++;
  }
  if (!*pivot) {
    key = "";
    value = "";
  }
  *pivot = '\0';
  key = buffer;
  value = pivot + 1;
#undef MAX_BUF_LEN
}

String get_file_full_path(String& path) {
  if (!path.length()) {
    return CHARS2STR("");
  }

  if (path.length() == 1) {
    if (path[0] == '.') {
      return CHARS2STR("");
    } else if (path[0] == '~') {
      return CHARS2STR(getenv("HOME"));
    } else if (path[0] == '/') {
      return CHARS2STR("/");
    } else {
#define MAX_BUF_LEN 1024
      char buffer[MAX_BUF_LEN];
      getcwd(buffer, MAX_BUF_lEN);
      return CHARS2STR(buffer);
    }
  }

  if (buffer[0] == '.' && buffer[1] == '/') {
    full_config_file_name = cur_path + &(buffer[1]);
  } else if (buffer[0] == '~' && buffer[1] == '/') {
    full_config_file_name = CHARS2STR(getenv("HOME")) + &(buffer[1]);
  } else if (buffer[0] == '/') {
    full_config_file_name = buffer;
  } else {
    full_config_file_name = cur_path + "/" + buffer;
  }
}


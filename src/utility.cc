#include "global.h"
#include <string.h>

// eliminate all the blank chars from the head and tail of the string
// NOTE: the parameter and return value are both char * strings
char *trim(char *str) {
  if (!str || !(*str)) {
    return NULL;
  }
  char *head = str;
  while (*head == ' ' || *head == '\t') {
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
  while (*tail == ' ' || *head == '\t') {
    tail--;
  }
  *(++tail) = '\0';
  return head;
}

// split the string into key and value
// you should always input the correct string, which means,
// the first parameter 'line' should always contains one and 
// only one space char(' ')
void split_to_kv(String& line, String& key, String& value) {
#define MAX_BUF_LEN 1024
  char buffer[MAX_BUF_LEN];
  strcpy(buffer, line.c_str());
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

// get the full path
// NOTE: this does not support multiple consecutive '.'s or '..'s, 
// for example './../.././' etc.
// please do not give the invalid path name, otherwise, 
// the result will be confused
String get_file_full_path(String path) {
#define MAX_BUF_LEN 512
  char buffer[MAX_BUF_LEN];
  int i, j;

  if (!path.length()) {
    return CHARS2STR("");
  }

  if (path[0] == '/') {
    return path;
  }

  if (path == "." || path == "./") {
    getcwd(buffer, MAX_BUF_LEN);
    return CHARS2STR(buffer);
  }

  if (path[0] == '.' && path[1] == '/') {
    getcwd(buffer, MAX_BUF_LEN);
    for (i = strlen(buffer) - 1, j = 1; path[j]; ++j, ++i) {
      buffer[i] = path[j];
    }
    buffer[i] = '\0';
    return CHARS2STR(buffer);
  }

  if (path == ".." || 
      (path.length() > 2 && path[0] == '.' && 
       path[1] == '.' && path[2] == '/')) {
    getcwd(buffer, MAX_BUF_LEN);
    i = strlen(buffer) - 1;
    while (buffer[i] != '/') {
      i--;
    }
    if (i == 0) {
      buffer[++i] = '\0';
    } else {
      buffer[i] = '\0';
    }

    if (path == "..") {
      return CHARS2STR(buffer);
    }

    if (i == 1) {
      j = 3;
    } else {
      j = 2;
    }
    for (; j < path.length(); ++j, ++i) {
      buffer[i] = path[j];
    }
    buffer[i] = '\0';
    return CHARS2STR(buffer);
  }

  if (path == "~" || path == "~/") {
    return CHARS2STR(getenv("HOME"));
  }

  if (path[0] == '~' && path[1] == '/') {
    strcpy(buffer, getenv("HOME"));
    for (i = strlen(buffer), j = 1; j < path.length(); ++j, ++i) {
      buffer[i] = path[j];
    }
    buffer[i] = '\0';
    return CHARS2STR(buffer);
  }
  getcwd(buffer, MAX_BUF_LEN);
  return CHARS2STR(buffer) + "/" + path;
#undef MAX_BUF_LEN
}


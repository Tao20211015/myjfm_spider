/*******************************************************************************
 * utility.cc - the utility function
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdlib.h>
#include <string.h>

#include "config.h"
#include "utility.h"

_START_MYJFM_NAMESPACE_

void Utility::ltrim(char* str) {
  if (!str) {
    return;
  }

  char* p = str;
  while (::isspace(*p)) {
    p++;
  }

  if (p == str) {
    return;
  }

  while ((*str++ = *p++));
}

String Utility::ltrim(const String& str) {
  const char* p = str.c_str();
  while (::isspace(*p)) {
    p++;
  }
  return CHARS2STR(p);
}

void Utility::rtrim(char* str) {
  if (!str) {
    return;
  }

  char* p = str + strlen(str) - 1;

  while (p >= str && ::isspace(*p)) {
    *p = '\0';
    p--;
  }
}

String Utility::rtrim(const String& str) {
  if (str.length() <= 0) {
    return CHARS2STR("");
  }
  const char* head = str.c_str();

  const char* tail = head + strlen(head) - 1;
  while (tail >= head && ::isspace(*tail)) {
    tail--;
  }

  String res(head, tail - head + 1);
  return res;
}

void Utility::trim(char* str) {
  Utility::ltrim(str);
  Utility::rtrim(str);
}

String Utility::trim(const String& str) {
  return Utility::rtrim(Utility::ltrim(str));
}

void Utility::toupper(char* str) {
  if (!str) {
    return;
  }

  while (*str) {
    *str = ::toupper(*str);
    str++;
  }
}

String Utility::toupper(const String& str) {
  String res = str;
  int i;
  for (i = 0; i < res.length(); ++i) {
    res[i] = ::toupper(res[i]);
  }
  return res;
}

void Utility::toupper_inplace(String& str) {
  int i;
  for (i = 0; i < str.length(); ++i) {
    str[i] = ::toupper(str[i]);
  }
}

void Utility::tolower(char* str) {
  if (!str) {
    return;
  }

  while (*str) {
    *str = ::tolower(*str);
    str++;
  }
}

String Utility::tolower(const String& str) {
  String res = str;
  int i;
  for (i = 0; i < res.length(); ++i) {
    res[i] = ::tolower(res[i]);
  }
  return res;
}

void Utility::tolower_inplace(String& str) {
  int i;
  for (i = 0; i < str.length(); ++i) {
    str[i] = ::tolower(str[i]);
  }
}

void Utility::split(const String& str, 
    const String& separator, 
    Vector<String>& container) {

  container.clear();

  if (separator.length() <= 0) {
    container.push_back(str);
    return;
  }

  String_size_t pre = 0;
  String_size_t cur;

  while (1) {
    cur = str.find_first_of(separator, pre);
    if (cur == String_tail) {
      cur = str.length();
      if (cur != pre) {
        container.push_back(String(str.c_str() + pre, cur - pre));
      }
      break;
    } else {
      if (cur != pre) {
        container.push_back(String(str.c_str() + pre, cur - pre));
      }
    }
    pre = cur + 1;
  }
}

// get the full path
// NOTE: this does not support multiple consecutive '.'s or '..'s, 
// for example './../.././' etc.
// please do not give the invalid path name, otherwise, 
// the result will be confused
String Utility::get_file_full_path(String path) {
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

_END_MYJFM_NAMESPACE_


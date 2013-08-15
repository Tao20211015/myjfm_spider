/*******************************************************************************
 * utility.cc - the utility function
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <stdint.h>
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
  if (str.length() == 0) {
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
  uint32_t i;
  for (i = 0; i < res.length(); ++i) {
    res[i] = ::toupper(res[i]);
  }
  return res;
}

void Utility::toupper_inplace(String& str) {
  uint32_t i;
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
  uint32_t i;
  for (i = 0; i < res.length(); ++i) {
    res[i] = ::tolower(res[i]);
  }

  return res;
}

void Utility::tolower_inplace(String& str) {
  uint32_t i;
  for (i = 0; i < str.length(); ++i) {
    str[i] = ::tolower(str[i]);
  }
}

void Utility::split(const String& str, 
    const String& separator, 
    Vector<String>& container) {

  container.clear();

  if (separator.length() == 0) {
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
  char buffer[512];
  uint32_t i;
  uint32_t j;

  if (path.length() == 0) {
    return CHARS2STR("");
  }

  if (path[0] == '/') {
    return path;
  }

  if (path == "." || path == "./") {
    getcwd(buffer, 512);
    return CHARS2STR(buffer);
  }

  if (path[0] == '.' && path[1] == '/') {
    getcwd(buffer, 512);
    for (i = strlen(buffer) - 1, j = 1; path[j]; ++j, ++i) {
      buffer[i] = path[j];
    }
    buffer[i] = '\0';
    return CHARS2STR(buffer);
  }

  if (path == ".." || 
      (path.length() > 2 && path[0] == '.' && 
       path[1] == '.' && path[2] == '/')) {
    getcwd(buffer, 512);
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
  getcwd(buffer, 512);
  return CHARS2STR(buffer) + "/" + path;
}

bool Utility::is_hex_digit(char p) {
  if ((p >= '0' && p <= '9') || 
      (p >= 'a' && p <= 'z') || 
      (p >= 'A' && p <= 'Z')) {
    return true;
  }

  return false;
}

RES_CODE Utility::str2hex(const char* str, uint32_t& number) {
  if (!str) {
    return S_FAIL;
  }

  const char* p = str;
  number = 0;

  if (*p == '0') {
    number = 0;
    return S_OK;
  }

  while (is_hex_digit(*p)) {
    if (*p >= '0' && *p <= '9') {
      number = number * 16 + *p - '0';
    } else if (*p >= 'a' && *p <= 'z') {
      number = number * 16 + *p - 'a';
    } else {
      number = number * 16 + *p - 'A';
    }

    p++;
  }

  return S_OK;
}

RES_CODE Utility::str2hex(String& str, uint32_t number) {
  return Utility::str2hex(str.c_str(), number);
}

char* Utility::strdupn(const char* src, int n) {
  if (src == NULL || n < 0) {
    return NULL;
  }

  char* res = new char[n + 1];
  if (res) {
    memcpy(res, src, n);
    res[n] = '\0';
  }

  return res;
}

char* Utility::strdup(const char* src) {
  if (src == NULL) {
    return NULL;
  }

  int n = strlen(src);
  char* res = new char[n + 1];
  if (res) {
    memcpy(res, src, n);
    res[n] = '\0';
  }

  return res;
}

int Utility::strcmp(const char* str1, const char* str2) {
  if (str1 == NULL || str2 == NULL) {
    return -2;
  }
  while (*str1 && *str2) {
    uint8_t a = *str1++;
    uint8_t b = *str2++;
    if (a > b) {
      return 1;
    } else if (a < b) {
      return -1;
    }
  }
  if (*str1) {
    return 1;
  }
  if (*str2) {
    return -1;
  }
  return 0;
}

int Utility::strcasecmp(const char* str1, const char* str2) {
  if (str1 == NULL || str2 == NULL) {
    return -2;
  }
  while (*str1 && *str2) {
    uint8_t a = *str1++;
    uint8_t b = *str2++;
    a = (a >= 'A' && a <= 'Z') ? (a - 'A' + 'a') : a;
    b = (b >= 'A' && b <= 'Z') ? (b - 'A' + 'a') : b;
    if (a > b) {
      return 1;
    } else if (a < b) {
      return -1;
    }
  }
  if (*str1) {
    return 1;
  }
  if (*str2) {
    return -1;
  }
  return 0;
}

int Utility::strncasecmp(const char* str1, const char* str2, uint32_t n) {
  if (str1 == NULL || str2 == NULL) {
    return -2;
  }
  while (*str1 && *str2 && n) {
    uint8_t a = *str1++;
    uint8_t b = *str2++;
    n--;
    a = (a >= 'A' && a <= 'Z') ? (a - 'A' + 'a') : a;
    b = (b >= 'A' && b <= 'Z') ? (b - 'A' + 'a') : b;
    if (a > b) {
      return 1;
    } else if (a < b) {
      return -1;
    }
  }
  if (n == 0) {
    return 0;
  }
  if (*str1) {
    return 1;
  }
  if (*str2) {
    return -1;
  }
}

RES_CODE Utility::escape(String& str, String& escstr) {
  escstr = "";
  int i;
  for (i = 0; i < str.length(); ++i) {
    if (IS_URI_CHR(str[i])) {
      escstr.append(1, str[i]);
    } else {
      char buffer[4];
      sprintf(buffer, "%%%X%X", (unsigned char)str[i] >> 4, str[i] & 0x0f);
      escstr += buffer;
    }
  }

  return S_OK;
}

_END_MYJFM_NAMESPACE_


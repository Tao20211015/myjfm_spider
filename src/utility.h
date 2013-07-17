/*******************************************************************************
 * utility.h - the utility function
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <stdint.h>
#include <assert.h>

#include "config.h"

#define ASSERT(exp) assert(exp)

_START_MYJFM_NAMESPACE_

class Utility {
public:
  static void ltrim(char* str);
  static String ltrim(const String& str);

  static void rtrim(char* str);
  static String rtrim(const String& str);

  static void trim(char* str);
  static String trim(const String& str);
  
  static void toupper(char* str);
  static String toupper(const String& str);
  static void toupper_inplace(String& str);

  static void tolower(char* str);
  static String tolower(const String& str);
  static void tolower_inplace(String& str);

  static void split(const String& str, 
      const String& separator, 
      Vector<String>& container);
  
  static String get_file_full_path(String path);

  template <class T>
  static RES_CODE str2integer(const char* str, T& number) {
    if (!str) {
      return S_FAIL;
    }
    
    Stringstream ss;
    T tmp;
    bool ret = false;
    ss << str;
    if (ss.str().length() > 0) {
      try {
        ss >> tmp;
        if (ss.eof() && !ss.fail()) {
          ret = true;
          number = tmp;
        }
      } catch (std::ios_base::failure&) {
      }
    }
    
    if (ret) {
      return S_OK;
    } else {
      return S_FAIL;
    }
  }

  template <class T>
  static RES_CODE str2integer(String& str, T& number) {
    return Utility::str2integer(str.c_str(), number);
  }

  static bool is_hex_digit(char p);
  static RES_CODE str2hex(const char* str, uint32_t& number);
  static RES_CODE str2hex(String& str, uint32_t number);
};

_END_MYJFM_NAMESPACE_

#endif


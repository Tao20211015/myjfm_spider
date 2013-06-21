#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "config.h"
#include "global.h"
#include <assert.h>

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
};

_END_MYJFM_NAMESPACE_

#endif


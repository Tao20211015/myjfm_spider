#ifndef _UTILITY_H_
#define _UTILITY_H_

#include <assert.h>
#include "global.h"

#define ASSERT(exp) assert(exp)

char* trim(char* str);
void split_to_kv(String& line, String& key, String& value);

#endif


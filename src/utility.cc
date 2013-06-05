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


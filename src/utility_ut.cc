#include <string>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>

#include "utility.h"

#define String std::string
#define CHARS2STR(a) String(a)

void trim_test() {
  char a[100] = "   trim trim  ";
  std::cout << "a:\"" << a << "\" -> " << trim(a) << std::endl;
  strcpy(a, " trim");
  std::cout << "a:\"" << a << "\" -> " << trim(a) << std::endl;
  strcpy(a, "trim trim   ");
  std::cout << "a:\"" << a << "\" -> " << trim(a) << std::endl;
  strcpy(a, "t");
  std::cout << "a:\"" << a << "\" -> " << trim(a) << std::endl;
  strcpy(a, " t");
  std::cout << "a:\"" << a << "\" -> " << trim(a) << std::endl;
  strcpy(a, "t ");
  std::cout << "a:\"" << a << "\" -> " << trim(a) << std::endl;
}

void split_to_kv_test() {
  String line = "a 1";
  String key, value;
  split_to_kv(line, key, value);
  std::cout << "line:\"" << line << "\"  " << key << " -> " << value << std::endl;
}

void get_file_full_path_test() {
  String a = "..";
  std::cout << ".. -> " << get_file_full_path(a) << std::endl;
  a = ".";
  std::cout << ". -> " << get_file_full_path(a) << std::endl;
  a = "./";
  std::cout << "./ -> " << get_file_full_path(a) << std::endl;
  a = "../src";
  std::cout << "../src -> " << get_file_full_path(a) << std::endl;
  a = "~";
  std::cout << "~ -> " << get_file_full_path(a) << std::endl;
  a = "~/";
  std::cout << "~/ -> " << get_file_full_path(a) << std::endl;
  a = "~/ab/a";
  std::cout << "~/ab/a -> " << get_file_full_path(a) << std::endl;
  a = "/usr/loca/bin";
  std::cout << "/usr/local/bin -> " << get_file_full_path(a) << std::endl;
  a = "/";
  std::cout << "/ -> " << get_file_full_path(a) << std::endl;
  a = "a/b";
  std::cout << "a/b -> " << get_file_full_path(a) << std::endl;
}

int main() {
  trim_test();
  split_to_kv_test();
  get_file_full_path_test();
  return 0;
}


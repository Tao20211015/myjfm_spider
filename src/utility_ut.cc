#include <string>
#include <vector>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>

#include "utility.h"

void trim_test() {
  char a[100] = "   trim trim  ";
  myjfm::Utility::trim(a);
  assert(strcmp(a, "trim trim") == 0);

  strcpy(a, " trim");
  myjfm::Utility::trim(a);
  assert(strcmp(a, "trim") == 0);

  strcpy(a, "trim trim   ");
  myjfm::Utility::trim(a);
  assert(strcmp(a, "trim trim") == 0);

  strcpy(a, "t");
  myjfm::Utility::trim(a);
  assert(strcmp(a, "t") == 0);

  strcpy(a, " t");
  myjfm::Utility::trim(a);
  assert(strcmp(a, "t") == 0);

  strcpy(a, "t ");
  myjfm::Utility::trim(a);
  assert(strcmp(a, "t") == 0);

  std::string b = "   trim trim  ";
  assert(myjfm::Utility::trim(b) == "trim trim");

  b = " trim";
  assert(myjfm::Utility::trim(b) == "trim");

  b = "trim trim   ";
  assert(myjfm::Utility::trim(b) == "trim trim");

  b = "t";
  assert(myjfm::Utility::trim(b) == "t");

  b = " t";
  assert(myjfm::Utility::trim(b) == "t");

  b = "t ";
  assert(myjfm::Utility::trim(b) == "t");
}

void toupper_test() {
  char a[] = "AbcdE 1";
  myjfm::Utility::toupper(a);
  assert(strcmp(a, "ABCDE 1") == 0);

  std::string b = "AbcdE 1";
  assert(myjfm::Utility::toupper(b) == "ABCDE 1");

  std::string c = "AbcdE 1";
  myjfm::Utility::toupper_inplace(c);
  assert(c == "ABCDE 1");
}

void tolower_test() {
  char a[] = "AbcdE 1";
  myjfm::Utility::tolower(a);
  assert(strcmp(a, "abcde 1") == 0);

  std::string b = "AbcdE 1";
  assert(myjfm::Utility::tolower(b) == "abcde 1");

  std::string c = "AbcdE 1";
  myjfm::Utility::tolower_inplace(c);
  assert(c == "abcde 1");
}

void split_test() {
  std::string line = "a 1";
  std::vector<std::string> key_and_value;
  myjfm::Utility::split(line, " ", key_and_value);
  assert(key_and_value.size() == 2 && key_and_value[0] == "a" && key_and_value[1] == "1");
}

void get_file_full_path_test() {
  std::string a = "..";
  std::cout << ".. -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = ".";
  std::cout << ". -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "./";
  std::cout << "./ -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "../src";
  std::cout << "../src -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "~";
  std::cout << "~ -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "~/";
  std::cout << "~/ -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "~/ab/a";
  std::cout << "~/ab/a -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "/usr/loca/bin";
  std::cout << "/usr/local/bin -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "/";
  std::cout << "/ -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
  a = "a/b";
  std::cout << "a/b -> " << myjfm::Utility::get_file_full_path(a) << std::endl;
}

int main() {
  trim_test();
  toupper_test();
  tolower_test();
  split_test();
  get_file_full_path_test();
  return 0;
}


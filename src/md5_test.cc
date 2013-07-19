#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "config.h"
#include "md5.h"
#include "url.h"

const uint32_t shuffle_size = 10;
const uint32_t random_times = 1000000;
const double avg_rate = 1.0 / double(shuffle_size);
const double avg_delta = 0.001; // 0.1%

String dict[62] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", 
  "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", 
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
  "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

String random_string(uint32_t len) {
  uint32_t i;
  String res = "";
  for (i = 0; i < len; ++i) {
    res += dict[rand() % 62];
  }
  return res;
}

String random_site(uint32_t site_len) {
  return random_string(site_len);
}

String random_file(uint32_t file_len) {
  return random_string(file_len);
}

String random_args(uint32_t args_len) {
  return random_string(args_len);
}

void md5_shuffle_test() {
  uint32_t i;
  srand((unsigned)time(NULL));
  uint32_t *count = new uint32_t[shuffle_size];
  memset(count, 0, sizeof(uint32_t) * shuffle_size);

  double *rate = new double[shuffle_size];
  memset(rate, 0, sizeof(double) * shuffle_size);

  double *delta = new double[shuffle_size];
  memset(delta, 0, sizeof(double) * shuffle_size);

  for (i = 0; i < random_times; ++i) {
    uint32_t site_len = (rand() % 50) + 1;
    String site = random_site(site_len);

    uint32_t file_len = (rand() % 50) + 1;
    String file = random_file(file_len);

    uint32_t args_len = (rand() % 100) + 1;
    String args = random_args(args_len);

    String str_url = "http://www.";
    str_url += site + ".com/" + file + ".html?" + args;
    _MYJFM_NAMESPACE_::Url url(str_url);
    _MYJFM_NAMESPACE_::MD5 md5;
    url.get_md5(md5);
    uint32_t index = 0;
    md5.shuffle(shuffle_size, index);
    count[index]++;
  }

  for (i = 0; i < shuffle_size; ++i) {
    rate[i] = double(count[i]) / double(random_times);
    delta[i] = fabs(rate[i] - avg_rate);
    Cout << i << " -> " << count[i] 
      << " && " << rate[i] << " && " << delta[i] << Endl;
  }

  for (i = 0; i < shuffle_size; ++i) {
    assert(delta[i] < avg_delta);
  }

  delete [] count;
  delete [] rate;
  delete [] delta;
}

void md5_test() {
  String str;

  _MYJFM_NAMESPACE_::MD5Caculator md5_1(String(""));
  md5_1.to_string(str);
  assert(str == "d41d8cd98f00b204e9800998ecf8427e");

  _MYJFM_NAMESPACE_::MD5Caculator md5_2(String("a"));
  md5_2.to_string(str);
  assert(str == "0cc175b9c0f1b6a831c399e269772661");

  _MYJFM_NAMESPACE_::MD5Caculator md5_3(String("message digest"));
  md5_3.to_string(str);
  assert(str == "f96b697d7cb7938d525a2f31aaf161d0");
}

int main() {
  md5_test();
  md5_shuffle_test();
  return 0;
}


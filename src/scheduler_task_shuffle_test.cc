/*******************************************************************************
 * test the scheduler task's shuffle algorithm
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>
#include <math.h>

#include "config.h"
#include "md5.h"
#include "url.h"

const int shuffle_size = 10;
const int random_times = 1000000;
const double avg_rate = 1.0 / double(shuffle_size);
const double avg_delta = 0.001; // 0.1%

String dict[62] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", 
  "a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m", 
  "n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z", 
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M", 
  "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"};

String random_string(int len) {
  int i;
  String res = "";
  for (i = 0; i < len; ++i) {
    res += dict[rand() % 62];
  }
  return res;
}

String random_site(int site_len) {
  return random_string(site_len);
}

String random_file(int file_len) {
  return random_string(file_len);
}

String random_args(int args_len) {
  return random_string(args_len);
}

int main() {
  int i;
  srand((unsigned)time(NULL));
  int *count = new int[shuffle_size];
  memset(count, 0, sizeof(int) * shuffle_size);

  double *rate = new double[shuffle_size];
  memset(rate, 0, sizeof(double) * shuffle_size);

  double *delta = new double[shuffle_size];
  memset(delta, 0, sizeof(double) * shuffle_size);

  for (i = 0; i < random_times; ++i) {
    int site_len = (rand() % 50) + 1;
    String site = random_site(site_len);

    int file_len = (rand() % 50) + 1;
    String file = random_file(file_len);

    int args_len = (rand() % 100) + 1;
    String args = random_args(args_len);

    String str_url = "http://www.";
    str_url += site + ".com/" + file + ".html?" + args;
    _MYJFM_NAMESPACE_::Url url(str_url);
    _MYJFM_NAMESPACE_::MD5 md5;
    url.get_md5(md5);
    int j;
    unsigned int index = 0;
    for (j = 0; j < 4; ++j) {
      int tmp = (unsigned int)(md5._value[j << 2]);
      tmp %= shuffle_size;
      index += tmp;
    }

    index %= shuffle_size;

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

  return 0;
}


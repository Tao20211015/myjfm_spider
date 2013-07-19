/*******************************************************************************
 * md5.h - the md5 algorithm implementation
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#ifndef _MD5_H_
#define _MD5_H_

#include <stdint.h>
#include <string.h>
#include <string>
#include <fstream>

#include "config.h"

_START_MYJFM_NAMESPACE_

struct MD5 {
  uint8_t _value[16];
  MD5() {
    memset(_value, 0, 16);
  }

  bool operator==(MD5& md5) {
    return !memcmp(_value, md5._value, 16);
  }

  RES_CODE shuffle(uint32_t x, uint32_t& index) {
    if (x == 0) {
      return S_FAIL;
    }

    // optimize for the extractors and dnsers
    if (x == 1) {
      index = 0;
      return S_OK;
    }

    // ((_value[0-3] % x) + (_value[4-7] % x) + 
    // (_value[8-11] % x) + (_value[12-15] % x)) % x
    // ignore overflow
    uint32_t i;
    uint32_t* p = (uint32_t*)_value;

    for (i = 0; i < 4; ++i) {
      index += p[i] % x;
    }

    index %= x;

    return S_OK;
  }
};

/* MD5caculator declaration. */
class MD5Caculator {
public:
  MD5Caculator();
  MD5Caculator(const void*, uint32_t);
	MD5Caculator(const String&);
	MD5Caculator(Ifstream&);

	RES_CODE update(const void*, uint32_t);
	RES_CODE update(const String&);
	RES_CODE update(Ifstream&);
	RES_CODE digest(MD5& md5);
	RES_CODE to_string(String&);
	RES_CODE reset();

private:
	MD5Caculator(const MD5Caculator&);
	MD5Caculator& operator=(const MD5Caculator&);

	RES_CODE update(const uint8_t*, uint32_t);
	RES_CODE final();
	const uint8_t* digest();
	RES_CODE transform(const uint8_t block[64]);
	RES_CODE encode(const uint32_t*, uint8_t*, uint32_t);
	RES_CODE decode(const uint8_t*, uint32_t*, uint32_t);
	RES_CODE bytes_to_hexstring(const uint8_t*, uint32_t, String&);

	// state (ABCD)
	uint32_t _state[4];

  // number of bits, modulo 2^64 (low-order word first)
	uint32_t _count[2];

  // input buffer
	uint8_t _buffer[64];

  // message digest
	uint8_t _digest[16];

  // calculate finished ?
	bool _finished;		
  
  // padding for calculate
	static const uint8_t PADDING[64];
	static const char HEX[16];
};

_END_MYJFM_NAMESPACE_

#endif


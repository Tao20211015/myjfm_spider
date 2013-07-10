#ifndef _MD5_H_
#define _MD5_H_

#include <string.h>
#include <string>
#include <fstream>

#include "config.h"

_START_MYJFM_NAMESPACE_

struct MD5 {
  unsigned char _value[16];
  MD5() {
    memset(_value, 0, 16);
  }

  int operator==(MD5& md5) {
    return !memcmp(_value, md5._value, 16);
  }
};

/* MD5caculator declaration. */
class MD5Caculator {
public:
  typedef unsigned char Byte;
  typedef unsigned long Ulong;

  MD5Caculator();
  MD5Caculator(const void*, size_t);
	MD5Caculator(const String&);
	MD5Caculator(Ifstream&);

	RES_CODE update(const void*, size_t);
	RES_CODE update(const String&);
	RES_CODE update(Ifstream&);
	RES_CODE digest(MD5& md5);
	RES_CODE to_string(String&);
	RES_CODE reset();

private:
	MD5Caculator(const MD5Caculator&);
	MD5Caculator& operator=(const MD5Caculator&);

	RES_CODE update(const Byte*, size_t);
	RES_CODE final();
	const Byte* digest();
	RES_CODE transform(const Byte block[64]);
	RES_CODE encode(const Ulong*, Byte*, size_t);
	RES_CODE decode(const Byte*, Ulong*, size_t);
	RES_CODE bytes_to_hexstring(const Byte*, size_t, String&);

	// state (ABCD)
	Ulong _state[4];

  // number of bits, modulo 2^64 (low-order word first)
	Ulong _count[2];

  // input buffer
	Byte _buffer[64];

  // message digest
	Byte _digest[16];

  // calculate finished ?
	bool _finished;		
  
  // padding for calculate
	static const Byte PADDING[64];
	static const char HEX[16];
};

_END_MYJFM_NAMESPACE_

#endif


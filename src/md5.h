#ifndef _MD5_H_
#define _MD5_H_

#include "config.h"
#include "global.h"
#include <string.h>
#include <string>
#include <fstream>

_START_MYJFM_NAMESPACE_

struct MD5 {
  unsigned char _value[16];
  MD5() {
    memset(_value, 0, 16);
  }
  int operator==(MD5& md5) {
    return memcmp(_value, md5._value, 16);
  }
};

/* MD5caculator declaration. */
class MD5caculator {
public:
  typedef unsigned char Byte;
  typedef unsigned long Ulong;

  MD5caculator();
  MD5caculator(const void* input, size_t length);
	MD5caculator(const String& str);
	MD5caculator(Ifstream& in);

	void update(const void* input, size_t length);
	void update(const String& str);
	void update(Ifstream& in);
	void digest(MD5& md5);
	String to_string();
	void reset();

private:
	MD5caculator(const MD5caculator&);
	MD5caculator& operator=(const MD5caculator&);

	void update(const Byte* input, size_t length);
	void final();
	const Byte* digest();
	void transform(const Byte block[64]);
	void encode(const Ulong* input, Byte* output, size_t length);
	void decode(const Byte* input, Ulong* output, size_t length);
	String bytes_to_hexstring(const Byte* input, size_t length);

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


/*******************************************************************************
 * md5.cc - the md5 algorithm implementation
 *
 * Copyright (c) 2013, myjfm <mwxjmmyjfm at gmail dot com>
 * All rights reserved.
 ******************************************************************************/

#include <string.h>

#include "config.h"
#include "md5.h"

_START_MYJFM_NAMESPACE_

#define Byte MD5Caculator::Byte
#define Ulong MD5Caculator::Ulong

const Byte MD5Caculator::PADDING[64] = { 0x80 };
const char MD5Caculator::HEX[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'a', 'b',
	'c', 'd', 'e', 'f'
};

// Constants for transform routine
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// ROTATE_LEFT rotates x left n bits.
#define ROTATE_LEFT(x, n) (((x) << (n)) | ((x) >> (32-(n))))

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + ac; \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + ac; \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + ac; \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + ac; \
	(a) = ROTATE_LEFT ((a), (s)); \
	(a) += (b); \
}

// Default construct.
MD5Caculator::MD5Caculator() {
	reset();
}

// Construct a MD5Caculator object with a input buffer.
MD5Caculator::MD5Caculator(const void* input, size_t length) {
	reset();
	update(input, length);
}

// Construct a MD5Caculator object with a string.
MD5Caculator::MD5Caculator(const String &str) {
	reset();
	update(str);
}

// Construct a MD5Caculator object with a file.
MD5Caculator::MD5Caculator(Ifstream &in) {
	reset();
	update(in);
}

RES_CODE MD5Caculator::digest(MD5& md5) {
	if (!_finished) {
		_finished = true;
		final();
	}
  
  memcpy(md5._value, _digest, 16);

  return S_OK;
}

const Byte* MD5Caculator::digest() {
	if (!_finished) {
		_finished = true;
		final();
	}

  return _digest;
}

// Reset the calculate state
RES_CODE MD5Caculator::reset() {
	_finished = false;
	// reset number of bits.
	_count[0] = _count[1] = 0;
	// Load magic initialization constants.
	_state[0] = 0x67452301;
	_state[1] = 0xefcdab89;
	_state[2] = 0x98badcfe;
	_state[3] = 0x10325476;

  return S_OK;
}

// Updating the context with a input buffer.
RES_CODE MD5Caculator::update(const void* input, size_t length) {
	return update((const Byte*)input, length);
}

// Updating the context with a string.
RES_CODE MD5Caculator::update(const String& str) {
	return update((const Byte*)str.c_str(), str.length());
}

// Updating the context with a file.
RES_CODE MD5Caculator::update(Ifstream& in) {
	if (!in) {
		return S_STREAM_NOT_EXIST;
  }

	Stream_size length;
#define MAX_BUF_LEN 1024
	char buffer[MAX_BUF_LEN];

	while (!in.eof()) {
		in.read(buffer, MAX_BUF_LEN);
#undef MAX_BUF_LEN
		length = in.gcount();

		if (length > 0) {
      RES_CODE res = update(buffer, length);

      if (res != S_OK) {
        in.close();
        return res;
      }
    }
	}

	in.close();

  return S_OK;
}

// MD5 block update operation. Continues an MD5 message-digest operation, 
// processing another message block, and updating the context.
RES_CODE MD5Caculator::update(const Byte* input, size_t length) {
  if (!input || length <= 0) {
    return S_NO_INPUT;
  }

	Ulong i;
  Ulong index;
  Ulong part_len;

	_finished = false;

	// Compute number of bytes mod 64
	index = (Ulong)((_count[0] >> 3) & 0x3f);

	// update number of bits
	if ((_count[0] += ((Ulong)length << 3)) < ((Ulong)length << 3)) {
		_count[1]++;
  }

	_count[1] += ((Ulong)length >> 29);

	part_len = 64 - index;

	// transform as many times as possible.
	if (length >= part_len) {
		memcpy(_buffer + index, input, part_len);
		transform(_buffer);

		for (i = part_len; i + 63 < length; i += 64) {
			transform(input + i);
    }

		index = 0;
	} else {
		i = 0;
	}

	// Buffer remaining input
	memcpy(_buffer + index, input + i, length - i);

  return S_OK;
}

// MD5 finalization. Ends an MD5 message-_digest operation, writing the
// the message _digest and zeroizing the context.
RES_CODE MD5Caculator::final() {
	Byte bits[8];
	Ulong old_state[4];
	Ulong old_count[2];
	Ulong index;
  Ulong pad_len;

	// Save current state and count.
	memcpy(old_state, _state, 16);
	memcpy(old_count, _count, 8);

	// Save number of bits
	encode(_count, bits, 8);

	// Pad out to 56 mod 64.
	index = (Ulong)((_count[0] >> 3) & 0x3f);
	pad_len = (index < 56) ? (56 - index) : (120 - index);
	update(PADDING, pad_len);

	// Append length (before padding)
	update(bits, 8);

	// Store state in digest
	encode(_state, _digest, 16);

	// Restore current state and count.
	memcpy(_state, old_state, 16);
	memcpy(_count, old_count, 8);

  return S_OK;
}

// MD5 basic transformation. Transforms _state based on block.
RES_CODE MD5Caculator::transform(const Byte block[64]) {
	Ulong a = _state[0];
  Ulong b = _state[1];
  Ulong c = _state[2];
  Ulong d= _state[3];
  Ulong x[16];

	decode(block, x, 64);

	// Round 1
	FF (a, b, c, d, x[ 0], S11, 0xd76aa478);
	FF (d, a, b, c, x[ 1], S12, 0xe8c7b756);
	FF (c, d, a, b, x[ 2], S13, 0x242070db);
	FF (b, c, d, a, x[ 3], S14, 0xc1bdceee);
	FF (a, b, c, d, x[ 4], S11, 0xf57c0faf);
	FF (d, a, b, c, x[ 5], S12, 0x4787c62a);
	FF (c, d, a, b, x[ 6], S13, 0xa8304613);
	FF (b, c, d, a, x[ 7], S14, 0xfd469501);
	FF (a, b, c, d, x[ 8], S11, 0x698098d8);
	FF (d, a, b, c, x[ 9], S12, 0x8b44f7af);
	FF (c, d, a, b, x[10], S13, 0xffff5bb1);
	FF (b, c, d, a, x[11], S14, 0x895cd7be);
	FF (a, b, c, d, x[12], S11, 0x6b901122);
	FF (d, a, b, c, x[13], S12, 0xfd987193);
	FF (c, d, a, b, x[14], S13, 0xa679438e);
	FF (b, c, d, a, x[15], S14, 0x49b40821);

	// Round 2
	GG (a, b, c, d, x[ 1], S21, 0xf61e2562);
	GG (d, a, b, c, x[ 6], S22, 0xc040b340);
	GG (c, d, a, b, x[11], S23, 0x265e5a51);
	GG (b, c, d, a, x[ 0], S24, 0xe9b6c7aa);
	GG (a, b, c, d, x[ 5], S21, 0xd62f105d);
	GG (d, a, b, c, x[10], S22,  0x2441453);
	GG (c, d, a, b, x[15], S23, 0xd8a1e681);
	GG (b, c, d, a, x[ 4], S24, 0xe7d3fbc8);
	GG (a, b, c, d, x[ 9], S21, 0x21e1cde6);
	GG (d, a, b, c, x[14], S22, 0xc33707d6);
	GG (c, d, a, b, x[ 3], S23, 0xf4d50d87);
	GG (b, c, d, a, x[ 8], S24, 0x455a14ed);
	GG (a, b, c, d, x[13], S21, 0xa9e3e905);
	GG (d, a, b, c, x[ 2], S22, 0xfcefa3f8);
	GG (c, d, a, b, x[ 7], S23, 0x676f02d9);
	GG (b, c, d, a, x[12], S24, 0x8d2a4c8a);

	// Round 3
	HH (a, b, c, d, x[ 5], S31, 0xfffa3942);
	HH (d, a, b, c, x[ 8], S32, 0x8771f681);
	HH (c, d, a, b, x[11], S33, 0x6d9d6122);
	HH (b, c, d, a, x[14], S34, 0xfde5380c);
	HH (a, b, c, d, x[ 1], S31, 0xa4beea44);
	HH (d, a, b, c, x[ 4], S32, 0x4bdecfa9);
	HH (c, d, a, b, x[ 7], S33, 0xf6bb4b60);
	HH (b, c, d, a, x[10], S34, 0xbebfbc70);
	HH (a, b, c, d, x[13], S31, 0x289b7ec6);
	HH (d, a, b, c, x[ 0], S32, 0xeaa127fa);
	HH (c, d, a, b, x[ 3], S33, 0xd4ef3085);
	HH (b, c, d, a, x[ 6], S34,  0x4881d05);
	HH (a, b, c, d, x[ 9], S31, 0xd9d4d039);
	HH (d, a, b, c, x[12], S32, 0xe6db99e5);
	HH (c, d, a, b, x[15], S33, 0x1fa27cf8);
	HH (b, c, d, a, x[ 2], S34, 0xc4ac5665);

	// Round 4
	II (a, b, c, d, x[ 0], S41, 0xf4292244);
	II (d, a, b, c, x[ 7], S42, 0x432aff97);
	II (c, d, a, b, x[14], S43, 0xab9423a7);
	II (b, c, d, a, x[ 5], S44, 0xfc93a039);
	II (a, b, c, d, x[12], S41, 0x655b59c3);
	II (d, a, b, c, x[ 3], S42, 0x8f0ccc92);
	II (c, d, a, b, x[10], S43, 0xffeff47d);
	II (b, c, d, a, x[ 1], S44, 0x85845dd1);
	II (a, b, c, d, x[ 8], S41, 0x6fa87e4f);
	II (d, a, b, c, x[15], S42, 0xfe2ce6e0);
	II (c, d, a, b, x[ 6], S43, 0xa3014314);
	II (b, c, d, a, x[13], S44, 0x4e0811a1);
	II (a, b, c, d, x[ 4], S41, 0xf7537e82);
	II (d, a, b, c, x[11], S42, 0xbd3af235);
	II (c, d, a, b, x[ 2], S43, 0x2ad7d2bb);
	II (b, c, d, a, x[ 9], S44, 0xeb86d391);

	_state[0] += a;
	_state[1] += b;
	_state[2] += c;
	_state[3] += d;

  return S_OK;
}

// Encode input (Ulong) into output (Byte).
// Assume length is a multiple of 4.
RES_CODE MD5Caculator::encode(const Ulong* input, 
    Byte* output, size_t length) {
	for (size_t i = 0, j = 0; j < length; ++i, j += 4) {
		output[j] = (Byte)(input[i] & 0xff);
		output[j + 1] = (Byte)((input[i] >> 8) & 0xff);
		output[j + 2] = (Byte)((input[i] >> 16) & 0xff);
		output[j + 3] = (Byte)((input[i] >> 24) & 0xff);
	}

  return S_OK;
}

// Decodes input (Byte) into output (Ulong).
// Assumes length is a multiple of 4.
RES_CODE MD5Caculator::decode(const Byte* input, 
    Ulong* output, size_t length) {
	for (size_t i = 0, j = 0; j < length; ++i, j += 4) {	
		output[i] = ((Ulong)input[j]) | (((Ulong)input[j + 1]) << 8) | 
      (((Ulong)input[j + 2]) << 16) | (((Ulong)input[j + 3]) << 24);
	}

  return S_OK;
}

// Convert byte array to hex string.
RES_CODE MD5Caculator::bytes_to_hexstring(const Byte* input, 
    size_t length, String& str) {
  str = "";
	str.reserve(length << 1);

	for(size_t i = 0; i < length; ++i) {
		int t = input[i];
		int a = t / 16;
		int b = t % 16;
		str.append(1, HEX[a]);
		str.append(1, HEX[b]);
	}

	return S_OK;
}

// Convert digest to string value
RES_CODE MD5Caculator::to_string(String& str) {
	return bytes_to_hexstring(digest(), 16, str);
}

_END_MYJFM_NAMESPACE_


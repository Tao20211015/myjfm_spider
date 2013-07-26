#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <zlib.h>
#include <stdint.h>

typedef enum {
  HTTP_COMPRESS_GZIP, 
  HTTP_COMPRESS_DEFLATE, 
  HTTP_COMPRESS_COMPRESS, 
  HTTP_COMPRESS_IDENTITY 
} CompressMode;

#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

static unsigned gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */

char in_buffer[1024 * 1024 * 8];
bool dummy_initialized = false;

bool check_header(char* in, uint32_t in_len, char*& out, uint32_t& out_len) {
  typedef enum {
    GZIP_INIT = 0, 
    GZIP_OS, 
    GZIP_EXTRA0, 
    GZIP_EXTRA1, 
    GZIP_EXTRA2, 
    GZIP_ORIG, 
    GZIP_COMMENT, 
    GZIP_CRC 
  } Mode;

  char c;
  uint32_t i = 0;
  bool rs = true;
  Mode mode = GZIP_INIT;
  uint32_t skip_count = 0;
  uint32_t flags = 0;
  uint32_t len = 0;

  if (!in) {
    out = NULL;
    out_len = -1;
    return false;
  }

  while (in_len) {
    switch (mode) {
      case GZIP_INIT:
        c = in[i++];
        in_len--;

        if (skip_count == 0 && ((unsigned)c & 0377) != gz_magic[0]) {
          out = NULL;
          out_len = -1;
          rs = false;
          return rs;
        }

        if (skip_count == 1 && ((unsigned)c & 0377) != gz_magic[1]) {
          out = NULL;
          out_len = -1;
          rs = false;
          return rs;
        }

        if (skip_count == 2 && ((unsigned)c & 0377) != Z_DEFLATED) {
          out = NULL;
          out_len = -1;
          rs = false;
          return rs;
        }

        if (++skip_count == 4) {
          flags = (unsigned) c & 0377;
          if (flags & RESERVED) {
            out = NULL;
            out_len = -1;
            rs = false;
            return rs;
          }

          mode = GZIP_OS;
          skip_count = 0;
        }

        break;

      case GZIP_OS:
        c = in[i++];
        in_len--;
        skip_count++;

        if (++skip_count == 6) {
          mode = GZIP_EXTRA0;
        }

        break;

      case GZIP_EXTRA0:
        if (flags & EXTRA_FIELD) {
          c = in[i++];
          in_len--;
          len = (uInt) c & 0377;
          mode = GZIP_EXTRA1;
        } else {
          mode = GZIP_ORIG;
        }

        break;

      case GZIP_EXTRA1:
        c = in[i++];
        in_len--;
        len = ((uInt) c & 0377) << 8;
        skip_count = 0;
        mode = GZIP_EXTRA2;
        break;

      case GZIP_EXTRA2:
        if (skip_count == len) {
          mode = GZIP_ORIG;
        } else {
          c = in[i++];
          in_len--;
          skip_count++;
        }

        break;

      case GZIP_ORIG:
        if (flags & ORIG_NAME) {
          c = in[i++];
          in_len--;
          if (c == 0) {
            mode = GZIP_COMMENT;
          }
        } else {
          mode = GZIP_COMMENT;
        }

        break;

      case GZIP_COMMENT:
        if (flags & COMMENT) {
          c = in[i++];
          in_len--;

          if (c == 0) {
            mode = GZIP_CRC;
            skip_count = 0;
          }
        } else {
          mode = GZIP_CRC;
          skip_count = 0;
        }

        break;

      case GZIP_CRC:
        if (flags & HEAD_CRC) {
          c = in[i++];
          in_len--;
          if (++skip_count == 2) {
            out = in + i;
            out_len = in_len;
            return true;
          }
        } else {
          out = in + i;
          out_len = in_len;
          return true;
        }

        break;
    }
  }

  out = in + i;
  out_len = in_len;
  return true;
}

// return code:
// -1: error
// 0:  finish
// 1:  ok, but still need to decompress
int http_decompress(z_stream* stream, char* out, uint32_t& out_len) {
  if (stream == NULL || out_len == 0 || out == NULL) {
    out_len = 0;
    return -1;
  }
  
  stream->next_out = (Bytef*)out;
  stream->avail_out = (uInt)out_len;

  int code = inflate(stream, Z_NO_FLUSH);
  unsigned bytes_written = (uInt)out_len - stream->avail_out;

  if (code == Z_STREAM_END) {
    std::cout << 1 << std::endl;
    out_len = bytes_written;
    return 0;
  } else if (code == Z_OK || code == Z_BUF_ERROR) {
    std::cout << 2 << std::endl;
    out_len = bytes_written;
    return 1;
  } else if (code == Z_DATA_ERROR) {
    Bytef* tmp_addr = stream->next_in;
    uInt tmp_len = stream->avail_in;

    // some servers (notably Apache with mod_deflate) don't generate zlib headers
    // insert a dummy header and try again
    static char dummy_head[2] = {
      0x8 + 0x7 * 0x10,
      (((0x8 + 0x7 * 0x10) * 0x100 + 30) / 31 * 31) & 0xFF,
    };

    inflateReset(stream);
    stream->next_in = (Bytef*) dummy_head;
    stream->avail_in = sizeof(dummy_head);

    if (inflate(stream, Z_NO_FLUSH) != Z_OK) {
      out_len = 0;
      std::cout << 3 << std::endl;
      return -1;
    }

    // stop an endless loop caused by non-deflate data being labelled as deflate
    if (dummy_initialized) {
      out_len = -1;
      std::cout << 4 << std::endl;
      return -1;
    }

    dummy_initialized = true;
    stream->next_in = tmp_addr;
    stream->avail_in = tmp_len;
    out_len = 0;
    std::cout << 5 << std::endl;

    return 1;
  } else {
    out_len = -1;
    std::cout << 6 << std::endl;
    return -1;
  }
}

int main(int argc, const char *argv[]) {
  FILE* fd = NULL;
  if ((fd = fopen("index.html", "r")) == NULL) {
    std::cerr << "can not open file: index.html" << std::endl;
    return 1;
  }

  int read_len = -1;
  if ((read_len = fread(in_buffer, 1, 1024 * 1024 * 8, fd)) <= 0) {
    std::cerr << "can not read from file: index.html" << std::endl;
    return 1;
  }

  uint32_t in_len = (uint32_t)read_len;
  CompressMode mode = HTTP_COMPRESS_GZIP;
  char* in = in_buffer;
  char *out = new char[65540];
  uint32_t out_len = 65536;


  switch (mode) {
    case HTTP_COMPRESS_GZIP:
      /*
      if (!check_header(in_buffer, in_len, in, in_len)) {
        std::cerr << "can not check gzip header correctly" << std::endl;
        return 1;
      }
      */

    case HTTP_COMPRESS_DEFLATE:
      z_stream stream;
      memset(&stream, 0, sizeof(stream));

      if (mode == HTTP_COMPRESS_GZIP) {
        //if (inflateInit2(&stream, -MAX_WBITS) != Z_OK) {
        if (inflateInit2(&stream, 47) != Z_OK) {
          std::cerr << "can not inflateInit correctly" << std::endl;
          return 1;
        }
      } else {
        if (inflateInit(&stream) != Z_OK) {
          std::cerr << "can not inflateInit2 correctly" << std::endl;
          return 1;
        }
      }
      
      stream.next_in = (Bytef*)in;
      stream.avail_in = (uInt)in_len;

      while (1) {
        int status = http_decompress(&stream, out, out_len);
        switch (status) {
          case -1:
            std::cerr << "http_decompress error" << std::endl;
            return -1;

          case 0:
            out[out_len] = 0;
            std::cout << out << std::endl;
            if (inflateEnd(&stream) != Z_OK) {
              std::cerr << "inflateEnd error" << std::endl;
              return -1;
            }

            return 0;

          case 1:
            out[out_len] = 0;
            //std::cout << out << std::endl;
            out_len = 65536;
            fprintf(stderr, "%d\n", (int)((char*)stream.next_in - (char*)in));
            //std::cerr << stream.next_in << std::endl;
            break;

          default:
            std::cerr << "http_decompress error" << std::endl;
            return -1;
        }
      }

      return 1;

    default:
      std::cerr << "unknown CompressMode" << std::endl;
      return 1;
  }
}


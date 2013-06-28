#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>
#include <map>

#define _MYJFM_NAMESPACE_ myjfm
#define _START_MYJFM_NAMESPACE_ namespace myjfm {
#define _END_MYJFM_NAMESPACE_ }

#define Cerr std::cerr
#define Cout std::cout
#define Cin std::cin
#define Endl std::endl

#define String std::string
#define CHARS2STR(a) String(a)
#define String_size_t String::size_type
#define String_tail String::npos

#define Ifstream std::ifstream
#define Stream_size std::streamsize

#define Vector std::vector
#define Queue std::queue
#define Map std::map
#define Pair std::pair

enum IPTYPE {
  IP_DUMMY = 0x0, 
  IPv4 = 0x1, 
  IPv6 = 0x2 
};

enum Protocol {
  PRO_DUMMY = 0x0, 
  HTTP = 0x1, 
  FTP = 0x2 
};

enum RES_CODE {
  // all functions share these three statuses
  S_DUMMY = 0x0, 
  S_OK = 0x1, 
  S_FAIL = 0x2, 

  // Global member functions use these
  S_HAS_INIT = 0x4, 
  S_OUT_RANGE = 0x8, 

  // Hash member functions use  these
  S_EXIST = 0x4, 
  S_NOT_EXIST = 0x8, 

  // MD5caculator member functions use these
  S_STREAM_NOT_EXIST = 0x4, 
  S_NO_INPUT = 0x00040000, // Site member functions also use it

  // Site member functions use these
  S_NO_IP = 0x4, 
  S_BAD_IP_TYPE = 0x8, 
  S_HAS_SET = 0x10, 

  // Dnscache member functions use these
  S_NOT_FOUND = 0x4, 
  S_ALREADY_EXIST = 0x00040000, //Hash member functions also use it
  S_UNKNOWN_PROTOCOL = 0x00080000, //Site member functions also use it

  // Thread member functions use these
  S_HAS_STARTED = 0x4, 
  S_SET_ATTR_FAILED = 0x8, 
  S_THREAD_CREATE_FAILED = 0x00040000, //Threadpool member functions also use it
  S_NOT_RUNNING = 0x20, 
  S_JOIN_FAILED = 0x40, 
  S_CANCEL_FAILED = 0x80, 
  S_UNKOWN_STATE = 0x100, 

  // Threadpool member functions use these
  S_NOT_CONSTRUCTED = 0x4, 
  S_BAD_ARG = 0x8, 

  // Url member functions use these
  S_INVALID_URL = 0x4
};

#endif


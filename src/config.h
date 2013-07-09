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

#define Ofstream std::ofstream

#define MAX_LOG_LEN 1024

enum LOG_LEVEL {
  LOG_DUMMY = 0x0, 
  INFO = 0x1, 
  ERROR = 0x2, 
  WARNING = 0x4, 
  FATAL = 0x8
};

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

  // MD5Caculator member functions use these
  S_STREAM_NOT_EXIST = 0x4, 
  S_NO_INPUT = 0x00040000, // Site member functions also use it

  // Site member functions use these
  S_NO_IP = 0x4, 
  S_BAD_IP_TYPE = 0x8, 
  S_HAS_SET = 0x10, 

  // DnsCache member functions use these
  S_NOT_FOUND = 0x4, 
  S_ALREADY_EXIST = 0x00040000, //Hash member functions also use it
  S_UNKNOWN_PROTOCOL = 0x00080000, //Site member functions also use it

  // Thread member functions use these
  S_HAS_STARTED = 0x4, 
  S_SET_ATTR_FAILED = 0x8, 
  S_THREAD_CREATE_FAILED = 0x00040000, //ThreadPool member functions also use it
  S_NOT_RUNNING = 0x20, 
  S_JOIN_FAILED = 0x40, 
  S_CANCEL_FAILED = 0x80, 
  S_UNKOWN_STATE = 0x100, 

  // ThreadPool member functions use these
  S_NOT_CONSTRUCTED = 0x4, 
  // LoggerTask and Global member functions also use these
  S_BAD_ARG = 0x00040000, 

  // Url member functions use these
  S_INVALID_URL = 0x4, 

  //Logger member functions use these
  S_INVALID_LOG = 0x4, 
  S_IS_LOGGING = 0x8, 
  S_FAILED_TO_LOG_ON_DISK = 0x10, 

  // EventLoop member functions use these
  S_EPOLL_CREATE_FAILED = 0x4
};

#endif


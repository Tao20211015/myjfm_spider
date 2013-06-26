#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <iostream>

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

#endif


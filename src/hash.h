#ifndef _HASH_H_
#define _HASH_H_

#include "config.h"
#include "global.h"
#include "shared.h"

_START_MYJFM_NAMESPACE_

typedef unsigned long hash_size_type;

// the pure virtual functor class
template <class T>
class Hashfunction {
public:
  virtual hash_size_type operator()(T&) = 0;
};

class Charphashfunction : Hashfunction<char*> {
public:
  hash_size_type operator()(char*);
};

class Stringhashfunction : Hashfunction<String> {
public:
  hash_size_type operator()(String&);
};

class Charhashfunction : Hashfunction<char> {
public:
  hash_size_type operator()(char&);
};

class Ucharhashfunction : Hashfunction<unsigned char> {
public:
  hash_size_type operator()(unsigned char&);
};

class Shorthashfunction : Hashfunction<short> {
public:
  hash_size_type operator()(short&);
};

class Ushorthashfunction : Hashfunction<unsigned short> {
public:
  hash_size_type operator()(unsigned short&);
};

class Inthashfunction : Hashfunction<int> {
public:
  hash_size_type operator()(int&);
};

class Uinthashfunction : Hashfunction<unsigned int> {
public:
  hash_size_type operator()(unsigned int&);
};

class Longhashfunction : Hashfunction<long> {
public:
  hash_size_type operator()(long&);
};

class Ulonghashfunction : Hashfunction<unsigned long> {
public:
  hash_size_type operator()(unsigned long&);
};

template <class T>
class _hash_list_node {
public:
  _hash_list_node(const T& value) {
    _next = NULL;
    // copy constructor
    _value = new T(value);
  }

  ~_hash_list_node() {
    _next = NULL;
    delete _value;
  }

  _hash_list_node *_next;
  T* _value;
};

// the class T should has overloaded the operator==
// and
// should also have the copy constructor
// and
// the functor class Hashfunc should has overloaded the operator()
template <class T, class Hashfunc>
class Hash : public Shared {
  typedef _hash_list_node<T> Hashnode;
  typedef Vector<Hashnode*> Buckets;

public:
  Hash(hash_size_type n);
  ~Hash();
  int insert(T&);
  int is_exist(T&);

private:
  Hashfunc _hasher;
  hash_size_type get_key(T&);
  hash_size_type get_key(hash_size_type);
  hash_size_type bucket_num(T&);
  Buckets _buckets;
  hash_size_type _buckets_size;
};

_END_MYJFM_NAMESPACE_

#endif


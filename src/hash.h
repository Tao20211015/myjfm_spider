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
  virtual ~Hashfunction() {}
};

class Charphashfunction : Hashfunction<char*> {
public:
  virtual hash_size_type operator()(char*);
  virtual ~Charphashfunction() {}
};

class Stringhashfunction : Hashfunction<String> {
public:
  virtual hash_size_type operator()(String&);
  virtual ~Stringhashfunction() {}
};

class Charhashfunction : Hashfunction<char> {
public:
  virtual hash_size_type operator()(char&);
  virtual ~Charhashfunction() {}
};

class Ucharhashfunction : Hashfunction<unsigned char> {
public:
  virtual hash_size_type operator()(unsigned char&);
  virtual ~Ucharhashfunction() {}
};

class Shorthashfunction : Hashfunction<short> {
public:
  virtual hash_size_type operator()(short&);
  virtual ~Shorthashfunction() {}
};

class Ushorthashfunction : Hashfunction<unsigned short> {
public:
  virtual hash_size_type operator()(unsigned short&);
  virtual ~Ushorthashfunction() {}
};

class Inthashfunction : Hashfunction<int> {
public:
  virtual hash_size_type operator()(int&);
  virtual ~Inthashfunction() {}
};

class Uinthashfunction : Hashfunction<unsigned int> {
public:
  virtual hash_size_type operator()(unsigned int&);
  virtual ~Uinthashfunction() {}
};

class Longhashfunction : Hashfunction<long> {
public:
  virtual hash_size_type operator()(long&);
  virtual ~Longhashfunction() {}
};

class Ulonghashfunction : Hashfunction<unsigned long> {
public:
  virtual hash_size_type operator()(unsigned long&);
  virtual ~Ulonghashfunction() {}
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
//class Hash : public Shared {
class Hash {
  typedef _hash_list_node<T> Hashnode;
  typedef Vector<Hashnode*> Buckets;

public:
  Hash(hash_size_type n);
  ~Hash();
  hash_size_type size();
  int insert(T& key);
  int is_exist(T& key);

private:
  Hashfunc _hasher;
  hash_size_type get_key(T& value);
  hash_size_type get_key(hash_size_type);
  hash_size_type bucket_num(T& value);
  Buckets _buckets;
  hash_size_type _buckets_size;
};

_END_MYJFM_NAMESPACE_

#endif


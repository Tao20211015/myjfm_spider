#include "config.h"
#include "global.h"
#include "hash.h"

_START_MYJFM_NAMESPACE_

static const int _num_primes = 28;
static const unsigned long  _prime_list[_num_primes] = {
  53,         97,           193,          389,        769, 
  1543,       3079,         6151,         12289,      24593, 
  49157,      98317,        196613,       393241,     786433, 
  1572869,    3145739,      6291469,      12582917,   25165843, 
  50331653,   100663319,    201326611,    402653189,  805306457, 
  1610612741, 3221225473ul, 4294967291ul
};

hash_size_type Charphashfunction::operator()(char* s) {
  if (!s) {
    return 0;
  }

  hash_size_type h = 0;
  while (*s) {
    h = 5 * h + *s++;
  }

  return h;
}

hash_size_type Stringhashfunction::operator()(String& s) {
  hash_size_type h = 0;
  int i = 0;
  for (; i < s.length(); ++i) {
    h = 5 * h + s[i];
  }
  return h;
}

hash_size_type Charhashfunction::operator()(char& s) {
  return hash_size_type(s);
}

hash_size_type Ucharhashfunction::operator()(unsigned char& s) {
  return hash_size_type(s);
}

hash_size_type Shorthashfunction::operator()(short& s) {
  return hash_size_type(s);
}

hash_size_type Ushorthashfunction::operator()(unsigned short& s) {
  return hash_size_type(s);
}

hash_size_type Inthashfunction::operator()(int& s) {
  return hash_size_type(s);
}

hash_size_type Uinthashfunction::operator()(unsigned int& s) {
  return hash_size_type(s);
}

hash_size_type Longhashfunction::operator()(long& s) {
  return hash_size_type(s);
}

hash_size_type Ulonghashfunction::operator()(unsigned long& s) {
  return hash_size_type(s);
}

template <class T, class Hashfunc>
Hash<T, Hashfunc>::Hash(unsigned long n) {
  int i = 0;
  for (i = 0; i < _num_primes; ++i) {
    if (_prime_list[i] > n) {
      _buckets_size = _prime_list[i];
      break;
    }
  }

  if (i == _num_primes) {
    _buckets_size = _prime_list[_num_primes - 1];
  }

  _buckets.resize(_buckets_size, (Hashnode*)0);
}

template <class T, class Hashfunc>
Hash<T, Hashfunc>::~Hash() {
  int i;
  for (i = 0; i < _buckets_size; ++i) {
    Hashnode* p = _buckets[i];
    while (p) {
      Hashnode* q = p;
      p = p->next;
      delete q;
    }
    _buckets[i] = NULL;
  }
  _buckets.clear();
  _buckets_size = 0;
  delete _hasher;
}

template <class T, class Hashfunc>
int Hash<T, Hashfunc>::insert(T& value) {
  hash_size_type key = get_key(value);
  if (_buckets[key] == NULL) {
    _buckets[key] = new Hashnode(value);
    return 0;
  }

  Hashnode* nodep = _buckets[key];

  while (nodep) {
    if (nodep->_value == value) {
      return 1;
    }
    nodep = nodep->_next;
  }

  nodep = new Hashnode(value);
  nodep->_next = _buckets[key]->_next;
  _buckets[key] = nodep;
  return 0;
}

template <class T, class Hashfunc>
int Hash<T, Hashfunc>::is_exist(T& value) {
  hash_size_type key = get_key(value);
  if (_buckets[key] == NULL) {
    return 0;
  }

  Hashnode* nodep = _buckets[key];

  while (nodep) {
    if (nodep->_value == value) {
      return 1;
    }
    nodep = nodep->_next;
  }

  return 0;
}

template <class T, class Hashfunc>
hash_size_type Hash<T, Hashfunc>::get_key(T& value) {
  return get_key(bucket_num(value));
}

template <class T, class Hashfunc>
hash_size_type Hash<T, Hashfunc>::get_key(hash_size_type bn) {
  return bn % _buckets_size;
}

template <class T, class Hashfunc>
hash_size_type Hash<T, Hashfunc>::bucket_num(T& value) {
  return _hasher(value);
}

_END_MYJFM_NAMESPACE_


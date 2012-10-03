/*
  Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

  This software is provided 'as-is', without any express or implied
  warranty. In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
  claim that you wrote the original software. If you use this software
  in a product, an acknowledgment in the product documentation would be
  appreciated but is not required.

  2. Altered source versions must be plainly marked as such, and must not be
  misrepresented as being the original software.

  3. This notice may not be removed or altered from any source
  distribution.
*/

#pragma once

#include "libpal/pal_platform.h"
#include "libpal/pal_string.h"
#include "libpal/pal_types.h"

template<class Key> struct palHashEqual {
  bool operator()(const Key& x, const Key& y) const {
    return x == y;
  }
};

template<class Key> struct palHashEqualDereference
{
  bool operator()(const Key& x, const Key& y) const
  {
    return *x == *y;
  }
};


PAL_INLINE uint32_t rotl32( uint32_t x, int8_t r ) {
  return (x << r) | (x >> (32 - r));
}

PAL_INLINE uint64_t rotl64( uint64_t x, int8_t r ) {
  return (x << r) | (x >> (64 - r));
}

PAL_INLINE uint32_t rotr32( uint32_t x, int8_t r ) {
  return (x >> r) | (x << (32 - r));
}

PAL_INLINE uint64_t rotr64( uint64_t x, int8_t r ) {
  return (x >> r) | (x << (64 - r));
}

#define	ROTL32(x,y)	rotl32(x,y)
#define ROTL64(x,y)	rotl64(x,y)
#define	ROTR32(x,y)	rotr32(x,y)
#define ROTR64(x,y)	rotr64(x,y)

/* We use MurmurHash3 by Austin Appleby */
PAL_INLINE uint32_t mh3_getblock(const uint32_t * p, ssize_t i) {
  return p[i];
}

PAL_INLINE void mh3_bmix32( uint32_t & h1, uint32_t & k1, uint32_t & c1, uint32_t & c2 ) {
  c1 = c1*5+0x7b7d159c;
  c2 = c2*5+0x6bce6396;

  k1 *= c1; 
  k1 = ROTL32(k1,11); 
  k1 *= c2;

  h1 = ROTL32(h1,13);
  h1 = h1*5+0x52dce729;
  h1 ^= k1;
}

//----------

PAL_INLINE unsigned int palMurmurHashSeed(const void * key, size_t len, uint32_t seed)
{
  uint32_t out;
  const uint8_t * data = (const uint8_t*)key;
  const ssize_t nblocks = len / 4;

  uint32_t h1 = 0x971e137b ^ seed;

  uint32_t c1 = 0x95543787;
  uint32_t c2 = 0x2ad7eb25;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(ssize_t i = -nblocks; i; i++)
  {
    uint32_t k1 = mh3_getblock(blocks,i);

    mh3_bmix32(h1,k1,c1,c2);
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
    mh3_bmix32(h1,k1,c1,c2);
  };

  //----------
  // finalization

  h1 ^= len;

  h1 *= 0x85ebca6b;
  h1 ^= h1 >> 13;
  h1 *= 0xc2b2ae35;
  h1 ^= h1 >> 16;

  h1 ^= seed;

  out = h1;
  return out;
}

PAL_INLINE unsigned int palMurmurHash(const void* key, size_t len) {
  return palMurmurHashSeed(key, len, 0xc58f1a7b);
}

// base palHashFunction template
template<typename T>  struct palHashFunction
{
  unsigned int operator()(const T& key) const;
};

/* Standard types uint64_t, uint32_t, const void* and const char* implementations */
template<>
struct palHashFunction<uint64_t>
{
  unsigned int operator()(const uint64_t& key) const
  {
    return palMurmurHash(reinterpret_cast<const void*>(&key), sizeof(uint64_t));
  }
};

template<> 
struct palHashFunction<uint32_t>
{
  unsigned int operator()(const uint32_t& key) const
  {
    return palMurmurHash(reinterpret_cast<const void*>(&key), sizeof(uint32_t));
  }
};

template<> 
struct palHashFunction<int>
{
  unsigned int operator()(const int& key) const
  {
    return palMurmurHash(reinterpret_cast<const void*>(&key), sizeof(int));
  }
};

/* conflicts with uint32_t/uint64_t hash functions
template<>
struct palHashFunction<uintptr_t> {
  unsigned int operator()(const uintptr_t& key) const {
    return palMurmurHash(reinterpret_cast<void*>(&key), sizeof(uintptr_t));
  }
};
*/

template<>
struct palHashFunction<const void*> {
  unsigned int operator()(const void* const& key) const {
    return palMurmurHash(key, sizeof(void*));
  }
};

template<>
struct palHashFunction<const char*>
{
  unsigned int operator()(const char* const& key) const
  {
    return palMurmurHash(key, palStringLength(key));
  }
};

template<>
struct palHashFunction<palDynamicString> {
  unsigned int operator()(const palDynamicString& str) {
    return palMurmurHash(str.C(), str.GetLength());
  }
};


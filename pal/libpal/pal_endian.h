/*
  Copyright (c) 2010 John McCutchan <john@johnmccutchan.com>

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


#ifndef LIBPAL_PAL_ENDIAN_H_
#define LIBPAL_PAL_ENDIAN_H_

template<int SIZE>
void palEndianByteSwap(const char* src, char* dest);

template<>
inline void palEndianByteSwap<2>(const char* src, char* dest) {
  unsigned short* short_dest = reinterpret_cast<unsigned short*>(dest);
  const unsigned short* short_src = reinterpret_cast<const unsigned short*>(src);
  *short_dest = (*short_src >> 8) | (*short_src << 8);
}

template<>
inline void palEndianByteSwap<4>(const char* src, char* dest) {
  unsigned int* int_dest = reinterpret_cast<unsigned int*>(dest);
  const unsigned int* int_src = reinterpret_cast<const unsigned int*>(src);
  *int_dest = (*int_src >> 24) | ((*int_src & 0x00ff0000) >> 8) |
          ((*int_src & 0x0000ff00) << 8) | (*int_src << 24);
}

template<>
inline void palEndianByteSwap<8>(const char* src, char* dest) {
  uint64_t* int64_dest = reinterpret_cast<uint64_t*>(dest);
  const uint64_t* int64_src = reinterpret_cast<const uint64_t*>(src);

  *int64_dest = (((uint64_t)(*int64_src) << 56) |
    (((uint64_t)(*int64_src) << 40) & 0xff000000000000ULL) |
    (((uint64_t)(*int64_src) << 24) & 0xff0000000000ULL) |
    (((uint64_t)(*int64_src) << 8)  & 0xff00000000ULL) |
    (((uint64_t)(*int64_src) >> 8)  & 0xff000000ULL) |
    (((uint64_t)(*int64_src) >> 24) & 0xff0000ULL) |
    (((uint64_t)(*int64_src) >> 40) & 0xff00ULL) |
    ((uint64_t)(*int64_src)  >> 56));
#if 0
  *int64_dest = (((*int64_src >>  0) & 0xff) << 56) | 
          (((*int64_src >>  8) & 0xff) << 48) | 
          (((*int64_src >> 16) & 0xff) << 40) | 
          (((*int64_src >> 24) & 0xff) << 32) |
          (((*int64_src >> 32) & 0xff) << 24) | 
          (((*int64_src >> 40) & 0xff) << 16) | 
          (((*int64_src >> 48) & 0xff) <<  8) | 
          (((*int64_src >> 56) & 0xff) <<  0);
#endif
}

inline float palEndianByteSwap(float input) {
  float r;
  palEndianByteSwap<4>(reinterpret_cast<const char*>(&input), reinterpret_cast<char*>(&r));
  return r;
}

inline uint16_t palEndianByteSwap(uint16_t input) {
  uint16_t r;
  palEndianByteSwap<2>(reinterpret_cast<const char*>(&input), reinterpret_cast<char*>(&r));
  return r;
}

inline short palEndianByteSwap(short input) {
  short r;
  palEndianByteSwap<2>(reinterpret_cast<const char*>(&input), reinterpret_cast<char*>(&r));
  return r;
}

inline int palEndianByteSwap(int input) {
  int r;
  palEndianByteSwap<4>(reinterpret_cast<const char*>(&input), reinterpret_cast<char*>(&r));
  return r;
}

inline uint32_t palEndianByteSwap(uint32_t input) {
  uint32_t r;
  palEndianByteSwap<4>(reinterpret_cast<const char*>(&input), reinterpret_cast<char*>(&r));
  return r;
}

inline uint64_t palEndianByteSwap(uint64_t input) {
  uint64_t r;
  palEndianByteSwap<8>(reinterpret_cast<const char*>(&input), reinterpret_cast<char*>(&r));
  return r;
}


#endif  // LIBPAL_PAL_ENDIAN_H_
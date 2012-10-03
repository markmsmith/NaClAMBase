/*
  Copyright (c) 2012 John McCutchan <john@johnmccutchan.com>

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

#include "libpal/pal_random.h"
#include "libpal/pal_timer.h"

// Knuth's subtractive random number generator
// See: Art of Computer Programming

palRandom::palRandom(uint32_t seed) {
  Reset(seed);
}

palRandom::palRandom() {
  Reset();
}

void palRandom::Reset(uint32_t seed) {
  uint32_t k = 1;
  _table[54] = seed;
  for (uint32_t i = 0; i < 54; i++) {
    uint32_t ii = (21 * (i + 1) % 55) - 1;
    _table[ii] = k;
    k = seed - k;
    seed = _table[ii];
  }
  for (uint32_t loop = 0; loop < 4; loop++) {
    for (uint32_t i = 0; i < 55; i++)
      _table[i] = _table[i] - _table[(1 + i + 30) % 55];
  }
  _index1 = 0;
  _index2 = 31;
}

void palRandom::Reset() {
  //Reset((uint32_t)palTimerGetTicks());
  Reset((uint32_t)0xdeadbeef);
}

uint32_t palRandom::Random() {
  _index1 = (_index1+1) % 55;
  _index2 = (_index2+1) % 55;
  _table[_index1] = _table[_index1] - _table[_index2];
  return _table[_index1];
}

uint32_t palRandom::RandomLessThan(uint32_t limit) {
  uint32_t r = 0;
  do {
    r = Random();
  } while(r > limit);
  return r;
}

uint32_t palRandom::RandomWithin(uint32_t lo, uint32_t hi) {
  uint32_t r = 0;
  do {
    r = Random();
  } while(r <= lo || r > hi);
  return r;
}

float palRandom::RandomFloat() {
  return (float)RandomDouble();
}

double palRandom::RandomDouble() {
   return double(Random()) * (1.0/4294967295.0);
}

palRandom g_RandomNumberGenerator;

float palGenerateRandomFloat() {
  return g_RandomNumberGenerator.RandomFloat();
}

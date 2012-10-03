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

#pragma once

#include "libpal/pal_types.h"

class palRandom {
  uint32_t _table[55];
  uint32_t _index1;
  uint32_t _index2;
public:
  palRandom(uint32_t seed);
  palRandom();

  void Reset(uint32_t seed);
  void Reset();

  uint32_t Random();
  // Between [0,limit)
  uint32_t RandomLessThan(uint32_t limit);
  // Between [lo, hi)
  uint32_t RandomWithin(uint32_t lo, uint32_t hi);

  // Between [0.0,1.0]
  float RandomFloat();

  // Between [0.0,1.0]
  double RandomDouble();
};

extern palRandom g_RandomNumberGenerator;
float palGenerateRandomFloat();
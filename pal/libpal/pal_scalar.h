/*
	Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_SCALAR_H__
#define LIBPAL_PAL_SCALAR_H__

#include "libpal/pal_platform.h"

class palScalar {
public:
  static float Cos(float x);
  static float Sin(float x);
  static float Tan(float x);
  static float ACos(float x);
  static float ASin(float x);
  static float ATan(float x);
  static float ATan2(float y, float x);

  // base e
  static float Ln(float x);
  // base 10
  static float Log(float x);
  static float Exp(float x);
  static float Pow(float x, float y);

  static float Sqrt(float x);

  static float RoundUp(float x);
  static float RoundDown(float x);
  static float RoundNearest(float x);

  static float Absolute(float x);
  // x < 0 return -1.0f;
  // x == 0 return 0.0f;
  // x > 0 return 1.0f;
  static float Sign(float x);
  static const float kEpsilon;
  static const float kMax;
  static const float kMin;
  static const float kE;
  static const float kPi;
  static const float kSqrtOneHalf;
  static const float kHalfPi;
  static const float kRadiansPerDegree;
  static const float kDegreesPerRadian;
};

#endif  // LIBPAL_PAL_SCALAR_H__


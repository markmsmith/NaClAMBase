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

#include "libpal/pal_scalar.h"
#include <math.h>
#include <float.h>

float palScalar::Cos(float x) {
  return cosf(x);
}

float palScalar::Sin(float x) {
  return sinf(x);
}

float palScalar::Tan(float x) {
  return tanf(x);
}

float palScalar::ACos(float x) {
  return acosf(x);
}

float palScalar::ASin(float x) {
  return asinf(x);
}

float palScalar::ATan(float x) {
  return atanf(x);
}
float palScalar::ATan2(float y, float x) {
  return atan2f(y, x);
}

float palScalar::Ln(float x) {
  return logf(x);
}

float palScalar::Log(float x) {
  return log10f(x);
}

float palScalar::Exp(float x) {
  return expf(x);
}

float palScalar::Pow(float x, float y) {
  return powf(x, y);
}

float palScalar::Sqrt(float x) {
  return sqrtf(x);
}

float palScalar::RoundUp(float x) {
  return ceilf(x);
}
float palScalar::RoundDown(float x) {
  return floorf(x);
}

float palScalar::RoundNearest(float x) {
  return floorf(x + 0.5f);
}

float palScalar::Absolute(float x) {
  return fabsf(x);
}

float palScalar::Sign(float x) {
  if (x < 0)
    return -1.0f;
  if (x > 0)
    return 1.0f;
  return 0.0f;
}

const float kEpsilon = FLT_EPSILON;
const float kMax = FLT_MAX;
const float kMin = FLT_MIN;
const float palScalar::kE = 2.71828183f;
const float palScalar::kPi = 3.14159265f;
const float palScalar::kSqrtOneHalf = 0.707106781f;
const float palScalar::kHalfPi = 1.57079633f;
const float palScalar::kRadiansPerDegree = 0.0174532925f;
const float palScalar::kDegreesPerRadian = 57.2957795f;
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

#ifndef LIBPAL_PAL_SIMD_H_
#define LIBPAL_PAL_SIMD_H_

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"

#include "libpal/pal_simd_types-inl.h"

extern const palSimd palSimd_UNIT_X;
extern const palSimd palSimd_UNIT_Y;
extern const palSimd palSimd_UNIT_Z;
extern const palSimd palSimd_UNIT_W;
extern const palSimd palSimd_ONE;

PAL_INLINE palSimd palSimdSplat(float f);
PAL_INLINE palSimd palSimdSplat(int i);
PAL_INLINE palSimd palSimdReverse(palSimd a);
PAL_INLINE float* palSimdGetArray(palSimd* a);
PAL_INLINE const float* palSimdGetConstArray(const palSimd* a);

PAL_INLINE float palSimdGetX(palSimd a);
PAL_INLINE float palSimdGetY(palSimd a);
PAL_INLINE float palSimdGetZ(palSimd a);
PAL_INLINE float palSimdGetW(palSimd a);

PAL_INLINE palSimd palSimdLoadAligned(const float* ptr);
PAL_INLINE palSimd palSimdLoadUnaligned(const float* ptr);
PAL_INLINE void palSimdStoreAligned(palSimd a, float* ptr);
PAL_INLINE void palSimdStoreUnaligned(palSimd a, float* ptr);

PAL_INLINE palSimd palSimdSetXYZ0(float x, float y, float z);
PAL_INLINE palSimd palSimdSetXYZW(float x, float y, float z, float w);

PAL_INLINE palSimd palSimdAND(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdOR(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdXOR(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdANDNOT(palSimd a, palSimd b);
// performs a bitwise negation aka return ~a;
PAL_INLINE palSimd palSimdNEG(palSimd a);


PAL_INLINE palSimd palSimdZero();

PAL_INLINE palSimd palSimdRoundDown(palSimd a);
PAL_INLINE palSimd palSimdRoundUp(palSimd a);
PAL_INLINE palSimd palSimdRoundNearest(palSimd a);

PAL_INLINE palSimd palSimdCompareLT(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdCompareLTE(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdCompareE(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdCompareNE(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdCompareGTE(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdCompareGT(palSimd a, palSimd b);
/* result[i] = x[i] >= lo[i] && x[i] <= hi[i] */
PAL_INLINE palSimd palSimdCompareInRangeInclusive(palSimd x, palSimd lo, palSimd hi);
/* result[i] = x[i] > lo[i] && x[i] < hi[i] */
PAL_INLINE palSimd palSimdCompareInRangeExclusive(palSimd x, palSimd lo, palSimd hi);
PAL_INLINE palSimd palSimdSignMask(palSimd a);
PAL_INLINE bool palSimdCompareMaskAny(palSimd compare_mask);
PAL_INLINE bool palSimdCompareMaskAll(palSimd compare_mask);
PAL_INLINE bool palSimdCompareMaskNone(palSimd compare_mask);
PAL_INLINE palSimd palSimdMakeMask(uint32_t maskX, uint32_t maskY, uint32_t maskZ, uint32_t maskW);

// returns a vector construct from the upper portions of a and b
// that is,
// [0] = b[2]
// [1] = b[3]
// [2] = a[2]
// [3] = a[3]
PAL_INLINE palSimd palSimdSelectUpper(palSimd a, palSimd b);

// returns a vector construct from the lower portions of a and b
// that is,
// [0] = a[0]
// [1] = a[1]
// [2] = b[0]
// [3] = b[1]
PAL_INLINE palSimd palSimdSelectLower(palSimd a, palSimd b);

// return[i] = if (compare_mask[i]) true_value[i] else false_value[i];
PAL_INLINE palSimd palSimdSelect(palSimd compare_mask, palSimd true_value, palSimd false_value);

PAL_INLINE palSimd palSimdSetX(palSimd a, float x);
PAL_INLINE palSimd palSimdSetY(palSimd a, float y);
PAL_INLINE palSimd palSimdSetZ(palSimd a, float z);
PAL_INLINE palSimd palSimdSetW(palSimd a, float w);

PAL_INLINE float palSimdGetIndex(palSimd a, int index);
PAL_INLINE palSimd palSimdSetIndex(palSimd a, int index, float v);

PAL_INLINE palSimd palSimdAdd(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdSub(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdMul(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdReciprocal(palSimd a);
PAL_INLINE palSimd palSimdDiv(palSimd a, palSimd b);


PAL_INLINE palSimd palSimdSqrt(palSimd a);
PAL_INLINE palSimd palSimdReciprocalSqrt(palSimd a);

PAL_INLINE palSimd palSimdMin(palSimd a, palSimd b);
PAL_INLINE palSimd palSimdMax(palSimd a, palSimd b);

PAL_INLINE palSimd palSimdClamp(palSimd min, palSimd max, palSimd a);
PAL_INLINE palSimd palSimdAbsolute(palSimd a);
PAL_INLINE palSimd palSimdNegative(palSimd a);

/* 4D dot product */
PAL_INLINE palSimd palSimdDot(palSimd a, palSimd b);
/* 3D dot product */
PAL_INLINE palSimd palSimdDot3(palSimd a, palSimd b);

PAL_INLINE palSimd palSimdCross(palSimd a, palSimd b);

PAL_INLINE palSimd palSimdSin(palSimd x);
PAL_INLINE palSimd palSimdCos(palSimd x);
PAL_INLINE palSimd palSimdTan(palSimd x);
PAL_INLINE palSimd palSimdAcos(palSimd x);
PAL_INLINE palSimd palSimdAsin(palSimd x);
PAL_INLINE palSimd palSimdAtan(palSimd x);
PAL_INLINE palSimd palSimdAtan2(palSimd y, palSimd x);

PAL_INLINE palSimd palSimdPow(palSimd x);
PAL_INLINE palSimd palSimdLn(palSimd x);
PAL_INLINE palSimd palSimdLog(palSimd x);
PAL_INLINE palSimd palSimdExp(palSimd x);

PAL_INLINE palSimd palSimdLength2(palSimd a);
PAL_INLINE palSimd palSimdLength(palSimd a);

PAL_INLINE void palSimdMatrixTranspose(palSimd* a, palSimd* b, palSimd* c, palSimd* d);

// include implementation
#include "libpal/pal_simd_impl-inl.h"

#endif

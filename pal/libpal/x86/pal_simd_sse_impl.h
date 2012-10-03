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

/* SSE Implementations of sin, cos, exp, atan and atan2 based on the
 * UT-SSE project: http://code.google.com/p/ut-sse. MIT license:
 Copyright (c) 2009 Peter Djeu, Michael Quinlan, and Peter Stone

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 */

 /* SSE implementations of log based on the 'Simple SSE and SSE2 optimized sin, cos, log and exp' 
  * project: http://gruntthepeon.free.fr/ssemath/
  * License:

  SIMD (SSE1+MMX or SSE2) implementation of sin, cos, exp and log

   Inspired by Intel Approximate Math library, and based on the
   corresponding algorithms of the cephes math library

   The default is to use the SSE1 version. If you define USE_SSE2 the
   the SSE2 intrinsics will be used in place of the MMX intrinsics. Do
   not expect any significant performance improvement with SSE2.


 Copyright (C) 2007  Julien Pommier

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
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
  3. This notice may not be removed or altered from any source distribution.

  (this is the zlib license)
*/

#ifndef LIBPAL_PAL_SIMD_SSE_H_
#define LIBPAL_PAL_SIMD_SSE_H_

#include "libpal/pal_debug.h"

#include "libpal/pal_scalar.h"
// OR: define TRUE_SIMD_TRIG

#define palSimdBroadcast(a, component) _mm_shuffle_ps(a, a, _MM_SHUFFLE(component, component, component, component))

/* W Z Y X -> X W Z Y */
#define palSimdRotateRight(a, i) _mm_shuffle_ps(a, a, _MM_SHUFFLE((unsigned char)(i+3)%4,(unsigned char)(i+2)%4,(unsigned char)(i+1)%4,(unsigned char)(i+0)%4))
/* W Z Y X -> Z Y X W */
#define palSimdRotateLeft(a, i) _mm_shuffle_ps(a, a, _MM_SHUFFLE((unsigned char)(7-i)%4,(unsigned char)(6-i)%4,(unsigned char)(5-i)%4,(unsigned char)(4-i)%4))

/* Returns the vector a swizzled:
 *
 * r[0] = a[x];
 * r[1] = a[y];
 * r[2] = a[z];
 * r[3] = a[w];
 * 0 <= x,y,z,w <= 3.
 */
#define palSimdSwizzle(a, x, y, z, w) _mm_shuffle_ps(a, a, _MM_SHUFFLE(w, z, y, x))

/* Returns the vector a swizzled:
 *
 * r[0] = a[w];
 * r[1] = a[z];
 * r[2] = a[y];
 * r[3] = a[x];
 * 0 <= x,y,z,w <= 3.
 */
#define palSimdSwizzleR(a, w, y, z, x) _mm_shuffle_ps(a, a, _MM_SHUFFLE(w, z, y, x))

PAL_INLINE palSimd palSimdSplat(float f)
{
  palSimd s;
  s = _mm_set_ps1(f);
  return s;
}

PAL_INLINE palSimd palSimdSplat(int i) {
  palSimd s;
  s = _mm_castsi128_ps(_mm_set1_epi32(i));
  return s;
}


PAL_INLINE palSimd palSimdReverse(palSimd a) {
  // reverses components in vector
  // X|Y|Z|W -> W|Z|Y|X
  a = _mm_shuffle_ps(a, a, 0x1B);
  return a;
}

PAL_INLINE float* palSimdGetArray(palSimd* a) {
  return reinterpret_cast<float*>(a);
}

PAL_INLINE const float* palSimdGetConstArray(const palSimd* a) {
    return reinterpret_cast<const float*>(a);
}

PAL_INLINE float palSimdGetX(palSimd a) {
  return reinterpret_cast<float*>(&a)[0];
}

PAL_INLINE float palSimdGetY(palSimd a) {
  return reinterpret_cast<float*>(&a)[1];
}

PAL_INLINE float palSimdGetZ(palSimd a) {
  return reinterpret_cast<float*>(&a)[2];
}

PAL_INLINE float palSimdGetW(palSimd a) {
  return reinterpret_cast<float*>(&a)[3];
}


PAL_INLINE palSimd palSimdLoadAligned(const float* ptr) {
  palSimd a;
  a = _mm_load_ps(ptr);
  return a;
}

PAL_INLINE palSimd palSimdLoadUnaligned(const float* ptr) {
  palSimd a;
  a = _mm_loadu_ps(ptr);
  return a;
}

PAL_INLINE void palSimdStoreAligned(palSimd a, float* ptr) {
  _mm_store_ps(ptr, a);
}

PAL_INLINE void palSimdStoreUnaligned(palSimd a, float* ptr) {
  _mm_storeu_ps(ptr, a);
}

PAL_INLINE palSimd palSimdSetXYZ0(float x, float y, float z) {
  palSimd a;
  a = _mm_set_ps(0.0, z, y, x);
  return a;
}

PAL_INLINE palSimd palSimdSetXYZW(float x, float y, float z, float w) {
  palSimd a;
  a = _mm_set_ps(w, z, y, x);
  return a;
}

PAL_INLINE palSimd palSimdAND(palSimd a, palSimd b) {
  return _mm_and_ps(a, b);
}

PAL_INLINE palSimd palSimdOR(palSimd a, palSimd b) {
  return _mm_or_ps(a, b);
}

PAL_INLINE palSimd palSimdXOR(palSimd a, palSimd b) {
  return _mm_xor_ps(a, b);
}

PAL_INLINE palSimd palSimdANDNOT(palSimd a, palSimd b) {
  return _mm_andnot_ps(a, b);
}

// performs a bitwise negation aka return ~a;
PAL_INLINE palSimd palSimdNEG(palSimd a) {
  palSimd all_on = palSimdSplat((int)0xffffffff);
  return palSimdXOR(a, all_on);
}

PAL_INLINE palSimd palSimdZero() {
  palSimd a;
  a = _mm_setzero_ps();
  return a;
}

#if !defined(PAL_PLATFORM_NACL)
PAL_INLINE palSimd palSimdRoundDown(palSimd a) {
  return _mm_round_ps(a, _MM_FROUND_TO_NEG_INF);
}

PAL_INLINE palSimd palSimdRoundUp(palSimd a) {
  return _mm_round_ps(a, _MM_FROUND_TO_POS_INF);
}

PAL_INLINE palSimd palSimdRoundNearest(palSimd a) {
  return _mm_round_ps(a, _MM_FROUND_TO_NEAREST_INT);
}
#endif

PAL_INLINE palSimd palSimdCompareLT(palSimd a, palSimd b) {
  return _mm_cmplt_ps(a, b);
}

PAL_INLINE palSimd palSimdCompareLTE(palSimd a, palSimd b) {
  return _mm_cmple_ps(a, b);
}

PAL_INLINE palSimd palSimdCompareE(palSimd a, palSimd b) {
  return _mm_cmpeq_ps(a, b);
}

PAL_INLINE palSimd palSimdCompareNE(palSimd a, palSimd b) {
  return _mm_cmpneq_ps(a, b);
}

PAL_INLINE palSimd palSimdCompareGTE(palSimd a, palSimd b) {
  return _mm_cmpge_ps(a, b);
}

PAL_INLINE palSimd palSimdCompareGT(palSimd a, palSimd b) {
  return _mm_cmpgt_ps(a, b);
}

/* result[i] = x[i] >= lo[i] && x[i] <= hi[i] */
PAL_INLINE palSimd palSimdCompareInRangeInclusive(palSimd x, palSimd lo, palSimd hi) {
  palSimd gtemask = palSimdCompareGTE(x, lo);
  palSimd ltemask = palSimdCompareLTE(x, hi);
  return palSimdAND(gtemask, ltemask);
}

/* result[i] = x[i] > lo[i] && x[i] < hi[i] */
PAL_INLINE palSimd palSimdCompareInRangeExclusive(palSimd x, palSimd lo, palSimd hi) {
  palSimd gtmask = palSimdCompareGT(x, lo);
  palSimd ltmask = palSimdCompareLT(x, hi);
  return palSimdAND(gtmask, ltmask);
}

PAL_INLINE palSimd palSimdSignMask(palSimd a) {
  __m128i ia = _mm_castps_si128(a);
  palSimd signbits = _mm_castsi128_ps(_mm_srli_epi32(ia, 31));
  // signbits != 0
  return palSimdCompareNE(signbits, palSimdZero());
}

PAL_INLINE bool palSimdCompareMaskAny(palSimd compare_mask) {
  return _mm_movemask_ps(compare_mask) != 0x0;
}

PAL_INLINE bool palSimdCompareMaskAll(palSimd compare_mask) {
  return _mm_movemask_ps(compare_mask) == 0xf;
}

PAL_INLINE bool palSimdCompareMaskNone(palSimd compare_mask) {
  return _mm_movemask_ps(compare_mask) == 0x0;
}

PAL_INLINE palSimd palSimdMakeMask(uint32_t mask0, uint32_t mask1, uint32_t mask2, uint32_t mask3) {
  __m128i mask;
  mask = _mm_set_epi32(mask3, mask2, mask1, mask0);
  return _mm_castsi128_ps(mask);
}

PAL_INLINE palSimd palSimdSelectUpper(palSimd a, palSimd b) {
  return _mm_movehl_ps(a, b);
}

PAL_INLINE palSimd palSimdSelectLower(palSimd a, palSimd b) {
  return _mm_movelh_ps(a, b);
}

// return[i] = if (compare_mask[i]) true_value[i] else false_value[i];
PAL_INLINE palSimd palSimdSelect(palSimd compare_mask, palSimd true_value, palSimd false_value) {
  return _mm_or_ps(_mm_and_ps(compare_mask, true_value),_mm_andnot_ps(compare_mask, false_value));
}


PAL_INLINE palSimd palSimdSetX(palSimd a, float x) {
  reinterpret_cast<float*>(&a)[0] = x;
  return a;
}

PAL_INLINE palSimd palSimdSetY(palSimd a, float y) {
  reinterpret_cast<float*>(&a)[1] = y;
  return a;
}

PAL_INLINE palSimd palSimdSetZ(palSimd a, float z) {
  reinterpret_cast<float*>(&a)[2] = z;
  return a;
}

PAL_INLINE palSimd palSimdSetW(palSimd a, float w) {
  reinterpret_cast<float*>(&a)[3] = w;
  return a;
}

PAL_INLINE float palSimdGetIndex(palSimd a, int index) {
  return reinterpret_cast<float*>(&a)[index];
}

PAL_INLINE palSimd palSimdSetIndex(palSimd a, int index, float v) {
  reinterpret_cast<float*>(&a)[index] = v;
  return a;
}

PAL_INLINE palSimd palSimdAdd(palSimd a, palSimd b) {
  return _mm_add_ps(a, b);
}

PAL_INLINE palSimd palSimdSub(palSimd a, palSimd b) {
  return _mm_sub_ps(a, b);
}

PAL_INLINE palSimd palSimdMul(palSimd a, palSimd b) {
  return _mm_mul_ps(a, b);
}

PAL_INLINE palSimd palSimdReciprocal(palSimd a) {
  return _mm_rcp_ps(a);
}

PAL_INLINE palSimd palSimdDiv(palSimd a, palSimd b) {
  return _mm_div_ps(a, b);
}

PAL_INLINE palSimd palSimdSqrt(palSimd a) {
  return _mm_sqrt_ps(a);
}

PAL_INLINE palSimd palSimdReciprocalSqrt(palSimd a) {
  return _mm_rsqrt_ps(a);
}

PAL_INLINE palSimd palSimdMin(palSimd a, palSimd b) {
  return _mm_min_ps(a, b);
}

PAL_INLINE palSimd palSimdMax(palSimd a, palSimd b) {
  return _mm_max_ps(a, b);
}

PAL_INLINE palSimd palSimdClamp(palSimd min, palSimd max, palSimd a) {
  return palSimdMin(palSimdMax(min, a), max);
}

PAL_INLINE palSimd palSimdAbsolute(palSimd a) {
  palSimd absolute_mask = palSimdMakeMask(0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF);
  return _mm_and_ps(absolute_mask, a);
}

PAL_INLINE palSimd palSimdNegative(palSimd a) {
  palSimd negative_zero = { -0.0f, -0.0f, -0.0f, -0.0f};
  return _mm_xor_ps(negative_zero, a);
}

PAL_INLINE palSimd palSimdDot(palSimd a, palSimd b) {
  // the mask is actually two 4-bit masks:
  // 0xab
  // a == read mask
  // b == write mask
  // eachb it in the read mask indicates a component that is included in the dot product
  // each bit in the write mask indicates a component that the result is written to
  // 0xff == 4D dot product written to all 4 components of the result
  // 0x77 == 3D dot product (x,y,z) written into (x,y,z)
  // 0xf7 == 4D dot product written into (x,y,z)
  // 0x7f == 3D dot product written into all 4 components of the result
  //return _mm_dp_ps(a, b, 0xff);
  {
    float* fa = (float*)&a;
    float* fb = (float*)&b;
    float dp = fa[0] * fb[0] + fa[1] * fb[1] + fa[2] * fb[2] + fa[3] * fb[3];
    return palSimdSplat(dp);
  }
  //palSimd t0 = _mm_mul_ps(a, b);
  //palSimd t1 = _mm_shuffle_ps(t0, t0, _MM_SHUFFLE(1,0,3,2));
  //palSimd t2 = _mm_add_ps(t0, t1);
  //palSimd t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(2,3,0,1));
  //palSimd dot = _mm_add_ps(t3, t2);
  //return dot;
}

PAL_INLINE palSimd palSimdDot3(palSimd a, palSimd b) {
  /* Dot between xyz written into all four slots of the result */
  //return _mm_dp_ps(a, b, 0x7f);
  {
    float* fa = (float*)&a;
    float* fb = (float*)&b;
    float dp = fa[0] * fb[0] + fa[1] * fb[1] + fa[2] * fb[2];
    return palSimdSplat(dp);
  }
}

PAL_INLINE palSimd palSimdCross(palSimd a, palSimd b) {
  palSimd t0, t1, t2, t3, t4, result;

  // t0 = { a.y, a.z, a.x, a.w }
  t0 = _mm_shuffle_ps( a, a, _MM_SHUFFLE(3,0,2,1) );

  // t1 = { b.z, b.x, b.y, b.w }
  t1 = _mm_shuffle_ps( b, b, _MM_SHUFFLE(3,1,0,2) );

  // t2 = { a.z, a.x, a.y, a.w }
  t2 = _mm_shuffle_ps( a, a, _MM_SHUFFLE(3,1,0,2) );

  // t3 = { b.y, b.z, b.x, b.w }
  t3 = _mm_shuffle_ps( b, b, _MM_SHUFFLE(3,0,2,1) );

  // result = { a.y * b.z, a.z * b.x, a.x * b.y, a.w * b.w }
  result = palSimdMul( t0, t1 );

  // t4     = { a.z * b.y, a.x * b.z, a.y * b.x, a.w * b.w }
  t4 = palSimdMul(t2, t3);

  // result = { a.y * b.z - a.z * b.y,
  //            a.z * b.x - a.x * b.z,
  //            a.x * b.y - a.y * b.x, 
  //            0 }
  result = palSimdSub(result, t4);
  return result;
}

PAL_INLINE palSimd __palSimdSin(palSimd x) {
  //either the value is a NaN or it's in the range [-PI, PI]
  //assert( all( nanMask(x) | inRangeMask(x, reint_i2f(sse4Ints::expand(0xc0490fdb)), reint_i2f(sse4Ints::expand(0x40490fdb))) ) );

  palSimd c3 = palSimdSplat((int)0xbe2aaaab); // -0.166667f
  palSimd c5 = palSimdSplat((int)0x3c0887e6); //  0.008333f
  palSimd c7 = palSimdSplat((int)0xb94fc635); // -0.000198f
  palSimd c9 = palSimdSplat((int)0x362f5e1d); //  0.000003f

  palSimd x2 = palSimdMul(x,x);
  palSimd x3 = palSimdMul(x, x2);
  // rval = x + x3*c3 + x3*x2*(x2*c7 + c5 + x2*x2*c9);
  palSimd rval = x;
  rval = palSimdAdd(rval, palSimdMul(x3,c3));
  palSimd tmp = palSimdMul(x2, c7);
  tmp = palSimdAdd(tmp, c5);
  tmp = palSimdAdd(tmp, palSimdMul(x2, palSimdMul(x2, c9)));
  rval = palSimdAdd(rval, palSimdMul(palSimdMul(x3,x2), tmp));

  palSimd threshold = palSimdSplat((int)0x39e89769);
  palSimd abs_x = palSimdAbsolute(x);
  // abs_x < threshold
  palSimd ltmask = palSimdCompareLT(abs_x, threshold);
  // fix up values that generate 0 but should just be x,
  // below this absolute value cutoff x and sin(x) are identical
  return palSimdSelect(ltmask, x, rval);
}

PAL_INLINE palSimd palSimdSin(palSimd x) {
#if defined(TRUE_SIMD_TRIG)
  palSimd pi = palSimdSplat((int)0x40490fdb); // 3.141593f
  palSimd inv_pi = palSimdSplat((int)0x3ea2f983); // 1.0f / 3.141593f

  // (int)inv_pi * x
  __m128i ipart = _mm_castps_si128(_mm_mul_ps(inv_pi, x));
  // (float)ipart
  palSimd ipart_ps = _mm_castsi128_ps(ipart);
  // ipart_shifted = (float)(ipart << 31);
  palSimd ipart_shifted = _mm_castsi128_ps(_mm_slli_epi32(ipart, 31));

  // if ipart is odd, set the sign bit to make x_ror negative
  palSimd x_ror = palSimdXOR(ipart_shifted, palSimdSub(x, palSimdMul(ipart_ps, pi)));

  return __palSimdSin(x_ror);
#else
  float sin_x = palScalar::Sin(palSimdGetX(x));
  float sin_y = palScalar::Sin(palSimdGetY(x));
  float sin_z = palScalar::Sin(palSimdGetZ(x));
  float sin_w = palScalar::Sin(palSimdGetW(x));
  return palSimdSetXYZW(sin_x, sin_y, sin_z, sin_w);
#endif
}

PAL_INLINE palSimd palSimdCos(palSimd x) {
#if defined(TRUE_SIMD_TRIG)
  palSimd half_pi = palSimdSplat((int)0x3fc90fdb); // 1.570796f
  return palSimdSin(palSimdAdd(x,half_pi));
#else
  float cos_x = palScalar::Cos(palSimdGetX(x));
  float cos_y = palScalar::Cos(palSimdGetY(x));
  float cos_z = palScalar::Cos(palSimdGetZ(x));
  float cos_w = palScalar::Cos(palSimdGetW(x));
  return palSimdSetXYZW(cos_x, cos_y, cos_z, cos_w);
#endif
}

PAL_INLINE palSimd palSimdTan(palSimd x) {
  float tan_x = palScalar::Tan(palSimdGetX(x));
  float tan_y = palScalar::Tan(palSimdGetY(x));
  float tan_z = palScalar::Tan(palSimdGetZ(x));
  float tan_w = palScalar::Tan(palSimdGetW(x));
  return palSimdSetXYZW(tan_x, tan_y, tan_z, tan_w);
}

PAL_INLINE palSimd palSimdAcos(palSimd x) {
#if defined(TRUE_SIMD_TRIG)
  palAssert(false);
#else
  float acos_x = palScalar::ACos(palSimdGetX(x));
  float acos_y = palScalar::ACos(palSimdGetY(x));
  float acos_z = palScalar::ACos(palSimdGetZ(x));
  float acos_w = palScalar::ACos(palSimdGetW(x));
  return palSimdSetXYZW(acos_x, acos_y, acos_z, acos_w);
#endif
}

PAL_INLINE palSimd palSimdAsin(palSimd x) {
#if defined(TRUE_SIMD_TRIG)
  palAssert(false);
#else
  float asin_x = palScalar::ASin(palSimdGetX(x));
  float asin_y = palScalar::ASin(palSimdGetY(x));
  float asin_z = palScalar::ASin(palSimdGetZ(x));
  float asin_w = palScalar::ASin(palSimdGetW(x));
  return palSimdSetXYZW(asin_x, asin_y, asin_z, asin_w);
#endif
}

PAL_INLINE palSimd palSimdPow(palSimd x, palSimd y) {
  float pow_x = palScalar::Pow(palSimdGetX(x), palSimdGetX(y));
  float pow_y = palScalar::Pow(palSimdGetY(x), palSimdGetY(y));
  float pow_z = palScalar::Pow(palSimdGetZ(x), palSimdGetZ(y));
  float pow_w = palScalar::Pow(palSimdGetW(x), palSimdGetW(y));
  return palSimdSetXYZW(pow_x, pow_y, pow_z, pow_w);
}

PAL_INLINE palSimd palSimdLn(palSimd x) {
  __m128i emm0;

  palSimd one = palSimdSplat((int)0x3f800000); // 1.0f
  palSimd min_norm = palSimdSplat((int)0x00800000); // smallest normalized number
  palSimd inv_mant_mask = palSimdSplat((int)~0x7f800000);
  palSimd half = palSimdSplat(0.5f);
  palSimd c1 = palSimdSplat((int)0x7f);
  palSimd sqrt_half = palSimdSplat(0.707106781186547524f);
  palSimd cephes_log_p0 = palSimdSplat(7.0376836292e-2f);
  palSimd cephes_log_p1 = palSimdSplat(- 1.1514610310e-1f);
  palSimd cephes_log_p2 = palSimdSplat(1.1676998740e-1f);
  palSimd cephes_log_p3 = palSimdSplat(- 1.2420140846e-1f);
  palSimd cephes_log_p4 = palSimdSplat(+ 1.4249322787e-1f);
  palSimd cephes_log_p5 = palSimdSplat(- 1.6668057665e-1f);
  palSimd cephes_log_p6 = palSimdSplat(+ 2.0000714765e-1f);
  palSimd cephes_log_p7 = palSimdSplat(- 2.4999993993e-1f);
  palSimd cephes_log_p8 = palSimdSplat(+ 3.3333331174e-1f);
  palSimd cephes_log_q1 = palSimdSplat(-2.12194440e-4f);
  palSimd cephes_log_q2 = palSimdSplat(0.693359375f);


  palSimd invalid_mask = _mm_cmple_ps(x, _mm_setzero_ps());

  x = _mm_max_ps(x, min_norm);  /* cut off denormalized stuff */

  emm0 = _mm_srli_epi32(_mm_castps_si128(x), 23);

  /* keep only the fractional part */
  x = _mm_and_ps(x, inv_mant_mask);
  x = _mm_or_ps(x, half);

  emm0 = _mm_sub_epi32(emm0, _mm_castps_si128(c1));
  palSimd e = _mm_cvtepi32_ps(emm0);

  e = _mm_add_ps(e, one);

  /* part2: 
     if( x < SQRTHF ) {
       e -= 1;
       x = x + x - 1.0;
     } else { x = x - 1.0; }
  */
  palSimd mask = _mm_cmplt_ps(x, sqrt_half);
  palSimd tmp = _mm_and_ps(x, mask);
  x = _mm_sub_ps(x, one);
  e = _mm_sub_ps(e, _mm_and_ps(one, mask));
  x = _mm_add_ps(x, tmp);

  palSimd z = _mm_mul_ps(x,x);

  palSimd y = cephes_log_p0;
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p1);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p2);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p3);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p4);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p5);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p6);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p7);
  y = _mm_mul_ps(y, x);
  y = _mm_add_ps(y, cephes_log_p8);
  y = _mm_mul_ps(y, x);

  y = _mm_mul_ps(y, z);
  

  tmp = _mm_mul_ps(e, cephes_log_q1);
  y = _mm_add_ps(y, tmp);


  tmp = _mm_mul_ps(z, half);
  y = _mm_sub_ps(y, tmp);

  tmp = _mm_mul_ps(e, cephes_log_q2);
  x = _mm_add_ps(x, y);
  x = _mm_add_ps(x, tmp);
  x = _mm_or_ps(x, invalid_mask); // negative arg will be NAN
  return x;
}

PAL_INLINE palSimd palSimdLog(palSimd x) {
  palAssert(false);
  return palSimdZero();
}

// domain: [0, 1]
// range:  [0, PI/4]
PAL_INLINE palSimd __atan_rd(palSimd x) {
  // either the value is a NaN or it's in the range [0, 1]
  //assert( all( nanMask(x) |
  //  inRangeMask(x, sse4Floats::zeros(),
  //  reint_i2f(sse4Ints::expand(0x3f800000))) ) );

  // using Euler's version of the atan series expansion, which converges quickly
  palSimd c1 = palSimdSplat((int)0x3f800000);        //              1.0f
  palSimd c2 = palSimdSplat((int)0x3f2aaaab);        //    2.0f /    3.0f
  palSimd c3 = palSimdSplat((int)0x3f088889);        //    8.0f /   15.0f
  palSimd c4 = palSimdSplat((int)0x3eea0ea1);        //   16.0f /   35.0f
  palSimd c5 = palSimdSplat((int)0x3ed00d01);        //  128.0f /  315.0f
  palSimd c6 = palSimdSplat((int)0x3ebd2318);        //  256.0f /  693.0f
  palSimd c7 = palSimdSplat((int)0x3eae968c);        // 1024.0f / 3003.0f

  // q = x / (x^2+c)
  palSimd q = palSimdDiv(x, palSimdAdd(palSimdMul(x,x), c1));

  palSimd z   = palSimdMul(x, q);
  palSimd z_2 = palSimdMul(z, z);
  palSimd z_3 = palSimdMul(z, z_2);
  // s   = c1 + c2*z + c3*z_2 + z_3*(c5*z + c4 + c6*z_2 + c7*z_3);
  // t1 = c1 + c2 * z
  palSimd t1 = palSimdAdd(c1, palSimdMul(c2, z));
  // t2 = c3 * z_2
  palSimd t2 = palSimdMul(c3, z_2);
  // t3 = (c5*z + c4 + c6*z_2 + c7*z_3)
  palSimd t3 = palSimdAdd(palSimdMul(c5, z), palSimdAdd(c4, palSimdAdd(palSimdMul(c6, z_2), palSimdMul(c7, z_3))));
  palSimd s = palSimdAdd(t1, palSimdAdd(t2, palSimdMul(z_3, t3)));
  palSimd rval = palSimdMul(q, s);

  // fix up values that generate 0 but should just be x,
  // below this cutoff, x and atan(x) are identical
  palSimd thr = palSimdSplat((int)0x39b89ba3); // 0.000352f
  palSimd ltmask = palSimdCompareLT(x, thr);
  return palSimdSelect(ltmask, x, rval);
}

PAL_INLINE palSimd palSimdAtan(palSimd x) {
  palSimd one = palSimdSplat((int)0x3f800000); // 1.0f

  // use the following identities:
  // 1) atan(x) = PI/2 - atan(1/x)
  // 2) atan(x) = -atan(-x)
  // ...so that all input is transformed into the range [0, 1]

  // take absolute value
  palSimd neg_x = palSimdCompareLT(x, palSimdZero());
  palSimd sign_conv = palSimdSelect(neg_x, palSimdNegative(one), one);
  palSimd abs_x = palSimdMul(sign_conv, x);

  // invert all values that are greater than one
  palSimd inv_mask = palSimdCompareGT(abs_x, one);
  palSimd inv_abs_x = palSimdReciprocal(abs_x);
  palSimd x_ror = palSimdSelect(inv_mask, inv_abs_x, abs_x);

  // call the helper on the in-range values
  palSimd atan_rd = __atan_rd(x_ror);

  // fix signs based on the signs of the input
  palSimd signs_fixed = palSimdMul(sign_conv, atan_rd);

  // correct the output range for all inverted input by
  // either subtracting from PI/2 or -PI/2, depending on the
  // sign of signs_fixed (which matches the neg_x mask)
  palSimd half_pi = palSimdSplat((int)0x3fc90fdb);   // 1.570796f
  palSimd base = palSimdSelect(neg_x, palSimdNegative(half_pi), half_pi);
  palSimd range_fixed = palSimdSelect(inv_mask, palSimdSub(base,signs_fixed), signs_fixed);

  return range_fixed;
}

PAL_INLINE palSimd palSimdAtan2(palSimd y, palSimd x) {
  palSimd pi = palSimdSplat((int)0x40490fdb); // 3.141593f

  // compute the atan
  palSimd raw_atan = palSimdAtan(palSimdDiv(y, x));

  // treat -0 as though it were negative
  palSimd neg_x = palSimdSignMask(x);
  palSimd neg_y = palSimdSignMask(y);

  // fix up quadrants 2 and 3 based on the sign of the input

  palSimd all_on = palSimdSplat((int)0xffffffff);
  // move from quadrant 4 to 2 by adding PI
  palSimd in_quad2 = palSimdAND(neg_x, palSimdXOR(neg_y, all_on));
  palSimd quad2_fixed = palSimdSelect(in_quad2, palSimdAdd(raw_atan, pi), raw_atan);

  // move from quadrant 1 to 3 by subtracting PI
  palSimd in_quad3 = palSimdAND(neg_x,neg_y);
  palSimd quad23_fixed = palSimdSelect(in_quad3, palSimdSub(raw_atan, pi), quad2_fixed);

  return quad23_fixed;
}

// computes 2^x, input is in integer format, output is in float format
// domain: [-126, 127]
// range:  [2^-126, 2^127]
PAL_INLINE palSimd __exp_exponent(__m128i x) {
  palSimd c1 = palSimdSplat((int)0x3f800000);
  palSimd x_shifted = _mm_castsi128_ps(_mm_slli_epi32(x, 23));
  return palSimdAdd(x_shifted, c1);
}

// computes e^x
// domain: [0.0, log_2(e)],
// range:  [1.0, 2.0)
PAL_INLINE  palSimd __exp_mantissa(palSimd x) {
  palSimd c1 = palSimdSplat((int)0x3f800000);        // 1.0f
  palSimd c2 = palSimdSplat((int)0x3f000000);        // 0.5f
  palSimd c3 = palSimdSplat((int)0x3e2aaa1d);        // 0.166665f
  palSimd c5 = palSimdSplat((int)0x3d093a89);        // 0.033503f
  palSimd c6 = palSimdSplat((int)0x3bb71b61);        // 0.005588f

  palSimd x2 = palSimdMul(x,x);
  palSimd x2_2 = palSimdMul(x2,c2);

  // return c1 + x + x2_2 + c3*x*x2 + x2_2*x2_2*(c3 + c5*x + c6*x2);
  // c1 + x + x2_2
  palSimd t0 = palSimdAdd(c1, palSimdAdd(x, x2_2));
  // c3 * x * x2
  palSimd t1 = palSimdMul(c3, palSimdMul(x, x2));
  // x2_2*x2_2
  palSimd t2 = palSimdMul(x2_2, x2_2);
  // c3 + c5*x + c6*x2
  palSimd t3 = palSimdAdd(c3, palSimdAdd(palSimdMul(c5, x), palSimdMul(c6, x2)));
  return palSimdAdd(t0, palSimdAdd(t1, palSimdMul(t2, t3)));
}


// handles everything in the reduced domain (0xc2aeac51, 0x42b0c0a6) which is
// approximately (-87.3, 88.4), if the input is not in this range
// the results are undefined
PAL_INLINE palSimd __exp_rd(palSimd x) {
  palSimd log_2e = palSimdSplat((int)0x3fb8aa3b);    // 1.442695f
  palSimd log_e2 = palSimdSplat((int)0x3f317218);    // 0.693147f

  __m128i pre_e = _mm_castps_si128(palSimdMul(log_2e, x));
  palSimd pre_m = palSimdSub(x,palSimdMul(log_e2,_mm_castsi128_ps(pre_e)));  // generates mantissa

  return palSimdMul(__exp_exponent(pre_e), __exp_mantissa(pre_m));

}

// NOTE: the output of this function produces infinity at a lower value of x
// than the reference version, at x = 88.376266 rather than x = 88.722839

PAL_INLINE palSimd palSimdExp(palSimd x) {
  palSimd min_thr = palSimdSplat((int)0xc2aeac51);   // -87.336555f
  palSimd max_thr = palSimdSplat((int)0x42b0c0a6);   //  88.376266f

  palSimd clamp0 = palSimdMax(min_thr, x);
  palSimd clamp1 = palSimdMin(max_thr, clamp0);

  return __exp_rd(clamp1);
}

PAL_INLINE palSimd palSimdLength2(palSimd a) {
  return palSimdDot(a, a);
}

PAL_INLINE palSimd palSimdLength(palSimd a) {
  palSimd length2 = palSimdLength2(a);
  return palSimdSqrt(length2);
}

PAL_INLINE void palSimdMatrixTranspose(palSimd* a, palSimd* b, palSimd* c, palSimd* d) {
  _MM_TRANSPOSE4_PS(*a, *b, *c, *d);
}

#endif  // LIBPAL_PAL_SIMD_SSE_H_
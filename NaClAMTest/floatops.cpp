#include "floatops.h"

float sumFloatArray(float* floats, uint32_t length) {
  float sum = 0.0;
  for (uint32_t i = 0; i < length; i++) {
    sum += floats[i];
  }
  return sum;
}

void subFloatArrays(float* src1_dst, float* src2, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    src1_dst[i] -= src2[i];
  }
}

void addFloatArrays(float* src1_dst, float* src2, uint32_t length) {
  for (uint32_t i = 0; i < length; i++) {
    src1_dst[i] += src2[i];
  }
}
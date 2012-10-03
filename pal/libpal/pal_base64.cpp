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

#include "libpal/pal_debug.h"
#include "libpal/pal_base64.h"

int palBase64::Encode(const unsigned char* data, size_t input_length, char* target, size_t* output_length) {
  static const char Base64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
  static const char Pad64 = '=';
  size_t datalength = 0;
  unsigned char input[3];
  unsigned char output[4];
  size_t i;
  size_t srclength = input_length;
  const unsigned char* src = data;

  while (2 < srclength) {
    input[0] = *src++;
    input[1] = *src++;
    input[2] = *src++;
    srclength -= 3;

    output[0] = input[0] >> 2;
    output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
    output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
    output[3] = input[2] & 0x3f;
    palAssert(output[0] < 64);
    palAssert(output[1] < 64);
    palAssert(output[2] < 64);
    palAssert(output[3] < 64);

    if (datalength + 4 > *output_length)
      return (-1);

    target[datalength++] = Base64[output[0]];
    target[datalength++] = Base64[output[1]];
    target[datalength++] = Base64[output[2]];
    target[datalength++] = Base64[output[3]];
  }

  if (0 != srclength) {
    input[0] = input[1] = input[2] = '\0';
    for (i = 0; i < srclength; i++)
      input[i] = *src++;

    output[0] = input[0] >> 2;
    output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
    output[2] = ((input[1] & 0x0f) << 2) + (input[2] >> 6);
    palAssert(output[0] < 64);
    palAssert(output[1] < 64);
    palAssert(output[2] < 64);

    if (datalength + 4 > *output_length)
      return (-1);
    target[datalength++] = Base64[output[0]];
    target[datalength++] = Base64[output[1]];
    if (srclength == 1)
      target[datalength++] = Pad64;
    else
      target[datalength++] = Base64[output[2]];
    target[datalength++] = Pad64;
  }

  *output_length = datalength;
  return 0;
}

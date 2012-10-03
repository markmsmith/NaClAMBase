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
#include "libpal/pal_mem_blob.h"

class palImage {
  unsigned char* pixels;
  int x;
  int y;
public:
  palImage();
  ~palImage();

  void Reset();

  bool Load(unsigned char* buffer, uint64_t length);

  unsigned char* GetPixels() const;
  int GetWidth() const;
  int GetHeight() const;
};

#if defined(PAL_ENABLE_TURBOJPEG)
class palJPEGEncoder {
  void* _instance;
public:
  palJPEGEncoder();

  int Init();
  int Fini();

  // Assumes an 32-bit RGBA image, can expose more formats if necessary
  // Encodes with 4:2:0 chrominance subsampling, can expose more formats if necessary
  palMemBlob Compress(void* src, int width, int pitch, int height, int jpeg_quality); 

  void FreeCompressedImage(palMemBlob image);
};
#endif
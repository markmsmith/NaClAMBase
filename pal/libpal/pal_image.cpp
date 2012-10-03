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


#if defined(PAL_ENABLE_TURBOJPEG)
#include "libpal/turbo-jpeg/include/turbojpeg.h"
#endif
#define STBI_NO_STDIO
#define STBI_HEADER_FILE_ONLY
#include "libpal/stb/stb_image.h"
#include "libpal/pal_allocator.h"
#include "libpal/pal_image.h"

palImage::palImage() {
  pixels = NULL;
  x = 0;
  y = 0;
}

palImage::~palImage() {
  if (pixels) {
    g_StdProxyAllocator->Deallocate(pixels);
  }
}

void palImage::Reset() {
  if (pixels) {
    g_StdProxyAllocator->Deallocate(pixels);
  }
  pixels = NULL;
  x = 0;
  y = 0;
}

bool palImage::Load(unsigned char* buffer, uint64_t length) {
  int components;
  int required_components = 4;
  pixels = stbi_load_from_memory(buffer, (int)length, &x, &y, &components, required_components);
  return pixels != NULL;
}

unsigned char* palImage::GetPixels() const {
  return pixels;
}

int palImage::GetWidth() const {
  return x;
}

int palImage::GetHeight() const {
  return y;
}

#if defined(PAL_ENABLE_TURBOJPEG)

palJPEGEncoder::palJPEGEncoder() {
  _instance = NULL;
}

int palJPEGEncoder::Init() {
  _instance = tjInitCompress();
  if (_instance == NULL) {
    return -1;
  }
  return 0;
}

int palJPEGEncoder::Fini() {
  if (_instance) {
    tjDestroy(_instance);
  }
  _instance = NULL;
  return 0;
}

palMemBlob palJPEGEncoder::Compress(void* src, int width, int pitch, int height, int jpeg_quality) {
  unsigned char* jpeg_out = NULL;
  unsigned long jpeg_out_len = 0;
  int r = tjCompress2(_instance, (unsigned char*)src, width, pitch, height, TJPF_RGBX, &jpeg_out, &jpeg_out_len, TJSAMP_420, jpeg_quality, 0);
  if (r != 0) {
    return palMemBlob();
  }
  return palMemBlob(jpeg_out, jpeg_out_len);
}

void palJPEGEncoder::FreeCompressedImage(palMemBlob image) {
  unsigned char* jpeg_buf = image.GetPtr<unsigned char>(0);
  if (jpeg_buf) {
    tjFree(jpeg_buf);
  }
}
#endif
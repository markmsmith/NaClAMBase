/*
  Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_FONT_RASTERIZER_STB_H_
#define LIBPAL_PAL_FONT_RASTERIZER_STB_H_

#include "libpal/stb/stb_truetype.h"

#include "libpal/pal_font_rasterizer.h"

/* The STB font rasterizer can use this dumb linear allocator. It is reset after each glyph is rasterized */
/* 16KB now, but 8KB is probably enough. */
template<int PAL_FONT_RASTERIZER_BUFFER_SIZE=16*1024>
struct palFontRasterizerStbMemoryBuffer {
  unsigned int memory_buffer_size_;
  unsigned int memory_buffer_cursor_;
  unsigned char memory_buffer_[PAL_FONT_RASTERIZER_BUFFER_SIZE];

  palFontRasterizerStbMemoryBuffer() {
    memory_buffer_size_ = PAL_FONT_RASTERIZER_BUFFER_SIZE;
    memory_buffer_cursor_ = 0;
  }

  void* allocate_from_memory_buffer(int size) {
    palAssert(memory_buffer_cursor_+size < PAL_FONT_RASTERIZER_BUFFER_SIZE);
    void* r = &memory_buffer_[memory_buffer_cursor_];
    memory_buffer_cursor_ += size;
    return r;
  }
  void free_from_memory_buffer(void* addr) {
    // NOP
  }
  void reset_memory_bufer() {
    memory_buffer_cursor_ = 0;
  }
};

class palFontRasterizerStb : public palFontRasterizer {
  const unsigned char* font_data_;
  stbtt_fontinfo font_info_;
  int loaded_glyph_;
  float scale_for_height_;

  // bitmap stuff
  int glyph_bitmap_width_;
  int glyph_bitmap_height_;
  int glyph_bitmap_x_offset_;
  int glyph_bitmap_y_offset_;
  unsigned char* glyph_bitmap_;

  palFontRasterizerStbMemoryBuffer<>* memory_buffer_;
public:

  palFontRasterizerStb();
  virtual ~palFontRasterizerStb();

  void Init(palFontRasterizerStbMemoryBuffer<>* memory_buffer);

  virtual palFontRasterizerError InitFont(const unsigned char* font_data,  int font_data_length, int face_index);

  virtual int GetGlyphIndex(int ch);
  virtual void GetFontExtents(palFontExtents* font_extents);
  virtual int GetKerningAdvance(int glyph1, int glyph2);

  virtual void SetHeightInPixels(float font_height_in_pixels);

  virtual void LoadGlyph(int glyph);
  virtual void GetGlyphExtents(palFontGlyphExtents* glyph_extents);

  virtual void RasterizeGlyph();
  virtual void GetBitmap(unsigned char** pixels, int* bitmap_left, int* bitmap_top, int* width, int* height, int* pitch);
  virtual void GetGlyphRasterBox(palFontGlyphRasterBox* raster_box);

  
};

#endif  // LIBPAL_PAL_FONT_RASTERIZER_STB_H_
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

#include "libpal/libpal.h"
#include "libpal/pal_font_rasterizer_stb.h"

#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_malloc(x,u) ((palFontRasterizerStbMemoryBuffer<>*)u)->allocate_from_memory_buffer(x)
#define STBTT_free(x,u) ((palFontRasterizerStbMemoryBuffer<>*)u)->free_from_memory_buffer(x)
#include "libpal/stb/stb_truetype.h"

palFontRasterizerStb::palFontRasterizerStb() {
  font_data_ = NULL;
  memory_buffer_ = NULL;
  memset(&font_info_, 0, sizeof(font_info_));
  font_info_.userdata = NULL;
  loaded_glyph_ = 0;
  scale_for_height_ = 1.0f/64.0f;
}

palFontRasterizerStb::~palFontRasterizerStb() {
}

void palFontRasterizerStb::Init(palFontRasterizerStbMemoryBuffer<>* memory_buffer) {
  memory_buffer_ = memory_buffer;
  font_info_.userdata = memory_buffer;
}

palFontRasterizerError palFontRasterizerStb::InitFont(const unsigned char* font_data,  int font_data_length, int face_index) {
  font_data_ = font_data;
  int offset = stbtt_GetFontOffsetForIndex(font_data, face_index);
  if (offset == -1) {
    return PAL_FONT_RASTERIZER_INVALID_INDEX;
  }
  int r = stbtt_InitFont(&font_info_, font_data_, offset);
  if (r == 0) {
    return PAL_FONT_RASTERIZER_ERROR;
  }
  return PAL_FONT_RASTERIZER_NO_ERROR;
}

int palFontRasterizerStb::GetGlyphIndex(int ch) {
  return stbtt_FindGlyphIndex(&font_info_, ch);
}

void palFontRasterizerStb::GetFontExtents(palFontExtents* font_extents) {
  stbtt_GetFontVMetrics(&font_info_, &font_extents->ascent, &font_extents->descent, &font_extents->linegap);
  // scale
  font_extents->ascent = (int)((float)font_extents->ascent * scale_for_height_);
  font_extents->descent = (int)((float)font_extents->descent * scale_for_height_);
  font_extents->linegap = (int)((float)font_extents->linegap * scale_for_height_);
  // compute next line delta
  font_extents->next_line_delta = font_extents->ascent - font_extents->descent + font_extents->linegap;
}

int palFontRasterizerStb::GetKerningAdvance(int glyph1, int glyph2) {
  return (int)((float)stbtt_GetGlyphKernAdvance(&font_info_, glyph1, glyph2) * scale_for_height_);
}

void palFontRasterizerStb::SetHeightInPixels(float font_height_in_pixels) {
  int ascent;
  int descent;
  int linegap;
  stbtt_GetFontVMetrics(&font_info_, &ascent, &descent, &linegap);
  //scale_for_height_ = font_height_in_pixels / ascent;
  scale_for_height_ = stbtt_ScaleForPixelHeight(&font_info_, font_height_in_pixels);
}

void palFontRasterizerStb::LoadGlyph(int glyph) {
  memory_buffer_->reset_memory_bufer();
  glyph_bitmap_ = NULL;
  loaded_glyph_ = glyph;
}

void palFontRasterizerStb::GetGlyphExtents(palFontGlyphExtents* glyph_extents) {
  stbtt_GetGlyphHMetrics(&font_info_, loaded_glyph_, &glyph_extents->post_x_offset, &glyph_extents->pre_x_offset);
  // scale for pixel height
  glyph_extents->post_x_offset = (int)((float)glyph_extents->post_x_offset * scale_for_height_);
  glyph_extents->pre_x_offset = (int)((float)glyph_extents->pre_x_offset * scale_for_height_);
}

void palFontRasterizerStb::RasterizeGlyph() {
  glyph_bitmap_ = stbtt_GetGlyphBitmap(&font_info_, scale_for_height_, scale_for_height_, loaded_glyph_, &glyph_bitmap_width_, &glyph_bitmap_height_, &glyph_bitmap_x_offset_, &glyph_bitmap_y_offset_);
}

void palFontRasterizerStb::GetBitmap(unsigned char** pixels, int* bitmap_left, int* bitmap_top, int* width, int* height, int* pitch) {
  *pixels = glyph_bitmap_;
  *bitmap_left = glyph_bitmap_x_offset_;
  *bitmap_top = glyph_bitmap_y_offset_;
  *width = glyph_bitmap_width_;
  *height = glyph_bitmap_height_;
  *pitch = glyph_bitmap_width_;
}

void palFontRasterizerStb::GetGlyphRasterBox(palFontGlyphRasterBox* raster_box) {
  stbtt_GetGlyphBitmapBox(&font_info_, loaded_glyph_, scale_for_height_, scale_for_height_, &raster_box->min[0], &raster_box->min[1], &raster_box->max[0], &raster_box->max[1]);
}
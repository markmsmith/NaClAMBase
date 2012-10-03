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

#pragma once

#if defined(PAL_ENABLE_FREETYPE)
#include "libpal/pal_font_rasterizer.h"

class palFontRasterizerFreeType : public palFontRasterizer {
  // these are NULL just to keep the freetype headers from being included.
  void* ft_memory_;
  void* ft_library_;
  void* ft_face_;
public:

  palFontRasterizerFreeType();
  virtual ~palFontRasterizerFreeType();

  virtual palFontRasterizerError InitFont(const unsigned char* font_data,  int font_data_length, int font_index);

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

#endif
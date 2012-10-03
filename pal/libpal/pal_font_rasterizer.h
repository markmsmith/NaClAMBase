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

struct palFontExtents {
  int ascent;
  int descent;
  int linegap;
  int next_line_delta;
};

struct palFontGlyphExtents {
  int pre_x_offset;
  int post_x_offset;
};

struct palFontGlyphRasterBox {
  int min[2];
  int max[2];
  int GetWidth() const { return max[0] - min[0]; }
  int GetHeight() const { return max[1] - min[1]; }
};

struct palFontGlyphBox {
  float min[2];
  float max[2];

  float GetWidth() const { return max[0] - min[0]; }
  float GetHeight() const { return max[1] - min[1]; }
};

enum palFontRasterizerError {
  PAL_FONT_RASTERIZER_NO_ERROR,
  PAL_FONT_RASTERIZER_ERROR,
  PAL_FONT_RASTERIZER_INVALID_INDEX,
};

/* Behaves as a state machine:
 *
 *
 * LoadGlyph and SetSize change the state of the machine
 *
 * RasterizeGlyph will rasterize the currently loaded glyph at the currently set size
 *
 */
class palFontRasterizer {
public:
  
  palFontRasterizer() {};
  virtual ~palFontRasterizer() {};

  virtual palFontRasterizerError InitFont(const unsigned char* font_data, int font_data_length, int face_index) = 0;

  virtual int GetGlyphIndex(int ch) = 0;
  virtual void GetFontExtents(palFontExtents* font_extents) = 0;
  virtual int GetKerningAdvance(int glyph1, int glyph2) = 0;

  virtual void SetHeightInPixels(float font_height_in_pixels) = 0;

  virtual void LoadGlyph(int glyph) = 0;
  virtual void GetGlyphExtents(palFontGlyphExtents* glyph_extents) = 0;

  virtual void RasterizeGlyph() = 0;
  virtual void GetBitmap(unsigned char** pixels, int* bitmap_left, int* bitmap_top, int* width, int* height, int* pitch) = 0;
  virtual void GetGlyphRasterBox(palFontGlyphRasterBox* raster_box) = 0;  
};

/*
  Copyright (c) 2011 John McCutchan <john@johnmccutchan.com>

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

#include "libpal/pal_platform.h"
#include "libpal/pal_string.h"

class palPath {
public:
  static char GetDirectorySeperator();

  

  static uint64_t GetAccessTime(const char* path);
  static uint64_t GetModificationTime(const char* path);
  static uint64_t GetCreationTime(const char* path);

  static bool IsAbsolutePath(const char* path);
  static bool Exists(const char* path);
  static bool IsDirectory(const char* path);
  static bool IsFile(const char* path);

  static bool Split(const char* path, palDynamicString* head, palDynamicString* tail);
  static bool SplitDrive(const char* path, palDynamicString* drive, palDynamicString* tail);
  static bool SplitExtension(const char* path, palDynamicString* root, palDynamicString* extension);

  static void Build(palDynamicString* path, ...);
};

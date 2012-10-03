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

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"
#include "libpal/pal_mem_blob.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include "libpal/windows/pal_file_windows.h"
#elif defined(PAL_PLATFORM_NACL)
#include "libpal/posix/pal_file_posix.h"
#else
#error Need to implement palFile classes for this platform
#endif

enum palFileSeekOrigin {
  kFileSeekBegin = 0,
  kFileSeekCurrent = 1,
  kFileSeekEnd = 2,
  kFileSeekOriginNUM = 3,
};

enum palFileAccess {
  kFileAccessRead = 0,
  kFileAccessWrite = 1,
  kFileAccessReadWrite = 2,
  NUM_palFileAccess
};

enum palFileMode {
  kFileModeCreateNew = 0,
  kFileModeCreate = 1,
  kFileModeOpen = 2,
  kFileModeOpenOrCreate = 3,
  kFileModeTruncate = 4,
  kFileModeAppend = 5,
  NUM_palFileModes
};

#define PAL_FILE_ERROR_OPENNING palMakeErrorCode(0xcd, 1)

class palFile {
  PAL_DISALLOW_COPY_AND_ASSIGN(palFile);
  palFilePlatformData _pdata;
  palFileMode _mode;
  palFileAccess _access;
public:
  palFile();
  ~palFile();

  int Open(const char* filename, palFileMode mode, palFileAccess access);
  int OpenForReading(const char* filename);
  int OpenForWritingTruncate(const char* filename);
  
  void Close();
  bool IsOpen();

  palFileMode GetMode();
  palFileAccess GetAccess();

  void CopyContentsAsString(palMemBlob* blob);
  void CopyContents(palMemBlob* blob);

  uint64_t Seek(palFileSeekOrigin p, int64_t offset);
  uint64_t Read(void* buffer, size_t num_bytes);
  uint64_t Write(const void* buffer, size_t num_bytes);
  uint64_t Write(const char* str);
  uint64_t WritePrintf(const char* str, ...);

  uint64_t OffsetRead(uint64_t offset, void* buffer, size_t num_bytes);
  uint64_t OffsetWrite(uint64_t offset, const void* buffer, size_t num_bytes);

  uint64_t GetPosition();
  uint64_t GetSize();

  uint64_t SetFileSize(uint64_t new_size);

  void Flush();

  static int CopyFileContentsAsString(const char* filename, palMemBlob* blob);
  static int CopyFileContents(const char* filename, palMemBlob* blob);
  static void FreeFileContents(palMemBlob* blob);
  static int WriteFileContents(const char* filename, palMemBlob blob);
};

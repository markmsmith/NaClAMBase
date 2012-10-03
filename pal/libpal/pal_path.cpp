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

#include "libpal/pal_path.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include <windows.h>
#define MIX_TIME(time) ((uint64_t)time.dwHighDateTime << 32 | (uint64_t)time.dwLowDateTime)
#else
#warning Implement palPath for your platform!
#endif

char palPath::GetDirectorySeperator() {
#if defined(PAL_PLATFORM_WINDOWS)
  return '\\';
#else
  return '/';
#endif
}

uint64_t palPath::GetAccessTime(const char* path) {
#if defined(PAL_PLATFORM_WINDOWS)
  HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return 0;
  }
  FILETIME ctime;
  FILETIME atime;
  FILETIME mtime;
  BOOL r;
  r = GetFileTime(hFile, &ctime, &atime, &mtime);
  if (r == 0) {
    return 0;
  }
  return (uint64_t)(MIX_TIME(atime));
#else
  return 0;
#endif
}

uint64_t palPath::GetModificationTime(const char* path) {
#if defined(PAL_PLATFORM_WINDOWS)
  HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return 0;
  }
  FILETIME ctime;
  FILETIME atime;
  FILETIME mtime;
  BOOL r;
  r = GetFileTime(hFile, &ctime, &atime, &mtime);
  if (r == 0) {
    return 0;
  }
  return (uint64_t)(MIX_TIME(mtime));
#else
  return 0;
#endif
}

uint64_t palPath::GetCreationTime(const char* path) {
#if defined(PAL_PLATFORM_WINDOWS)
  HANDLE hFile = CreateFile(path, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
  if (hFile == INVALID_HANDLE_VALUE) {
    return 0;
  }
  FILETIME ctime;
  FILETIME atime;
  FILETIME mtime;
  BOOL r;
  r = GetFileTime(hFile, &ctime, &atime, &mtime);
  if (r == 0) {
    return 0;
  }
  return (uint64_t)(MIX_TIME(ctime));
#else
  return 0;
#endif
}

bool palPath::IsAbsolutePath(const char* path) {
  size_t length = palStringLength(path);
  if (path[0] == GetDirectorySeperator()) {
    return true;
  }
#if defined(PAL_PLATFORM_WINDOWS)
  // Attempt to skip over drive letter
  if (length > 3) {
    if (palIsAlpha(path[0]) && path[1] == ':' && path[2] == GetDirectorySeperator()) {
      return true;
    }
  }
#endif
  return false;
}

bool palPath::Exists(const char* path) {
#if defined(PAL_PLATFORM_WINDOWS)
  DWORD attributes = GetFileAttributes(path);
  return attributes != 0xFFFFFFFF;
#else
  return false;
#endif
}

bool palPath::IsDirectory(const char* path) {
#if defined(PAL_PLATFORM_WINDOWS)
  DWORD attributes = GetFileAttributes(path);
  if (attributes == 0xFFFFFFFF) {
    return false;
  }
  return (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
#else
  return false;
#endif
}

bool palPath::IsFile(const char* path) {
#if defined(PAL_PLATFORM_WINDOWS)
  DWORD attributes = GetFileAttributes(path);
  if (attributes == 0xFFFFFFFF) {
    return false;
  }
  return (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0;
#else
  return false;
#endif
}

bool palPath::Split(const char* path, palDynamicString* head, palDynamicString* tail) {
  size_t index = palStringFindChFromRight(path, GetDirectorySeperator());
  if (index == -1) {
    head->Set(path);
    tail->Set('\0');
    return true;
  }
  head->Set(path, index);
  tail->Set(&path[index]);
  return true;
}

bool palPath::SplitDrive(const char* path, palDynamicString* drive, palDynamicString* tail) {
  size_t length = palStringLength(path);
  if (length < 3) {
    drive->Set('\0');
    tail->Set('\0');
    return false;
  }

  if (palIsAlpha(path[0]) && path[1] == ':') {
    drive->Set(path[0]);
    drive->Append(':');
    tail->Set(&path[2]);
    return true;
  } else {
    drive->Set('\0');
    tail->Set(&path[0]);
    return true;
  }
}

bool palPath::SplitExtension(const char* path, palDynamicString* root, palDynamicString* extension) {
  size_t index = palStringFindChFromRight(path, '.');
  if (index == -1) {
    root->Set(path);
    extension->Set('\0');
    return true;
  }
  root->Set(path, index);
  extension->Set(&path[index]);
  return true;
}

#include <stdarg.h>

void palPath::Build(palDynamicString* path, ...) {
  va_list ap;
  va_start(ap, path);
  
  path->Reset();
  do {
    const char* p_element = va_arg(ap, const char*);
    if (p_element == NULL) {
      break;
    }
    path->Append(GetDirectorySeperator());
    path->Append(p_element);
  } while(true);
}

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


#ifndef LIBPAL_PAL_BINARY_READER_H_
#define LIBPAL_PAL_BINARY_READER_H_

#include "libpal/pal_types.h"
#include "libpal/pal_string.h"

enum palBinaryReaderSeekType {
	kSeekSet,
	kSeekCur,
	kSeekEnd
};

class palBinaryReader {
	const char* buffer_start_;
	const char* buffer_end_;
	const char* buffer_p_;
	size_t buffer_length_;
	bool endian_swap_;
public:
	palBinaryReader();
	void UseReadOnlyBuffer(const char* buffer, size_t length);
	void EnableEndianSwap(bool enabled);

	bool IsEOF() const;

	size_t Seek(ssize_t offset, palBinaryReaderSeekType seek_type);
	bool Read(size_t length, void* r);
	bool ReadInt(int* r);
	bool ReadUint32(uint32_t* r);
	bool ReadBool(bool* r);
	bool ReadFloat(float* r);
	bool ReadDouble(double* r);
	bool ReadChar(char* r);
	bool FetchString(palDynamicString* string);
	bool FetchFixedLengthString(size_t length, palDynamicString* string);
};

#endif  // LIBPAL_PAL_BINARY_READER_H_
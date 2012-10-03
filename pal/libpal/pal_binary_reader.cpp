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

#include "libpal/pal_binary_reader.h"
#include "pal_memory.h"

palBinaryReader::palBinaryReader()
{
	buffer_start_ = NULL;
	buffer_length_ = 0;
	buffer_p_ = NULL;
	buffer_end_ = NULL;
	endian_swap_ = false;
}

void palBinaryReader::UseReadOnlyBuffer( const char* buffer, size_t length )
{
	buffer_start_ = buffer;
	buffer_length_ = length;
	buffer_p_ = buffer_start_;
	buffer_end_ = buffer_start_ + length;
}

void palBinaryReader::EnableEndianSwap( bool enabled )
{
	endian_swap_ = enabled;
}

bool palBinaryReader::IsEOF() const
{
	return buffer_p_ == buffer_end_;
}

size_t palBinaryReader::Seek(ssize_t offset, palBinaryReaderSeekType seek_type) {
	size_t current_offset = static_cast<uint32_t>(buffer_p_ - buffer_start_);
	switch (seek_type)
	{
	case kSeekCur:
		if (static_cast<ssize_t>(current_offset + offset) < 0) {
			// error
			return static_cast<uint32_t>(-1);
		} else if (static_cast<size_t>(current_offset + offset) >= buffer_length_) {
			// error
			return static_cast<uint32_t>(-1);
		}
		buffer_p_ = buffer_start_ + (current_offset + offset);
	break;
	case kSeekSet:
		if (offset < 0 || (size_t)offset >= buffer_length_) {
			// error
			return static_cast<uint32_t>(-1);
		}
		buffer_p_ = buffer_start_ + offset;
	break;
	case kSeekEnd:
		if (static_cast<ssize_t>(buffer_length_ + offset) < 0) {
			// error
			return static_cast<uint32_t>(-1);
		} else if (static_cast<size_t>(buffer_length_ + offset) >= buffer_length_) {
			// error
			return static_cast<uint32_t>(-1);
		}
		buffer_p_ = buffer_start_ + (buffer_length_ + current_offset);
	break;
	}
	return buffer_p_ - buffer_start_;
}

bool palBinaryReader::Read(size_t length, void* r) {
	if (buffer_p_ + length >= buffer_end_) {
		// EOF
		return false;
	}
	palMemoryCopyBytes(r, buffer_p_, length);
	buffer_p_ += length;
	return true;
}

bool palBinaryReader::ReadInt(int* r)
{
	if (buffer_p_ + sizeof(int) >= buffer_end_) {
		// EOF
		return false;
	}
	const int* int_buffer = reinterpret_cast<const int*>(buffer_p_);
	*r = *int_buffer;
	buffer_p_ += sizeof(int);
	return true;
}

bool palBinaryReader::ReadUint32(uint32_t* r)
{
	if (buffer_p_ + sizeof(uint32_t) >= buffer_end_) {
		// EOF
		return false;
	}
	const uint32_t* uint32_t_buffer = reinterpret_cast<const uint32_t*>(buffer_p_);
	*r = *uint32_t_buffer;
	buffer_p_ += sizeof(uint32_t);
	return true;
}

bool palBinaryReader::ReadBool(bool* r)
{
	if (buffer_p_ + sizeof(bool) >= buffer_end_) {
		// EOF
		return false;
	}
	const bool* bool_buffer = reinterpret_cast<const bool*>(buffer_p_);
	*r = *bool_buffer;
	buffer_p_ += sizeof(bool);
	return true;
}

bool palBinaryReader::ReadFloat(float* r)
{
	if (buffer_p_ + sizeof(float) >= buffer_end_) {
		// EOF
		return false;
	}
	const float* float_buffer = reinterpret_cast<const float*>(buffer_p_);
	*r = *float_buffer;
	buffer_p_ += sizeof(float);
	return true;
}

bool palBinaryReader::ReadDouble(double* r)
{
	if (buffer_p_ + sizeof(double) >= buffer_end_) {
		// EOF
		return false;
	}
	const double* double_buffer = reinterpret_cast<const double*>(buffer_p_);
	*r = *double_buffer;
	buffer_p_ += sizeof(double);
	return true;
}

bool palBinaryReader::ReadChar(char* r)
{
	if (buffer_p_ + sizeof(char) >= buffer_end_) {
		// EOF
		return false;
	}
	const char* char_buffer = reinterpret_cast<const char*>(buffer_p_);
	*r = *char_buffer;
	buffer_p_ += sizeof(char);
	return true;
}

bool palBinaryReader::FetchString(palDynamicString* string)
{
	const char* start = buffer_p_;
	while (*buffer_p_ != '\0' && IsEOF() == false) {
		buffer_p_++;
		continue;
	}
	if (start == buffer_p_)
		return false;
	size_t length = buffer_p_ - start;
  string->SetLength(0);
  string->Append(start, length);
	return true;
}

bool palBinaryReader::FetchFixedLengthString(size_t length, palDynamicString* string)
{
	const char* start = buffer_p_;
  string->SetLength(0);
	if (buffer_p_ + length >= buffer_end_) {
		// EOF
		return false;
	}
  string->Append(start, length);
  return true;
}
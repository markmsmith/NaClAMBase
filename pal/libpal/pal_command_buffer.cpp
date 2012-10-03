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

#include "libpal/pal_memory.h"
#include "libpal/pal_command_buffer.h"

palCommandBuffer::palCommandBuffer(uint8_t* buffer, uint32_t buffer_size) {
  buffer_ = buffer;
  buffer_size_ = buffer_size_;
  write_cursor_ = 0;
}

int palCommandBuffer::AppendCommand(palCommandHeader* header, void* payload) {
  const uint32_t total_size = header->payload_size + sizeof(header);
  if (total_size + write_cursor_ >= buffer_size_) {
    return -1;
  }
  palMemoryCopyBytes(&buffer_[write_cursor_], header, sizeof(header));
  write_cursor_ += sizeof(header);
  palMemoryCopyBytes(&buffer_[write_cursor_], payload, header->payload_size);
  write_cursor_ += header->payload_size;
  return 0;
}

int palCommandBuffer::BeginCommandProcessing(uint8_t** buffer, uint32_t* buffer_length) {
  *buffer = buffer_;
  *buffer_length = write_cursor_;
  return 0;
}

int palCommandBuffer::FinishCommandProcessing() {
  write_cursor_ = 0;
  return 0;
}
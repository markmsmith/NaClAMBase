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
#include "libpal/pal_errorcode.h"
#include "libpal/pal_types.h"
#include "libpal/pal_scalar.h"
#include "libpal/pal_simd.h"
#include "libpal/pal_timer.h"
#include "libpal/pal_atomic.h"
#include "libpal/pal_align.h"
#include "libpal/pal_debug.h"
#include "libpal/pal_string.h"
#include "libpal/pal_random.h"
#include "libpal/pal_thread.h"
#include "libpal/pal_file.h"
#include "libpal/pal_endian.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_page_allocator.h"
#include "libpal/pal_heap_allocator.h"
#include "libpal/pal_blob_array.h"
#include "libpal/pal_array.h"
#include "libpal/pal_min_heap.h"
#include "libpal/pal_image.h"
#include "libpal/pal_hash_functions.h"
#include "libpal/pal_hash_map.h"
#include "libpal/pal_hash_map_cache.h"
#include "libpal/pal_hash_set.h"
#include "libpal/pal_hash_functions.h"
#include "libpal/pal_list.h"
#include "libpal/pal_ilist.h"
#include "libpal/pal_tokenizer.h"
#include "libpal/pal_compacting_allocator.h"
#include "libpal/pal_allocator.h"
#include "libpal/pal_font_rasterizer_stb.h"
#include "libpal/pal_font_rasterizer_freetype.h"
#include "libpal/pal_utf8.h"
#include "libpal/pal_profiler.h"
#include "libpal/pal_delegate.h"
#include "libpal/pal_event.h"
#include "libpal/pal_time_line.h"
#include "libpal/pal_json.h"
#include "libpal/pal_object_id_table.h"
#include "libpal/pal_socket.h"
#include "libpal/pal_tcp_client.h"
#include "libpal/pal_tcp_listener.h"
#include "libpal/pal_web_socket_server.h"
#include "libpal/pal_md5.h"
#include "libpal/pal_command_buffer.h"
#include "libpal/pal_adi.h"
#include "libpal/pal_mem_blob.h"
#include "libpal/pal_frame_clock.h"
#include "libpal/pal_process.h"
#include "libpal/pal_pipe_stream.h"

int palStartup(palConsolePrintFunction print_func);
int palShutdown();
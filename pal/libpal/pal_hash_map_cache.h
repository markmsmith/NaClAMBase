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


#include "libpal/pal_debug.h"
#include "libpal/pal_random.h"
#include "libpal/pal_array.h"
#include "libpal/pal_hash_map.h"

template <class Key, class Value, class HashFunction = palHashFunction<Key>, class KeyEqual = palHashEqual<Key> >
class palHashMapCache
{
protected:
	mutable int _cacheSize;
	mutable int _cache_queries;
	mutable int _cache_hits;
	mutable int _cache_ejects;

	palHashMap<Key, Value, HashFunction, KeyEqual> _map;
public:
	palHashMapCache(int cacheSize) : _cacheSize(cacheSize), _cache_queries(0), _cache_hits(0), _cache_ejects(0), _map() {
	}

  void SetAllocator(palAllocatorInterface* allocator) {
    _map.SetAllocator(allocator);
  }

	int GetIndex(const Key& key) {
		_cache_queries++;
		return _map.FindIndex(key);
	}

	bool IsValidIndex(int index) {
		return index != kPalHashNULL;
	}

	void Cache(const Key& key, const Value& value)
	{
		if (_map.GetSize() > _cacheSize) {
			_map.EraseRandomEntry();
			_cache_ejects++;
		}
		_map.Insert(key, value);
	}

	void Clear() {
    ClearStatistics();
		_map.Clear();
	}

  void Reset() {
    ClearStatistics();
    _map.Reset();
  }

  void ClearStatistics() {
    _cache_queries = 0;
    _cache_hits = 0;
    _cache_ejects = 0;
  }

	Value* GetValueAtIndex(int index) {
		_cache_hits++;
		return _map.GetValueAtIndex(index);
	}

  const Value* GetValueAtIndex(int index) const {
    _cache_hits++;
    return _map.GetValueAtIndex(index);
  }

	int GetCacheQueryStat() const {
		return _cache_queries;
	}

	int GetCacheHitStat() const {
		return _cache_hits;
	}

	int GetCacheEjectStat() const {
		return _cache_ejects;
	}
};

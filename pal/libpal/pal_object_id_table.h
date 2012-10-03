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

#ifndef LIBPAL_PAL_OBJECT_ID_TABLE_H_
#define LIBPAL_PAL_OBJECT_ID_TABLE_H_

/*
  Maps 32-bit integer IDs to objects

  Template takes object type (T) and maximum number of objects that can be stored as a power of two (kPowTwoObjectsInTable)

  The lower bits represent the index into the table and the higher bits are a generation flag

  The generation flag is used to detect the case when an object at index K was removed and a new object
  at index K was added. In that case, NULL is returned.

  Complexity:

  Add new object
    O(1)
  Delete object by ID
    O(1)
  Fetch object pointer from ID
    O(1)
  Find object ID
    O(N)
  Iterate over all objects
    O(N)

  N = 2^kPowTwoObjectsInTable
*/
 
// The table supports 2^kPowTwoObjectsInTable
template<typename T, int kPowTwoObjectsInTable = 8>
class palObjectIdTable {
protected:

  uint32_t generation_count_;
  uint32_t next_available_slot_;
  uint32_t object_id_mask_;
  uint32_t generation_mask_;
  uint32_t object_count_;
  uint32_t highwater_mark_;

  T* object_lookup_[1 << kPowTwoObjectsInTable];
  uint32_t generation_lookup_[1 << kPowTwoObjectsInTable];
public:
  palObjectIdTable() {
    uint32_t max_objects = 1 << kPowTwoObjectsInTable;

    generation_count_ = 0;
    object_id_mask_ = (max_objects)-1;
    generation_mask_ = ~object_id_mask_ >> kPowTwoObjectsInTable;
    highwater_mark_ = 0;
    object_count_ = 0;

    /* When an object is NULL, the generation key is used to index the next
       free slot
     */
    next_available_slot_ = 0;
    for (uint32_t i = 0; i < max_objects; i++) {
      generation_lookup_[i] = i+1;
      object_lookup_[i] = NULL;
    }
  }

  uint32_t GetCapacity() {
    uint32_t max_objects = 1 << kPowTwoObjectsInTable;
    return max_objects;
  }

  uint32_t GetSize() {
    return object_count_;
  }

  uint32_t GetHighWaterMark() {
    return highwater_mark_;
  }

  uint32_t AddObject(T* o) {
    palAssert(o != NULL);
    uint32_t max_objects = 1 << kPowTwoObjectsInTable;
    if (next_available_slot_ == max_objects) {
      return max_objects;
    }

    generation_count_++;
    if (generation_count_ == 1) {
      // never allow a generation of 1
      generation_count_++;
    }

    uint32_t id = next_available_slot_;
    next_available_slot_ = generation_lookup_[id];

    generation_lookup_[id] = generation_count_;
    palAssert(object_lookup_[id] == NULL);
    object_lookup_[id] = o;

    object_count_++;
    if (object_count_ > highwater_mark_) {
      highwater_mark_ = object_count_;
    }

    return (generation_count_ << kPowTwoObjectsInTable)|id;
  }

  T* RemoveObject(uint32_t id) {
    uint32_t generation = (id >> kPowTwoObjectsInTable) & generation_mask_;
    uint32_t index = id & object_id_mask_;
    if (object_lookup_[index] != NULL && generation_lookup_[index] == generation) {
      // correct object
      T* r = object_lookup_[index];
      object_lookup_[index] = NULL;
      generation_lookup_[index] = next_available_slot_;
      next_available_slot_ = index;
      object_count_--;
      return r;
    }
    // not actually in the table
    return NULL;
  }

  T* MapObject(uint32_t id) {
    uint32_t generation = (id >> kPowTwoObjectsInTable) & generation_mask_;
    uint32_t index = id & object_id_mask_;
    if (object_lookup_[index] != NULL && generation_lookup_[index] == generation) {
      return object_lookup_[index];
    }
    return NULL;
  }

  uint32_t FindObjectID(T* o) {
    uint32_t max_objects = 1 << kPowTwoObjectsInTable;
    palAssert(o != NULL);
    for (uint32_t i = 0; i < max_objects; i++) {
      if (object_lookup_[i] == o) {
        return (generation_lookup_[i] << kPowTwoObjectsInTable) | i;
      }
    }
    return max_objects;
  }

  /* Iteration */
  uint32_t GetFirstObjectID() {
    uint32_t max_objects = 1 << kPowTwoObjectsInTable;
    for (uint32_t i = 0; i < max_objects; i++) {
      if (object_lookup_[i] != NULL) {
        return (generation_lookup_[i] << kPowTwoObjectsInTable) | i;
      }
    }
    return max_objects;
  }

  uint32_t GetNextObjectID(uint32_t id) {
    uint32_t max_objects = 1 << kPowTwoObjectsInTable;
    uint32_t index = id & object_id_mask_;
    for (uint32_t i = index+1; i < max_objects; i++) {
      if (object_lookup_[i] != NULL) {
        return (generation_lookup_[i] << kPowTwoObjectsInTable) | i;
      }
    }
    return max_objects;
  }
};

#endif  // LIBPAL_PAL_OBJECT_ID_TABLE_H_
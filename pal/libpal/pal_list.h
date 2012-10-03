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

#pragma once

#include "libpal/pal_pool_allocator.h"

template <typename T>
struct palListNode {
  // data goes first to make alignment easier
  T data;
  palListNode<T>* next;
  palListNode<T>* prev;

  palListNode() : next(NULL), prev(NULL) {
  }

  explicit palListNode(const T& node_data) : next(NULL), prev(NULL), data(node_data) {
  }
};

template <typename T, uint32_t Alignment = PAL_ALIGNOF(T)>
class palList {
 public:
   /* Types and constants */
   typedef palList<T, Alignment> this_type;
   typedef T element_type;
   static const uint32_t element_size = sizeof(palListNode<T>);
   static const uint32_t element_alignment = Alignment; 
 private:
  palAllocatorInterface* allocator_;
  palListNode<T> root_;

  PAL_DISALLOW_COPY_AND_ASSIGN(palList);
 public:
  palList() {
    // circular
    root_.next = &root_;
    root_.prev = &root_;
    allocator_ = NULL;
  }

  ~palList() {
    if (IsEmpty() == false) {
      // erase all nodes
      Erase(GetFirst(), GetLast());
    }
  }

  void SetAllocator(palAllocatorInterface* allocator) {
    allocator_ = allocator;
  }

  palAllocatorInterface* GetAllocator() const {
    return allocator_;
  }

  void PushFront(const T& item) {
    palListNode<T>* node = reinterpret_cast<palListNode<T>*>(allocator_->Allocate(sizeof(palListNode<T>), element_alignment));
    new (node) palListNode<T>(item);
    AddHead(node);
  }

  void PushBack(const T& item) {
    palListNode<T>* node = reinterpret_cast<palListNode<T>*>(allocator_->Allocate(sizeof(palListNode<T>), element_alignment));
    new (node) palListNode<T>(item);
    AddTail(node);
  }

  void PopFront() {
    palListNode<T>* node = PopHead();
    allocator_->Deallocate(node);
  }

  void PopBack() {
    palListNode<T>* node = PopTail();
    allocator_->Deallocate(node);
  }

  void InsertBefore(const T& item, palListNode<T>* node) {
    palListNode<T>* new_node = reinterpret_cast<palListNode<T>*>(allocator_->Allocate(sizeof(palListNode<T>), element_alignment));
    new (new_node) palListNode<T>(item);
    Add(new_node, node->prev, node);
  }

  void InsertAfter(const T& item, palListNode<T>* node) {
    palListNode<T>* new_node = reinterpret_cast<palListNode<T>*>(allocator_->Allocate(sizeof(palListNode<T>), element_alignment));
    new (new_node) palListNode<T>(item);
    Add(new_node, node, node->next);
  }

  void Erase(palListNode<T>* node) {
    RemoveNode(node);
    allocator_->Deallocate(node);
  }

  void Erase(palListNode<T>* begin, palListNode<T>* end) {
    palListNode<T>* node = begin;
    do {
      begin = node->next;
      RemoveNode(node);
      allocator_->Deallocate(node);
      if (node == end) {
        break;
      }
      node = begin;
    } while(true);
  }

  void RemoveFirst(const T& item) {
    palListNode<T>* node = GetFirst();

    if (IsEmpty()) {
      return;
    }

    do {
      palListNode<T>* next = node->next;
      bool over = IsLast(node);
      if (node->data == item) {
        // remove it
        Erase(node);
        break;
      }
      if (over) {
        break;
      }
      node = next;
    } while(true);
  }

  void Remove(const T& item) {
    palListNode<T>* node = GetFirst();

    if (IsEmpty()) {
      return;
    }

    do {

      palListNode<T>* next = node->next;

      bool over = IsLast(node);

      if (node->data == item) {
        // remove it
        Erase(node);
      }

      if (over) {
        break;
      }

      node = next;
    } while(true);
  }

  // Get first node
  palListNode<T>* GetFirst() const {
    return root_.next;
  }

  // Get last node
  palListNode<T>* GetLast() const {
    return root_.prev;
  }
  // Is first node
  bool IsFirst(const palListNode<T>* node) const {
    return node->prev == &root_;
  }
  // Is last node
  bool IsLast(const palListNode<T>* node) const {
    return node->next == &root_;
  }

  // is empty list
  bool IsEmpty() const {
    return root_.next == &root_;
  }

  int Length() const {
    int length = 0;
    palListNode<T>* node = root_.next;
    while (IsRoot(node) == false) {
      length++;
    }
    return length;
  }

  // Splice list after node
  void Splice(this_type* list, palListNode<T>* node) {
    palListNode<T>* first = list->GetFirst();
    palListNode<T>* last = list->GetLast();
    palListNode<T>* link = node->next;

    first->prev = node;
    node->next = first;

    last->next = link;
    link->prev = last;

    // list is now empty, reset
    list->root_.next = &list->root_;
    list->root_.prev = &list->root_;
  }

  // Splices list to beginning of list
  void SpliceHead(this_type* list) {
    if (list->IsEmpty() == false) {
      Splice(list, &root_);
    }
  }

  // Splices list to end of list
  void SpliceTail(this_type* list) {
    if (list->IsEmpty() == false) {
      Splice(list, root_.prev);
    }
  }

  // Cuts this[first...last] out of this and puts it into sublist
  void MakeSublist(this_type* sublist, palListNode<T>* first, palListNode<T>* last) {
    // cut nodes out of list
    first->prev->next = last->next;
    last->next->prev = first->prev;

    // make sublist
    sublist->root_.next = first;
    sublist->root_.prev = last;
    first->prev = &sublist->root_;
    last->next = &sublist->root_;
  }
  
  void dumpListForwardDebug(const palList<int>& l)
  {
    palListNode<int>* current;
    
    if (l.IsEmpty())
      return;
    
    current = l.GetFirst();
    do 
    {
      palPrintf("%d\n", current->data);
      if (l.IsLast(current))
        break;
      current = current->next;
    } while (true);
    palPrintf("--\n");
  }
  
  template <typename CF>
  void Sort(CF CompareFunc) {
    palListNode<T>* p = NULL;
    palListNode<T>* q = NULL;
    palListNode<T>* e = NULL;
    palListNode<T>* tail = NULL;
    palListNode<T>* list = NULL;
    
    int insize = 0;
    int nmerges = 0;
    int psize = 0;
    int qsize = 0;
    int i = 0;
    
    if (IsEmpty()) {
      // empty list
      return;
    }
      
    
    list = root_.next; // first actual element of the list
    
    insize = 1;
    
    while (1) {
      p = list;
      list = NULL;
      tail = NULL;
      
      nmerges = 0;  /* count number of merges we do in this pass */
      
      while (p) {
        nmerges++;  /* there exists a merge to be done */
        
        /* step `insize' places along from p */
        q = p;
        psize = 0;
        
        for (i = 0; i < insize; i++) {
          psize++;
          q = q->next;
          if (q == &root_) {
            // hit the end of the list
            q = NULL;
          }
          if (!q) {
           break; 
          }
        }
        
        /* if q hasn't fallen off end, we have two lists to merge */
        qsize = insize;
        
        /* now we have two lists; merge them */
        while (psize > 0 || (qsize > 0 && q)) {
          
          /* decide whether next element of merge comes from p or q */
          if (psize == 0) {
            /* p is empty; e must come from q. */
            e = q;
            q = q->next;
            qsize--;
            if (q == &root_) {
              q = NULL;
            }
          } else if (qsize == 0 || !q) {
            /* q is empty; e must come from p. */
            e = p;
            p = p->next;
            psize--;
            if (p == &root_) {
              p = NULL;
            }
          } else if (CompareFunc(p,q) <= 0) {
            /* First element of p is lower (or same);
             * e must come from p. */
            e = p;
            p = p->next;
            psize--;
            if (p == &root_) {
              p = NULL;
            }
          } else {
            /* First element of q is lower; e must come from q. */
            e = q;
            q = q->next;
            qsize--;
            if (q == &root_) {
              q = NULL;
            }
          }
          
          /* add the next element to the merged list */
          if (tail) {
            tail->next = e;
            e->prev = tail;
          } else {
            list = e;
          }
          
          tail = e;          
        }
        
        /* now p has stepped `insize' places along, and q has too */
        p = q;
      }
      
      root_.next = list;
      root_.prev = tail;
      list->prev = &root_;
      tail->next = &root_;
      
      //dumpListForwardDebug(*this);
      
      /* If we have done only one merge, we're finished. */
      if (nmerges <= 1)
        return;
      
      /* Otherwise repeat, merging lists twice the size */
      insize *= 2;
    }
  }
protected:
  // Add node between next and previous
  void Add(palListNode<T>* node, palListNode<T>* prev, palListNode<T>* next) {
    // adjust new pointers
    node->next = next;
    node->prev = prev;

    // insert into existing list
    next->prev = node;
    prev->next = node;
  }

  // Add to head of list
  void AddHead(palListNode<T>* node) {
    Add(node, &root_, root_.next);
  }

  // Add to tail of list
  void AddTail(palListNode<T>* node) {
    Add(node, root_.prev, &root_);
  }

  // Remove node
  void RemoveNode(palListNode<T>* node) {
    // node is inserted in a list
    palAssert(node->next != NULL);
    palAssert(node->prev != NULL);
    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next = NULL;
    node->prev = NULL;
  }

  // Removes first node
  palListNode<T>* PopHead() {
    if (IsEmpty())
      return NULL;

    palListNode<T>* popped = root_.next;

    RemoveNode(popped);

    return popped;
  }

  // Removes last node
  palListNode<T>* PopTail() {
    if (IsEmpty())
      return NULL;

    palListNode<T>* popped = root_.prev;

    RemoveNode(popped);

    return popped;
  }

  // Replace the obsolete node with the replacement node
  void Replace(palListNode<T>* obsolete, palListNode<T>* replacement) {
    replacement->next = obsolete->next;
    replacement->prev = obsolete->prev;
  }


  // Is this the root (dead) node
  bool IsRoot(const palListNode<T>* node) const {
    return node == &root_;
  }

  palListNode<T>* GetRoot() {
    return &root_;
  }
};
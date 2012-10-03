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

#ifndef LIBPAL_PAL_ILIST_H__
#define LIBPAL_PAL_ILIST_H__

/* Intrusive lists: The list node is placed inside the container class.
   That means no memory allocation required for nodes. Better cache performance.

   References:

   Linux Kernel
   http://locklessinc.com/articles/flexible_lists_in_cpp/

*/

#include "libpal/pal_debug.h"
#include "libpal/pal_types.h"

#define palIListNode_offsetof(Type, MemberName) (reinterpret_cast<size_t>(&reinterpret_cast<char &>(reinterpret_cast<Type *> (1)->MemberName))- 1)

#define palIListNodeDeclare(Type, MemberName)\
  static size_t offset_##MemberName(void) { return palIListNode_offsetof(Type, MemberName); }\
  palIListNode MemberName

#define palIListForeachDeclare(Type, MemberName) palIListForeach<Type , Type::offset_##MemberName>

#define palIListNodeValue(node, Type, MemberName) reinterpret_cast<Type*>(reinterpret_cast<unsigned char*>(node) - Type::offset_##MemberName())

#define palIListSorterDeclare(Type, MemberName) palIListSorter<Type, Type::offset_##MemberName>


struct palIListNode {
  palIListNode* next;
  palIListNode* prev;

  palIListNode() : next(NULL), prev(NULL) {
  }
};

class palIList;

class palIList {
  palIListNode root_;

 public:
  palIList() {
    // circular
    root_.next = &root_;
    root_.prev = &root_;
  }

  // Add node between next and previous
  void Add(palIListNode* node, palIListNode* prev, palIListNode* next) {
    // adjust new pointers
    node->next = next;
    node->prev = prev;

    // insert into existing list
    next->prev = node;
    prev->next = node;
  }

  // Add to head of list
  void AddHead(palIListNode* node) {
    Add(node, &root_, root_.next);
  }

  // Add to tail of list
  void AddTail(palIListNode* node) {
    Add(node, root_.prev, &root_);
  }

  // Remove node
  void Remove(palIListNode* node) {
    // node is inserted in a list
    palAssert(node->next != NULL);
    palAssert(node->prev != NULL);
    node->next->prev = node->prev;
    node->prev->next = node->next;
    node->next = NULL;
    node->prev = NULL;
  }

  // Removes first node
  palIListNode* PopHead() {
    if (IsEmpty())
      return NULL;

    palIListNode* popped = root_.next;

    Remove(popped);

    return popped;
  }

  // Removes last node
  palIListNode* PopTail() {
    if (IsEmpty())
      return NULL;

    palIListNode* popped = root_.prev;

    Remove(popped);

    return popped;
  }

  // Replace the obsolete node with the replacement node
  void Replace(palIListNode* obsolete, palIListNode* replacement) {
    replacement->next = obsolete->next;
    replacement->prev = obsolete->prev;
  }

  // Get first node
  palIListNode* GetFirst() const {
    return root_.next;
  }

  // Get last node
  palIListNode* GetLast() const {
    return root_.prev;
  }

  // Is this the root (dead) node
  bool IsRoot(const palIListNode* node) const {
    return node == &root_;
  }

  palIListNode* GetRoot() {
    return &root_;
  }

  // Is first node
  bool IsFirst(const palIListNode* node) const {
    return node->prev == &root_;
  }
  // Is last node
  bool IsLast(const palIListNode* node) const {
    return node->next == &root_;
  }

  // is empty list
  bool IsEmpty() const {
    return root_.next == &root_;
  }

  int Length() const {
    int length = 0;
    palIListNode* node = root_.next;
    while (IsRoot(node) == false) {
      length++;
    }
    return length;
  }

  // Splice list after node
  void Splice(palIList* list, palIListNode* node) {
    palIListNode* first = list->GetFirst();
    palIListNode* last = list->GetLast();
    palIListNode* link = node->next;

    first->prev = node;
    node->next = first;

    last->next = link;
    link->prev = last;

    // list is now empty, reset
    list->root_.next = &list->root_;
    list->root_.prev = &list->root_;
  }

  // Splices list to beginning of list
  void SpliceHead(palIList* list) {
    if (list->IsEmpty() == false) {
      Splice(list, &root_);
    }
  }

  // Splices list to end of list
  void SpliceTail(palIList* list) {
    if (list->IsEmpty() == false) {
      Splice(list, root_.prev);
    }
  }

  // Cuts this[first...last] out of this and puts it into sublist
  void MakeSublist(palIList* sublist, palIListNode* first, palIListNode* last) {
    // cut nodes out of list
    first->prev->next = last->next;
    last->next->prev = first->prev;

    // make sublist
    sublist->root_.next = first;
    sublist->root_.prev = last;
    first->prev = &sublist->root_;
    last->next = &sublist->root_;
  }
};

template <typename T, size_t offset(void)>
class palIListSorter {
  palIList* list_;
public:
  palIListSorter(palIList* list) : list_(list) {
  }

  template <typename CF>
  void Sort(CF CompareFunc) {
    palIListNode* p = NULL;
    palIListNode* q = NULL;
    palIListNode* e = NULL;
    palIListNode* tail = NULL;
    palIListNode* list = NULL;
     
    int insize = 0;
    int nmerges = 0;
    int psize = 0;
    int qsize = 0;
    int i = 0;
    
    if (list_->IsEmpty()) {
      // empty list
      return;
    }

    palIListNode* root = list_->GetRoot();

    list = list_->GetFirst(); // first actual element of the list
    
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
          if (list_->IsRoot(q)) {
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
          
          const T* p_value = reinterpret_cast<const T*>(reinterpret_cast<unsigned char*>(p) - offset());
          const T* q_value = reinterpret_cast<const T*>(reinterpret_cast<unsigned char*>(q) - offset());
          /* decide whether next element of merge comes from p or q */
          if (psize == 0) {
            /* p is empty; e must come from q. */
            e = q;
            q = q->next;
            qsize--;
            if (list_->IsRoot(q)) {
              q = NULL;
            }
          } else if (qsize == 0 || !q) {
            /* q is empty; e must come from p. */
            e = p;
            p = p->next;
            psize--;
            if (list_->IsRoot(p)) {
              p = NULL;
            }
          } else if (CompareFunc(p_value,q_value) <= 0) {
            /* First element of p is lower (or same);
             * e must come from p. */
            e = p;
            p = p->next;
            psize--;
            if (list_->IsRoot(p)) {
              p = NULL;
            }
          } else {
            /* First element of q is lower; e must come from q. */
            e = q;
            q = q->next;
            qsize--;
            if (list_->IsRoot(q)) {
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

      
      root->next = list;
      root->prev = tail;
      list->prev = root;
      tail->next = root;
      
      //dumpListForwardDebug(*this);
      
      /* If we have done only one merge, we're finished. */
      if (nmerges <= 1)
        return;
      
      /* Otherwise repeat, merging lists twice the size */
      insize *= 2;
    }
  }
};

template <typename T, size_t offset(void)>
class palIListForeach {
  palIList* list_;
  palIListNode* current;
  
  T* list_entry(palIListNode* node) {
    return reinterpret_cast<T*>((size_t)node - offset());
  }
public:
  palIListForeach(palIList* list) : list_(list) {
    current = list_->GetFirst();
  }
  
  T* GetListEntry() { return list_entry(current); }
  
  bool Finished() const {
    return list_->IsRoot(current);
  }
  
  void AddAfter(palIListNode* node) {
    palIListNode* next = current->next;
    list_->Add(node, current, next);
  }

  void Remove() {
    palIListNode* safe_next = current->next;
    list_->Remove(current);
    current = safe_next;
  }

  void First() {
    current = list_->GetFirst();
  }

  void Last() {
    current = list_->GetLast();
  }

  void Next() {
    current = current->next;
  }
  
  void Prev() {
    current = current->prev;
  }
};


#endif  // LIBPAL_PAL_ILIST_H__

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

#include "libpal/pal_list.h"
#include "libpal/pal_delegate.h"

struct palEventDelegate {
  /*
   IMPORTANT: Make sure that PAL_DELEGATE_SIZE
   is correct for your compiler/platform
  */
  char buffer[PAL_DELEGATE_SIZE];
  template <typename DT>
  DT* Cast() {
    return reinterpret_cast<DT*>(&buffer[0]);
  }

  bool operator==(const palEventDelegate& b) {
    return buffer[0] == b.buffer[0] &&
          buffer[1] == b.buffer[1] &&
          buffer[2] == b.buffer[2] &&
          buffer[3] == b.buffer[3] &&
          buffer[4] == b.buffer[4] &&
          buffer[5] == b.buffer[5] &&
          buffer[6] == b.buffer[6] &&
          buffer[7] == b.buffer[7];
  }
};

extern palAllocatorInterface* g_palEventDelegateAllocator;

template<typename Signature>
class palEvent;

template<typename R>
class palEvent<R ( ) > {
public:
  typedef palDelegate<R ()> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)()) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)() const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)()) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)()) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)() const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)()) {
    Unregister(DelegateType(func));
  }

  void Fire() const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)();
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1>
class palEvent<R ( Param1 ) > {
public:
  typedef palDelegate<R (Param1)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2>
class palEvent<R ( Param1, Param2 ) > {
public:
  typedef palDelegate<R (Param1, Param2)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2, typename Param3>
class palEvent<R ( Param1, Param2, Param3 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2, p3);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4>
class palEvent<R ( Param1, Param2, Param3, Param4 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2, p3, p4);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2, p3, p4, p5);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2, p3, p4, p5, p6);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6, typename Param7>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2, p3, p4, p5, p6, p7);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6, typename Param7, typename Param8>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2, p3, p4, p5, p6, p7, p8);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

template<typename R, typename Param1, typename Param2, typename Param3, typename Param4, typename Param5, typename Param6, typename Param7, typename Param8, typename Param9>
class palEvent<R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Param9 ) > {
public:
  typedef palDelegate<R (Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, Param9)> DelegateType;
protected:
  palList<palEventDelegate> delegates_;
public:
  palEvent() {
    delegates_.SetAllocator(g_palEventDelegateAllocator);
  }
  void Register(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.PushBack(ped);
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Register(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Register(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9) const ) {
    Register(DelegateType(obj, func));
  }

  void Register(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Register(DelegateType(func));
  }

  void Unregister(DelegateType del) {
    palEventDelegate ped;
    *(ped.Cast<DelegateType>()) = del;
    delegates_.RemoveFirst(ped);
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Unregister(DelegateType(obj, func));
  }

  template<typename X, typename Y>
  void Unregister(Y* obj, R (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9) const ) {
    Unregister(DelegateType(obj, func));
  }

  void Unregister(R (*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9)) {
    Unregister(DelegateType(func));
  }

  void Fire(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8, Param9 p9) const {
    palListNode<palEventDelegate>* node = delegates_.GetFirst();
    do 
    {
      DelegateType* del = node->data.Cast<DelegateType>();
      (*del)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
      if (delegates_.IsLast(node)) {
        break;
      }
      node = node->next;
    } while (true);
  }
};

int palEventInit();
int palEventShutdown();
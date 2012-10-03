/*
Copyright (c) 2009 John McCutchan <john@johnmccutchan.com>

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

#ifndef LIBPAL_PAL_DELEGATE_H_
#define LIBPAL_PAL_DELEGATE_H_

#include "libpal/pal_delegate_internal.h"

#if defined(PAL_ARCH_32BIT) && defined(PAL_PLATFORM_WINDOWS)
#define PAL_DELEGATE_SIZE 8
#elif defined(PAL_ARCH_64BIT) && defined(PAL_PLATFORM_WINDOWS)
#define PAL_DELEGATE_SIZE 16
#elif defined(PAL_ARCH_32BIT) && defined(PAL_PLATFORM_APPLE)
#define PAL_DELEGATE_SIZE 8
#elif defined(PAL_ARCH_64BIT) && defined(PAL_PLATFORM_APPLE)
#define PAL_DELEGATE_SIZE 16
#elif defined(PAL_PLATFORM_NACL)
#define PAL_DELEGATE_SIZE 8
#else
#error Figure out platform delegate size
#endif

// Declare delegate as a class template.  It will be specialized
// later for all number of arguments.
template <typename Signature>
class palDelegate;

template<typename R>
class palDelegate< R (	 ) >
  // Inherit from palDelegate0 so that it can be treated just like a palDelegate0
  : public _pal_delegate_internal::palDelegate0 < R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate0 < R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:
  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)(	))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)(  ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)(  ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};

template<typename R, class Param1>
class palDelegate< R ( Param1 ) >
  // Inherit from palDelegate1 so that it can be treated just like a palDelegate1
  : public _pal_delegate_internal::palDelegate1 < Param1, R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate1 < Param1, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:


  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};

template<typename R, class Param1, class Param2>
class palDelegate< R ( Param1, Param2 ) >
  // Inherit from palDelegate2 so that it can be treated just like a palDelegate2
  : public _pal_delegate_internal::palDelegate2 < Param1, Param2, R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate2 < Param1, Param2, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:
  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1, Param2 p2 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};

template<typename R, class Param1, class Param2, class Param3>
class palDelegate< R ( Param1, Param2, Param3 ) >
  // Inherit from palDelegate3 so that it can be treated just like a palDelegate3
  : public _pal_delegate_internal::palDelegate3 < Param1, Param2, Param3, R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate3 < Param1, Param2, Param3, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:


  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1, Param2 p2, Param3 p3 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};

template<typename R, class Param1, class Param2, class Param3, class Param4>
class palDelegate< R ( Param1, Param2, Param3, Param4 ) >
  // Inherit from palDelegate4 so that it can be treated just like a palDelegate4
  : public _pal_delegate_internal::palDelegate4 < Param1, Param2, Param3, Param4, R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate4 < Param1, Param2, Param3, Param4, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:


  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};


template<typename R, class Param1, class Param2, class Param3, class Param4, class Param5>
class palDelegate< R ( Param1, Param2, Param3, Param4, Param5 ) >
  // Inherit from palDelegate5 so that it can be treated just like a palDelegate5
  : public _pal_delegate_internal::palDelegate5 < Param1, Param2, Param3, Param4, Param5, R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate5 < Param1, Param2, Param3, Param4, Param5, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:


  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};


template<typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6>
class palDelegate< R ( Param1, Param2, Param3, Param4, Param5, Param6 ) >
  // Inherit from palDelegate6 so that it can be treated just like a palDelegate6
  : public _pal_delegate_internal::palDelegate6 < Param1, Param2, Param3, Param4, Param5, Param6, R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate6 < Param1, Param2, Param3, Param4, Param5, Param6, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:


  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};

template<typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7>
class palDelegate< R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7 ) >
  // Inherit from palDelegate7 so that it can be treated just like a palDelegate7
  : public _pal_delegate_internal::palDelegate7 < Param1, Param2, Param3, Param4, Param5, Param6, Param7, R >
{
private:
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate7 < Param1, Param2, Param3, Param4, Param5, Param6, Param7, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:


  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};

template<typename R, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8>
class palDelegate< R ( Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8 ) >
  // Inherit from palDelegate8 so that it can be treated just like a palDelegate8
  : public _pal_delegate_internal::palDelegate8 < Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, R >
{
  // Make using the base type a bit easier via typedef.
  typedef _pal_delegate_internal::palDelegate8 < Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, R > BaseType;

  // Allow users access to the specific type of this delegate.
  typedef palDelegate SelfType;
public:
  // Mimic the base class constructors.
  palDelegate() : BaseType() { }

  template < class X, class Y >
  palDelegate(Y * pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ))
    : BaseType(pthis, function_to_bind)	 { }

  template < class X, class Y >
  palDelegate(const Y *pthis,
    R (X::* function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ) const)
    : BaseType(pthis, function_to_bind)
  {	 }

  palDelegate(R (*function_to_bind)( Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8 ))
    : BaseType(function_to_bind)  { }
  void operator = (const BaseType &x)  {
    *static_cast<BaseType*>(this) = x; }
};

//N=0
template <class X, class Y, class RetType>
_pal_delegate_internal::palDelegate0<RetType> palMakeDelegate(Y* x, RetType (X::*func)()) {
  return _pal_delegate_internal::palDelegate0<RetType>(x, func);
}

template <class X, class Y, class RetType>
_pal_delegate_internal::palDelegate0<RetType> palMakeDelegate(Y* x, RetType (X::*func)() const) {
  return _pal_delegate_internal::palDelegate0<RetType>(x, func);
}

//N=1
template <class X, class Y, class Param1, class RetType>
_pal_delegate_internal::palDelegate1<Param1, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1)) {
  return _pal_delegate_internal::palDelegate1<Param1, RetType>(x, func);
}

template <class X, class Y, class Param1, class RetType>
_pal_delegate_internal::palDelegate1<Param1, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1) const) {
  return _pal_delegate_internal::palDelegate1<Param1, RetType>(x, func);
}

//N=2
template <class X, class Y, class Param1, class Param2, class RetType>
_pal_delegate_internal::palDelegate2<Param1, Param2, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2)) {
  return _pal_delegate_internal::palDelegate2<Param1, Param2, RetType>(x, func);
}

template <class X, class Y, class Param1, class Param2, class RetType>
_pal_delegate_internal::palDelegate2<Param1, Param2, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2) const) {
  return _pal_delegate_internal::palDelegate2<Param1, Param2, RetType>(x, func);
}

//N=3
template <class X, class Y, class Param1, class Param2, class Param3, class RetType>
_pal_delegate_internal::palDelegate3<Param1, Param2, Param3, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3)) {
  return _pal_delegate_internal::palDelegate3<Param1, Param2, Param3, RetType>(x, func);
}

template <class X, class Y, class Param1, class Param2, class Param3, class RetType>
_pal_delegate_internal::palDelegate3<Param1, Param2, Param3, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3) const) {
  return _pal_delegate_internal::palDelegate3<Param1, Param2, Param3, RetType>(x, func);
}

//N=4
template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class RetType>
_pal_delegate_internal::palDelegate4<Param1, Param2, Param3, Param4, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4)) {
  return _pal_delegate_internal::palDelegate4<Param1, Param2, Param3, Param4, RetType>(x, func);
}

template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class RetType>
_pal_delegate_internal::palDelegate4<Param1, Param2, Param3, Param4, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4) const) {
  return _pal_delegate_internal::palDelegate4<Param1, Param2, Param3, Param4, RetType>(x, func);
}

//N=5
template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class RetType>
_pal_delegate_internal::palDelegate5<Param1, Param2, Param3, Param4, Param5, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5)) {
  return _pal_delegate_internal::palDelegate5<Param1, Param2, Param3, Param4, Param5, RetType>(x, func);
}

template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class RetType>
_pal_delegate_internal::palDelegate5<Param1, Param2, Param3, Param4, Param5, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5) const) {
  return _pal_delegate_internal::palDelegate5<Param1, Param2, Param3, Param4, Param5, RetType>(x, func);
}

//N=6
template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class RetType>
_pal_delegate_internal::palDelegate6<Param1, Param2, Param3, Param4, Param5, Param6, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6)) {
  return _pal_delegate_internal::palDelegate6<Param1, Param2, Param3, Param4, Param5, Param6, RetType>(x, func);
}

template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class RetType>
_pal_delegate_internal::palDelegate6<Param1, Param2, Param3, Param4, Param5, Param6, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6) const) {
  return _pal_delegate_internal::palDelegate6<Param1, Param2, Param3, Param4, Param5, Param6, RetType>(x, func);
}

//N=7
template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class RetType>
_pal_delegate_internal::palDelegate7<Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7)) {
  return _pal_delegate_internal::palDelegate7<Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>(x, func);
}

template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class RetType>
_pal_delegate_internal::palDelegate7<Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7) const) {
  return _pal_delegate_internal::palDelegate7<Param1, Param2, Param3, Param4, Param5, Param6, Param7, RetType>(x, func);
}

//N=8
template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8, class RetType>
_pal_delegate_internal::palDelegate8<Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8)) {
  return _pal_delegate_internal::palDelegate8<Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, RetType>(x, func);
}

template <class X, class Y, class Param1, class Param2, class Param3, class Param4, class Param5, class Param6, class Param7, class Param8, class RetType>
_pal_delegate_internal::palDelegate8<Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, RetType> palMakeDelegate(Y* x, RetType (X::*func)(Param1 p1, Param2 p2, Param3 p3, Param4 p4, Param5 p5, Param6 p6, Param7 p7, Param8 p8) const) {
  return _pal_delegate_internal::palDelegate8<Param1, Param2, Param3, Param4, Param5, Param6, Param7, Param8, RetType>(x, func);
}

#endif
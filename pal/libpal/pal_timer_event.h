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

#ifndef LIBPAL_PAL_TIMER_EVENT_H_
#define LIBPAL_PAL_TIMER_EVENT_H_

#include "libpal/pal_ilist.h"
#include "libpal/pal_timer.h"
#include "libpal/pal_event.h"

class palTimerEvent {
public:
  typedef palEvent<void (palTimerEvent* timer)> TimerEvent;
  typedef TimerEvent::DelegateType TimerEventDelegate;

  palTimerEvent();
  ~palTimerEvent();

  void SetName(const char* name);
  void SetInterval(palTimerTick ticks);

  void Enable();
  void Disable();
  void ToggleEnable();
  bool IsEnabled();
  void Step(palTimerTick ticks);

  TimerEvent& GetTimerEvent();
private:
  TimerEvent event_;
  const char* name_;
  palTimerTick interval_;
  palTimerTick elapsed_;
  bool enabled_;
public:
  palIListNodeDeclare(palTimerEvent, list_node);
};

class palTimerEventManager {
public:
  palTimerEventManager();
  ~palTimerEventManager();

  void Register(palTimerEvent* timer);
  void Unregister(palTimerEvent* timer);

  void StepTimers(palTimerTick tick);
private:
  palIList list_head;
};

#endif  // LIBPAL_PAL_TIMER_EVENT_H_
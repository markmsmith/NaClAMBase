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

#include "libpal/pal_timer_event.h"

palTimerEvent::palTimerEvent() {
  enabled_ = false;
  elapsed_ = 0;
}

palTimerEvent::~palTimerEvent() {
}

void palTimerEvent::SetName(const char* name) {
  name_ = name;
}

void palTimerEvent::SetInterval(palTimerTick ticks) {
  interval_ = ticks;
}

void palTimerEvent::Enable() {
  enabled_ = true;
}

void palTimerEvent::Disable() {
  enabled_ = false;
}

void palTimerEvent::ToggleEnable() {
  enabled_ = !enabled_;
}

bool palTimerEvent::IsEnabled() {
  return enabled_;
}

void palTimerEvent::Step(palTimerTick ticks) {
  elapsed_ += ticks;
  if (elapsed_ > interval_) {
    elapsed_ = 0;
    event_.Fire(this);
  }
}

palTimerEvent::TimerEvent& palTimerEvent::GetTimerEvent() {
  return event_;
}

palTimerEventManager::palTimerEventManager() {

}

palTimerEventManager::~palTimerEventManager() {

}

void palTimerEventManager::Register(palTimerEvent* timer) {
  list_head.AddTail(&timer->list_node);
}

void palTimerEventManager::Unregister(palTimerEvent* timer) {
  list_head.Remove(&timer->list_node);
}

void palTimerEventManager::StepTimers(palTimerTick tick) {
  palIListForeachDeclare(palTimerEvent, list_node) fe(&list_head);

  while (fe.Finished() == false) {
    palTimerEvent* event = fe.GetListEntry();
    if (event->IsEnabled()) {
      event->Step(tick);
    }
    fe.Next();
  }
}
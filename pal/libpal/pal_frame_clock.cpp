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

#include "libpal/pal_frame_clock.h"

palFrameClock::palFrameClock() {
  _frame_step = 0.016f; // 16ms
  _max_delta_time = 0.016f; // 16ms
  _max_accumulated_time = 5.0f * _frame_step; // 5 frames
  _accumulated_time = 0.0f;
}

palFrameClock::~palFrameClock() {
}

void palFrameClock::Setup(float frame_step, float max_accumulated_time, float max_delta_time) {
  _frame_step = frame_step;
  _max_delta_time = max_delta_time;
  _max_accumulated_time = max_accumulated_time;
}

void palFrameClock::Reset() {
  _accumulated_time = 0.0f;
}

void palFrameClock::AddTime(float dt) {
  if (dt > _max_delta_time) {
    dt = _max_delta_time;
  }
  _accumulated_time += dt;
  if (_accumulated_time > _max_accumulated_time) {
    _accumulated_time = _max_accumulated_time;
  }
}

bool palFrameClock::CanTakeFrame() const {
  return _accumulated_time > _frame_step;
}

float palFrameClock::TakeFrame() {
  if (_accumulated_time > _frame_step) {
    _accumulated_time -= _frame_step;
    return _frame_step;
  } else {
    return 0.0f;
  }
}

float palFrameClock::GetAccumulatedTime() const {
  return _accumulated_time;
}

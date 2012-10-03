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

#include "libpal/pal_memory.h"
#include "libpal/pal_algorithms.h"
#include "libpal/pal_time_line.h"

static float palFilterLinear(float t) {
  return t;
}

palCustomFilterFunction filter_functions_[NUM_palTimeLineFilter] = {
  palFilterLinear,
  palFilterLinear,
  palFilterLinear,
  palFilterLinear,
};

palTimeLine::palTimeLine() {
  current_time_ = 0;
  current_filtered_position_ = 0;
  duration_ = 0;
  repeat_duration_ = 0;
  repeat_index_ = 0;
  repeat_count_ = 0;
  repeat_style_ = kTimeLineRepeatDont;
  filter_function_ = filter_functions_[kTimeLineFilterLinear];
  direction_ = kTimeLineForward;
  direction_sign_ = 1.0f;
}

palTimeLine::~palTimeLine() {
}

palTimeLine::palTimeLine(const palTimeLine& that) {
  this->current_time_ = that.current_time_;
  this->current_filtered_position_ = that.current_filtered_position_;
  this->duration_ = that.duration_;
  this->repeat_duration_ = that.repeat_duration_;
  this->repeat_index_ = that.repeat_index_;
  this->repeat_count_ = that.repeat_count_;
  this->repeat_style_ = that.repeat_style_;
  this->filter_function_ = that.filter_function_;
  this->direction_ = that.direction_;
  this->direction_sign_ = that.direction_sign_;
}

palTimeLine& palTimeLine::operator=(const palTimeLine& that) {
  if (this != &that) {
    this->current_time_ = that.current_time_;
    this->current_filtered_position_ = that.current_filtered_position_;
    this->duration_ = that.duration_;
    this->repeat_duration_ = that.repeat_duration_;
    this->repeat_index_ = that.repeat_index_;
    this->repeat_count_ = that.repeat_count_;
    this->repeat_style_ = that.repeat_style_;
    this->filter_function_ = that.filter_function_;
    this->direction_ = that.direction_;
    this->direction_sign_ = that.direction_sign_;
  }
  return *this;
}

palTimeLine::EndEventType& palTimeLine::GetTimeLineEndEvent() {
  return end_event_;
}

void palTimeLine::SetDuration(float seconds) {
  duration_ = seconds;
}

void palTimeLine::SetDuration(palTimerTick ticks) {
  duration_ = palTimerGetSeconds(ticks);
}

/* Controlling direction */
void palTimeLine::SetDirection(palTimeLineDirection direction) {
  direction_ = direction;
}

/* Controlling repetition */
void palTimeLine::RepeatForDuration(float seconds) {
  repeat_duration_ = seconds;
}

void palTimeLine::RepeatForDuration(palTimerTick ticks) {
  repeat_duration_ = palTimerGetSeconds(ticks);
}

void palTimeLine::RepeatForever() {
  repeat_count_ = -1;
}

void palTimeLine::RepeatCountTimes(int count) {
  repeat_count_ = count;
}

void palTimeLine::SetRepeatStyle(palTimeLineRepeatStyle repeat_style) {
  repeat_style_ = repeat_style;
}

/* Controlling filtering */
void palTimeLine::SetFilter(palTimeLineFilter filter) {
  filter_function_ = filter_functions_[filter];
}

void palTimeLine::SetCustomFilter(palCustomFilterFunction filter_function) {
  filter_function_ = filter_function;
}

void palTimeLine::Reset() {
  if (direction_ == kTimeLineForward) {
    current_time_ = 0.0f;
  } else {
    current_time_ = duration_;
  }
  repeat_index_ = 0;
  current_filtered_position_ = filter_function_(current_time_/duration_);
}

void palTimeLine::Step(float seconds) {
  current_time_ += direction_sign_ * seconds;
  current_time_ = palClamp(current_time_, 0.0f, duration_);
  current_filtered_position_ = filter_function_(current_time_/duration_);

  if (current_time_ <= 0 || current_time_ >= duration_) {
    /* We have reached the end */

    end_event_.Fire(this);

    // handle repetition
    if (repeat_style_ != kTimeLineRepeatDont) {
      /* we should repeat if repeat duration is positive and we haven't reached the repeat time limit */
      bool should_repeat = repeat_duration_ > 0.0f && repeat_duration_ < (duration_ * repeat_index_);
      /* we should repeat if repeat count is positive and we haven't reached the repeat count limit */
      should_repeat = should_repeat || (repeat_count_ > 0 && repeat_index_ < repeat_count_);

      if (should_repeat) {
        repeat_index_++;

        if (repeat_style_ == kTimeLineRepeatFlipDirection) {
          /* Flip directions */
          if (direction_ == kTimeLineForward) {
            direction_ = kTimeLineBackward;
          } else {
            direction_ = kTimeLineForward;
          }
          direction_sign_ *= -1.0f;
        }

        /* repeating, adjust time */
        if (direction_ == kTimeLineForward) {
          current_time_ = 0.0f;
        } else {
          current_time_ = duration_;
        }
        
        current_filtered_position_ = filter_function_(current_time_/duration_);
      }
    }
  }
  
}

void palTimeLine::Step(palTimerTick ticks) {
  Step(palTimerGetSeconds(ticks));
}

/* Returns between 0.0 and 1.0 */
float palTimeLine::GetNormalizedPosition() const {
  return current_time_/duration_;
}

/* Mainly returns 0.0 to 1.0, but can return between -1.0 and 2.0 */
float palTimeLine::GetFilteredPosition() const {
  return current_filtered_position_;
}
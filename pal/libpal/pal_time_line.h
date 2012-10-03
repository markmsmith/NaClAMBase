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

#ifndef LIBPAL_PAL_TIME_LINE_H_
#define LIBPAL_PAL_TIME_LINE_H_

#include "libpal/pal_event.h"
#include "libpal/pal_timer.h"

enum palTimeLineDirection {
  kTimeLineForward = 0,
  kTimeLineBackward = 1,
  NUM_palTimeLineDirection,
};

enum palTimeLineRepeatStyle {
  /* Don't repeat */
  kTimeLineRepeatDont,
  /* Each time the time line repeats, the direction will be the same */
  kTimeLineRepeatSameDirection,
  /* Each time the time line repeats, the direction will be reversed */
  kTimeLineRepeatFlipDirection,
  NUM_palTimeLineRepeatStyle,
};

enum palTimeLineFilter {
  kTimeLineFilterLinear,
  kTimeLineFilterEaseInQuadratic,
  kTimeLineFilterEaseOutQuadratic,
  kTimeLineFilterEaseInOutQuadratic,
  NUM_palTimeLineFilter,
};

typedef float (*palCustomFilterFunction)(float unfiltered);

class palTimeLine {
public:
  typedef palEvent<void (palTimeLine*)> EndEventType;
  typedef EndEventType::DelegateType TimeLineEndDelegate;

  palTimeLine();
  ~palTimeLine();
  palTimeLine(const palTimeLine& that);
  palTimeLine& operator=(const palTimeLine& that);

  void SetDuration(float seconds);
  void SetDuration(palTimerTick ticks);

  /* Callback registration */
  EndEventType& GetTimeLineEndEvent();

  /* Controlling direction */
  void SetDirection(palTimeLineDirection direction);

  /* Controlling repetition */
  void RepeatForDuration(float seconds);
  void RepeatForDuration(palTimerTick ticks);
  void RepeatForever();
  void RepeatCountTimes(int count);
  void SetRepeatStyle(palTimeLineRepeatStyle repeat_style);

  /* Controlling filtering */
  void SetFilter(palTimeLineFilter filter);
  void SetCustomFilter(palCustomFilterFunction filter_function);

  void Reset();
  void Step(float seconds);
  void Step(palTimerTick ticks);

  /* Returns between 0.0 and 1.0 */
  float GetNormalizedPosition() const;

  /* Mainly returns 0.0 to 1.0, but can return between -1.0 and 2.0 */
  float GetFilteredPosition() const;
private:
  float current_time_;
  float current_filtered_position_;
  float duration_;
  float repeat_duration_;
  int repeat_index_;
  int repeat_count_;
  palTimeLineDirection direction_;
  float direction_sign_;
  palTimeLineRepeatStyle repeat_style_;
  palCustomFilterFunction filter_function_;
  EndEventType end_event_;
};

#endif  // LIBPAL_PAL_TIME_LINE_H_
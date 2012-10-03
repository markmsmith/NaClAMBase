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

#ifndef LIBPAL_PAL_TIMER_H__
#define LIBPAL_PAL_TIMER_H__

#include "libpal/pal_platform.h"
#include "libpal/pal_types.h"

// the platform headers define one datatype and two functions:
// typedef ? palTimerTicks -  an integer representing the number of clock ticks
// palTimerTicks palTimerGetTicks() - get the current tick count
// float palTimerGetSeconds(palTimerTick ticks) - gets the number of seconds ticks holds

#if defined(PAL_PLATFORM_WINDOWS)
#include "libpal/windows/pal_timer_windows.h"
#elif defined(PAL_PLATFORM_APPLE)
#include "libpal/apple/pal_timer_apple.h"
#elif defined(PAL_PLATFORM_NACL)
#include "libpal/posix/pal_timer_posix.h"
#else
#error no palTimer for your platform
#endif

PAL_INLINE float palTimerGetMilliseconds(palTimerTick ticks) {
  float dt_s = palTimerGetSeconds(ticks);
  return dt_s * 1000.0f;
}

PAL_INLINE uint32_t palTimerTickGetMilliseconds(palTimerTick ticks) {
  palTimerTick hz = palTimerGetFrequency();
  hz /= 1000;
  return (uint32_t)(ticks / hz);
}

PAL_INLINE palTimerTick palTimerTickFromMilliseconds(uint32_t ms) {
  palTimerTick hz = palTimerGetFrequency();
  hz /= 1000;
  return hz * ms;
}

class palTimer {
  palTimerTick start_;
  palTimerTick stop_;

 public:
   PAL_INLINE palTimer() {
   }

   PAL_INLINE ~palTimer() {
   }

   PAL_INLINE void Start() {
     start_ = palTimerGetTicks();
   }

   PAL_INLINE void Stop() {
     stop_ = palTimerGetTicks();
   }

   PAL_INLINE palTimerTick GetDeltaTime() const {
     return stop_ - start_;
   }

   PAL_INLINE palTimerTick GetStartTime() const {
     return start_;
   }

  PAL_INLINE palTimerTick GetStopTime() const {
    return stop_;
   }

   PAL_INLINE float GetDeltaSeconds() const {
     palTimerTick dt = GetDeltaTime();
     float dt_s = palTimerGetSeconds(dt);
     return dt_s;
   }

   PAL_INLINE float GetDeltaMilliSeconds() const {
    return GetDeltaSeconds() * 1000.0f;
   }

   PAL_INLINE float GetDeltaMicroSeconds() const {
    return GetDeltaSeconds() * 1000000.0f;
   }

   void ConsoleOutput() const;
};

#endif  // LIBPAL_PAL_TIMER_H__
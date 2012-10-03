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

#ifndef LIBPAL_PAL_PROFILER_H_
#define LIBPAL_PAL_PROFILER_H_

#include "pal_timer.h"

struct palProfilerNode {
  palProfilerNode(const char* name_);

  const char* name;

  int recursion_count;

  int call_count;

  palTimerTick enter_to_exit_time;
  palTimerTick enter_tick;

  palProfilerNode* parent;
  palProfilerNode* sibling;
  palProfilerNode* child;
};

class palProfilerNodeIterator {
public:
  palProfilerNodeIterator(palProfilerNode* node);

  bool IsRoot();
  bool IsLeaf();

  int GetNumChildren();
  int GetNumSiblings();

  bool MoveToNthChild(int child_index);

  bool MoveUp();
  bool MoveRight();
  bool MoveDown();
  bool MoveNextSibling();

  bool MoveToFirstChildAtThisDepth();

  const char* GetParentName();
  palTimerTick GetParentTime();
  int GetParentCallCount();

  const char* GetCurrentName();
  palTimerTick GetCurrentTime();
  int GetCurrentCallCount();

protected:
  palProfilerNode* current;
};

class palProfiler {
public:
  palProfiler();

  int Shutdown();

  void Enter(const char* name);
  void Exit();

  void ResetCallCounter();

  void ConsoleDump();

  palProfilerNode* GetRootNode();
private:
  void FreeNode(palProfilerNode* node);
  palProfilerNode root_;
  palProfilerNode* current_node_;
};

class palProfilerScope {
  palProfiler* profiler_;
public:
  palProfilerScope(palProfiler* profiler, const char* name) : profiler_(profiler) {
    profiler_->Enter(name);
  }
  ~palProfilerScope() {
    profiler_->Exit();
  }
};

int palProfilerInit();
int palProfilerShutdown();

extern palProfiler g_palProfiler;
#define palProfileScope(name) palProfilerScope __profile_scope(&g_palProfiler, name);

#endif  // LIBPAL_PAL_PROFILER_H_
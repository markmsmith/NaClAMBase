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

#include "libpal/pal_debug.h"
#include "libpal/pal_allocator.h"
#include "libpal/pal_profiler.h"

palProxyAllocator* _profile_proxy_allocator;

palProfilerNode::palProfilerNode(const char* name_) {
  name = name_;
  call_count = 0;
  recursion_count = 0;
  enter_tick = 0;
  enter_to_exit_time = 0;
  parent = NULL;
  sibling = NULL;
  child = NULL;
}

palProfilerNodeIterator::palProfilerNodeIterator(palProfilerNode* node) {
  current = node;
}

bool palProfilerNodeIterator::IsRoot() {
  return current->parent == NULL;
}

bool palProfilerNodeIterator::IsLeaf() {
  return current->child == NULL;
}

int palProfilerNodeIterator::GetNumChildren() {
  palProfilerNode* node = current;
  int node_count = 0;

  if (!node)
    return node_count;

  node = node->child;

  while (node) {
    node_count++;
    node = node->sibling;
  }

  return node_count;
}

int palProfilerNodeIterator::GetNumSiblings() {
  palProfilerNode* node = current->parent;
  int node_count = 0;

  if (!node)
    return node_count;

  node = node->child;

  while (node) {
    node_count++;
    node = node->sibling;
  }

  return node_count;
}

bool palProfilerNodeIterator::MoveToNthChild(int child_index) {
  palProfilerNode* node = current;
  int node_count = 0;

  if (!node)
    return false;

  // first child
  node = node->child;

  if (!node)
    return false;

  while (child_index > 0) {
    if (!node->sibling)
      break;
    node = node->sibling;
    child_index--;
  }

  if (child_index == 0) {
    current = node;
  }

  return child_index == 0;
}

bool palProfilerNodeIterator::MoveUp() {
  if (!current) {
    return false;
  }

  if (!current->parent) {
    return false;
  }

  current = current->parent;

  return true;
}

bool palProfilerNodeIterator::MoveRight() {
  if (!current) {
    return false;
  }

  if (!current->sibling) {
    return false;
  }

  current = current->sibling;

  return true;
}

bool palProfilerNodeIterator::MoveDown() {
  if (!current) {
    return false;
  }

  if (!current->child) {
    return false;
  }

  current = current->child;

  return true;
}

bool palProfilerNodeIterator::MoveNextSibling() {
  if (!current)
    return false;

  if (!current->sibling)
    return false;

  current = current->sibling;
  return true;
}

bool palProfilerNodeIterator::MoveToFirstChildAtThisDepth() {
  if (!current) {
    return false;
  }

  if (!current->parent) {
    return true;
  }

  current = current->parent->child;
  return true;
}

const char* palProfilerNodeIterator::GetParentName() {
  if (!current)
    return NULL;
  if (!current->parent)
    return NULL;
  return current->parent->name;
}

palTimerTick palProfilerNodeIterator::GetParentTime() {
  if (!current)
    return 0;
  if (!current->parent)
    return 0;
  return current->parent->enter_to_exit_time;
}

int palProfilerNodeIterator::GetParentCallCount() {
  if (!current)
    return 0;
  if (!current->parent)
    return 0;

  return current->parent->call_count;
}

const char* palProfilerNodeIterator::GetCurrentName() {
  if (!current) {
    return NULL;
  }

  return current->name;
}

palTimerTick palProfilerNodeIterator::GetCurrentTime() {
  if (!current) {
    return 0;
  }

  return current->enter_to_exit_time;
}

int palProfilerNodeIterator::GetCurrentCallCount() {
  if (!current) {
    return 0;
  }

  return current->call_count;
}

palProfiler::palProfiler() : root_("palProfilerRoot") {
  current_node_ = &root_;
}

int palProfiler::Shutdown() {
  FreeNode(&root_);
  return 0;
}

void palProfiler::Enter(const char* name) {
  if (current_node_->name == name) {
    // recursive call
    current_node_->recursion_count++;
    return;
  }

  // search children of current node
  palProfilerNode* search_node = current_node_->child;
  while (search_node) {
    if (search_node->name == name) {
      break;
    }
    // check next
    search_node = search_node->sibling;
  }

  if (search_node) {
    // found it
    search_node->call_count++;
    search_node->enter_tick = palTimerGetTicks();
    current_node_ = search_node; // make current
    return;
  }

  // need to create new node

  palProfilerNode* new_node = _profile_proxy_allocator->Construct<palProfilerNode>(name);
  // establish family relationship
  new_node->sibling = current_node_->child;
  current_node_->child = new_node;
  new_node->parent = current_node_;
  current_node_ = new_node;
  new_node->call_count++;
  new_node->enter_tick = palTimerGetTicks();
  return;
}

void palProfiler::Exit() {
  if (current_node_->recursion_count > 0) {
    current_node_->recursion_count--;
    return;
  }

  // how long were we in this function?
  palTimerTick exit_tick = palTimerGetTicks();
  current_node_->enter_to_exit_time = exit_tick - current_node_->enter_tick;

  // move current node up the tree
  palAssert(current_node_->parent != NULL);
  current_node_ = current_node_->parent;
}

static void reset_call_count(palProfilerNode* node) {
  node->call_count = 0;
  if (node->child) {
    reset_call_count(node->child);
  }
  if (node->sibling) {
    reset_call_count(node->sibling);
  }
}

void palProfiler::ResetCallCounter() {
  reset_call_count(&root_);
}

static void console_dump(palProfilerNode* node, int depth) {
  for(int i = 0; i < depth; i++) {
    palPrintf("  ");
  }

  palPrintf(": %s %.2f\n", node->name, palTimerGetSeconds(node->enter_to_exit_time) * 1000.0f);

  if (node->child) {
    console_dump(node->child, depth+1);
  }
  if (node->sibling) {
    console_dump(node->sibling, depth);
  }
}

void palProfiler::ConsoleDump() {
  console_dump(&root_, 0);
}

palProfilerNode* palProfiler::GetRootNode() {
  return &root_;
}

void palProfiler::FreeNode(palProfilerNode* node) {
  if (node->child) {
    palProfilerNode* sibling_node = node->child->sibling;
    while (sibling_node != NULL) {
      palProfilerNode* next = sibling_node->sibling;
      FreeNode(sibling_node);
      sibling_node = next;
    }
    FreeNode(node->child);
  }
  if (node != &root_) {
    _profile_proxy_allocator->Destruct(node);
  }
}

palProfiler g_palProfiler;

int palProfilerInit() {
  _profile_proxy_allocator = g_DefaultHeapAllocator->Construct<palProxyAllocator>("palProfile", g_DefaultHeapAllocator);
  return 0;
}

int palProfilerShutdown() {
  g_palProfiler.Shutdown();
  g_DefaultHeapAllocator->Destruct(_profile_proxy_allocator);
  return 0;
}
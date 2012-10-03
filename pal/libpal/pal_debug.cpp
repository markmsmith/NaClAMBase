#include <stdlib.h>
#include "libpal/pal_debug.h"

#if defined(PAL_PLATFORM_WINDOWS)
#include <windows.h>
#include <Dbghelp.h>
#pragma comment(lib, "dbghelp")
static HANDLE hProcess;
#else
#warning no palDebug on your platform
#endif

void palAbort() {
  abort();
}

int palDebugInit() {
#if defined(PAL_PLATFORM_WINDOWS)
  static bool init = false;
  if (init == false) {
    init = true;
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    hProcess = GetCurrentProcess();
    if (!SymInitialize(hProcess, NULL, TRUE)) {
      init = false;
      return -1;
    }
  }
  return 0;
#else
  return -1;
#endif
}

int palDebugShutdown() {
  return 0;
}

int palDebugCaptureCallstack(int entries_to_skip, int max_entries, uintptr_t* callstack) {
#if defined(PAL_PLATFORM_WINDOWS)
  // capture callstack
  WORD num_frames = CaptureStackBackTrace(entries_to_skip, max_entries-1, (PVOID*)callstack, NULL);
  callstack[num_frames] = NULL;
  return num_frames;
#else
  return 0;
#endif
}

int palDebugGetSizeForSymbolLookup(int name_length) {
#if defined(PAL_PLATFORM_WINDOWS)
  return sizeof(SYMBOL_INFO) + name_length;
#else
  return 0;
#endif
}

const char* palDebugLookupSymbol(uintptr_t pc, int name_length, void* buffer) {
#if defined(PAL_PLATFORM_WINDOWS)
  SYMBOL_INFO* sym_info = (SYMBOL_INFO*)buffer;
  sym_info->MaxNameLen = name_length;
  sym_info->SizeOfStruct = sizeof(SYMBOL_INFO);
  SymFromAddr(hProcess, (DWORD64)(pc), 0, sym_info);
  return &sym_info->Name[0];
#else
  return NULL;
#endif
}
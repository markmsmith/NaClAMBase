#pragma once

#include "libpal/pal_platform.h"

#include <cstdint>
#include <cstddef>

#if !defined(PAL_PLATFORM_WINDOWS)
#include <unistd.h>
#endif
#include <cfloat>
#include <cstdarg>

#if defined(PAL_COMPILER_MICROSOFT)

#if defined(PAL_ARCH_64BIT)
typedef int64_t ssize_t;
#else
typedef int32_t ssize_t;
#endif

#endif
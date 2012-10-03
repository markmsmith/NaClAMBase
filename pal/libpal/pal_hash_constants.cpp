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

#include "libpal/pal_hash_constants.h"

const palHashTableSizeConfiguration kPalHashTableSizeConfigurations[] = {
  { 32,        53     },
  { 64,        97     }, 
  { 128,       193    },
  { 256,       389    },
  { 512,       769    },
  { 1024,      1543   },
  { 2048,      3079   },
  { 4196,      6151   },
  { 8192,      12289  },
  { 16384,     24593  },
  { 32768,     49157  },
  { 65536,     98317  },
  { 131072,    196613  },
  { 262144,    393241  },
  { 524288,    786433  },
  { 1048576,   1572869 },
  { 2097152,   3145739 },
  { 4194304,   6291469 },
  { 8388608,   12582917},
  { 16777216,  25165843},
  { 33554432,  50331653},
  { 67108864,  100663319},
  { 134217728, 201326611},
  // this could be extended. But 134 million feels like more than enough entries in the hash
  // Source: http://planetmath.org/encyclopedia/GoodHashTablePrimes.html
};

const int kPalNumHashTableSizeConfigurations = sizeof(kPalHashTableSizeConfigurations)/sizeof(kPalHashTableSizeConfigurations[0]);

const int kPalHashNULL = 0xffffffff;
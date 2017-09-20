#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include "common/lib/lib.h"

//*< for LoRaWAN
#define memset1             osel_memset
#define memcpy1             osel_memcpy
#define memcmp1             osel_memcmp
#define memcpyr             osel_memcpyr

#define srand1              l_srand

#ifndef bool
#define bool                bool_t
#endif

#ifndef false
#define false               FALSE
#endif

#ifndef true
#define true                TRUE
#endif

#endif

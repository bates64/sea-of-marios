#include "common.h"
#include "dx/config.h"
#include "dx/debug_menu.h"

#define MPACK_CONFORMING 0

typedef u8 uint8_t;
typedef u16 uint16_t;
typedef u32 uint32_t;
typedef u64 uint64_t;
typedef s8 int8_t;
typedef s16 int16_t;
typedef s32 int32_t;
typedef s64 int64_t;
#ifndef __cplusplus
typedef b32 bool;
#define false FALSE
#define true TRUE
#endif
#define SIZE_MAX 0xFFFFFFFF

// Use stdlib from os
#define MPACK_STDLIB 0
#include "gcc/memory.h"
#include "gcc/string.h"
#define MPACK_MEMCMP memcmp
#define MPACK_MEMCPY memcpy
#define MPACK_MEMMOVE memmove
#define MPACK_MEMSET memset
#define MPACK_STRLEN strlen
// Builder methods seem buggy with our heap
//#define MPACK_MALLOC general_heap_malloc
//#define MPACK_REALLOC
//#define MPACK_FREE general_heap_free

#define MPACK_CUSTOM_ASSERT 1
#define MPACK_CUSTOM_BREAK 1

#define MPACK_DEBUG DX_DEBUG_MENU

#define MPACK_NO_BUILTINS 1

#define MPACK_BUILDER_INTERNAL_STORAGE 1

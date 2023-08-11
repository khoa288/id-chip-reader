/**
 * @author Khoa Nguyen
 * @file typedef.h
 * @brief Header file for typedefs and macros.
 */

#pragma once
#ifndef CRYPTOGRAPHY_TYPEDEF_H_
#define CRYPTOGRAPHY_TYPEDEF_H_

// typedef _Bool            bool, BOOL;

typedef unsigned char uchar, u8, U8, uint8, uint8_t, UINT8, BYTE;
typedef signed char s8, S8, int8, INT8, char_t;

typedef unsigned int u16, uint16, uint16_t, UINT16, WORD;
typedef signed int s16, S16, int16, INT16;

typedef unsigned long uint, ushort, u32, U32, uint32, uint32_t, UINT32, DWORD;
typedef signed long s32, S32, int32, INT32;

typedef unsigned long uint64_t, u64, U64, uint64, UINT64;
typedef signed long s64, S64, int64, INT64;

typedef unsigned short string;

#define TRUE  1
#define FALSE 0
#define true  1
#define false 0
// #define NULL            0

#endif	// #ifndef CRYPTOGRAPHY_TYPEDEF_H_

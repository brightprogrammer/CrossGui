/**
 * @file Common.h
 * @date Sun, 21st January 2024
 * @author Siddharth Mishra (admin@brightprogrammer.in)
 * @copyright Copyright 2024 Siddharth Mishra
 * @copyright Copyright 2024 Anvie Labs
 *
 * Copyright 2024 Siddharth Mishra, Anvie Labs
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 *    and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 *    and the following disclaimer in the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse
 *    or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS “AS IS” AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * */

#ifndef ANVIE_COMMON_H
#define ANVIE_COMMON_H

#include <stdio.h>
#include <stdlib.h>

/********************************** CONVINIENT WRAPPER MACROS *************************************/


#define CONSTRUCTOR              __attribute__ ((constructor))
#define DESTRUCTOR               __attribute__ ((destructor))
#define FORCE_INLINE             __attribute__ ((always_inline))
#define UNUSED(x)                ((void)(x))
#define NEW(type)                (type *)calloc (1, sizeof (type))
#define ALLOCATE(type, n)        (type *)calloc (n, sizeof (type))
#define REALLOCATE(ptr, type, n) (type *)realloc (ptr, n * sizeof (type));
#define FREE(x)                  free ((void *)(x))
#define PACKED                   __attribute__ ((packed))

#define ALIGN_UP(x, y) (x + (y - (x % y)))
#define ALIGN_DOWN(x, y) (x - (x % y)))


/************************************* MISC UTILITY MACROS ****************************************/




#define MAX(x, y) ((x) > (y) ? (x) : (y))
#define MIN(x, y) ((x) < (y) ? (x) : (y))

#define MIN3(x, y, z) MIN (x, MIN (y, z))
#define MAX3(x, y, z) MAX (x, MAX (y, z))

#define CLAMP(x, y, z)   MIN (MAX (x, y), z)
#define ARRAY_SIZE(arr)  sizeof (arr) / sizeof (arr[0])
#define CHECK_ITER(b, e) (b && e && (b <= e))




/************************************* ERROR HANDLER MACROS ***************************************/




#define ERR_INVALID_ARGUMENTS              "Invalid Arguments\n"
#define ERR_OUT_OF_MEMORY                  "Out of memory (allocation failed)\n"
#define ERR_INVALID_SIZE                   "Invalid size (zero)\n"
#define ERR_INVALID_OBJECT_REF             "Invalid object reference (NULL)\n"
#define ERR_INVALID_OBJECT_CONTENTS        "Invalid contents inside provided object (not what expected)\n"
#define ERR_INVALID_ITERATOR               "Invalid iterator (NULL)\n"
#define ERR_OBJECT_INITIALIZATION_FAILED   "Failed to initialze object\n"
#define ERR_OBJECT_DEINITIALIZATION_FAILED "Failed to de-initialze object\n"

#define ERR_FILE_OPEN_FAILED        "Failed to open file (NULL returned)\n"
#define ERR_FILE_SEEK_FAILED        "Failed to seek/tell file position\n"
#define ERR_FILE_READ_FAILED        "Failed to read file\n"
#define ERR_UNSUPPORTED_FILE_FORMAT "Unsupported file format\n"

#define RETURN_VALUE_IF(cond, value, ...)                                                          \
    do {                                                                                           \
        if ((cond)) {                                                                              \
            fputs (__FUNCTION__, stderr);                                                          \
            fputs (" : ", stderr);                                                                 \
            fprintf (stderr, __VA_ARGS__);                                                         \
            return value;                                                                          \
        }                                                                                          \
    } while (0)

#define RETURN_IF(cond, ...)                                                                       \
    do {                                                                                           \
        if ((cond)) {                                                                              \
            fputs (__FUNCTION__, stderr);                                                          \
            fputs (" : ", stderr);                                                                 \
            fprintf (stderr, __VA_ARGS__);                                                         \
            return;                                                                                \
        }                                                                                          \
    } while (0)

#define GOTO_HANDLER_IF(cond, handler, ...)                                                        \
    do {                                                                                           \
        if ((cond)) {                                                                              \
            fputs (__FUNCTION__, stderr);                                                          \
            fputs (" : " #handler " : ", stderr);                                                  \
            fprintf (stderr, __VA_ARGS__);                                                         \
            goto handler;                                                                          \
        }                                                                                          \
    } while (0)

#define CALL_HANDLER_IF(cond, handler, ...)                                                        \
    do {                                                                                           \
        if ((cond)) {                                                                              \
            fputs (__FUNCTION__, stderr);                                                          \
            fputs (" : ", stderr);                                                                 \
            fprintf (stderr, __VA_ARGS__);                                                         \
            handler;                                                                               \
        }                                                                                          \
    } while (0)

#define ABORT_IF(cond, ...)                                                                        \
    do {                                                                                           \
        if ((cond)) {                                                                              \
            fputs (__FUNCTION__, stderr);                                                          \
            fputs (" : ", stderr);                                                                 \
            fprintf (stderr, __VA_ARGS__);                                                         \
            abort();                                                                               \
        }                                                                                          \
    } while (0)

#define RETURN_VALUE_IF_REACHED(val, ...)                                                          \
    do {                                                                                           \
        fputs (__FUNCTION__, stderr);                                                              \
        fputs (" : ", stderr);                                                                     \
        fputs ("unreachable code reached : ", stderr);                                             \
        fprintf (stderr, __VA_ARGS__);                                                             \
        return val;                                                                                \
    } while (0)

#define RETURN_IF_REACHED(...)                                                                     \
    do {                                                                                           \
        fputs (__FUNCTION__, stderr);                                                              \
        fputs (" : ", stderr);                                                                     \
        fputs ("unreachable code reached : ", stderr);                                             \
        fprintf (stderr, __VA_ARGS__);                                                             \
        return;                                                                                    \
    } while (0)

#define ABORT_IF_REACHED(...)                                                                      \
    do {                                                                                           \
        fputs (__FUNCTION__, stderr);                                                              \
        fputs (" : ", stderr);                                                                     \
        fputs ("unreachable code reached : ", stderr);                                             \
        fprintf (stderr, __VA_ARGS__);                                                             \
        abort();                                                                                   \
    } while (0)

#define PRINT_ERR(...)                                                                             \
    do {                                                                                           \
        fputs (__FUNCTION__, stderr);                                                              \
        fputs (" : ", stderr);                                                                     \
        fprintf (stderr, __VA_ARGS__);                                                             \
    } while (0)


/********************************* ENDIANNESS CONVERSION MACROS ***********************************/

#define LE2BE_U2(x) (((U2)(x) << 8) | ((Uint16)(x) >> 8))
#define LE2BE_U4(x) ((U4)LE2BE_U2 ((x) & 0xffff) << 16 | (LE2BE_U2 (((x) >> 16) & 0xffff)))
#define LE2BE_U1(x)                                                                                \
    ((U8)LE2BE_U4 ((x) & 0xffffffff) << 32 | ((Uint64)LE2BE_U4 (((x) >> 32) & 0xffffffff)))

#define BE2LE_U2(x) LE2BE_U2 (x)
#define BE2LE_U4(x) LE2BE_U4 (x)
#define BE2LE_U1(x) LE2BE_U8 (x)

/***************************** COUNT NUMBER OF VARIADIC ARGUMENTS *********************************/

/**
 * @b Just a comma separated list of 127 to 0 in reverse order.
 *
 * This is used in @c COUNT_VA_ARGS to get the number of arguments in a macro
 * that accepts variadic arguments. The limit is only till 128 arguments. After
 * that the program itself might fail to compile.
 * */
#define COUNT_LIST_128()                                                                           \
    127, 126, 125, 124, 123, 122, 121, 120, 119, 118, 117, 116, 115, 114, 113, 112, 111, 110, 109, \
        108, 107, 106, 105, 104, 103, 102, 101, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89,   \
        88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67,    \
        66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47, 46, 45,    \
        44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28, 27, 26, 25, 24, 23,    \
        22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0

/**
 * @b Get 128th argument from a list of arguments to a macro.
 *
 * This is used in @c COUNT_VA_ARGS to get the 128th argument from a list of variadic
 * arguments. The 128th argument is always the number of arguments passed to @c COUNT_VA_ARGS,
 * given that there are 128 or less arguments in the variadic argument list.
 * */
#define GET_128TH_ARG(                                                                             \
    _1,                                                                                            \
    _2,                                                                                            \
    _3,                                                                                            \
    _4,                                                                                            \
    _5,                                                                                            \
    _6,                                                                                            \
    _7,                                                                                            \
    _8,                                                                                            \
    _9,                                                                                            \
    _10,                                                                                           \
    _11,                                                                                           \
    _12,                                                                                           \
    _13,                                                                                           \
    _14,                                                                                           \
    _15,                                                                                           \
    _16,                                                                                           \
    _17,                                                                                           \
    _18,                                                                                           \
    _19,                                                                                           \
    _20,                                                                                           \
    _21,                                                                                           \
    _22,                                                                                           \
    _23,                                                                                           \
    _24,                                                                                           \
    _25,                                                                                           \
    _26,                                                                                           \
    _27,                                                                                           \
    _28,                                                                                           \
    _29,                                                                                           \
    _30,                                                                                           \
    _31,                                                                                           \
    _32,                                                                                           \
    _33,                                                                                           \
    _34,                                                                                           \
    _35,                                                                                           \
    _36,                                                                                           \
    _37,                                                                                           \
    _38,                                                                                           \
    _39,                                                                                           \
    _40,                                                                                           \
    _41,                                                                                           \
    _42,                                                                                           \
    _43,                                                                                           \
    _44,                                                                                           \
    _45,                                                                                           \
    _46,                                                                                           \
    _47,                                                                                           \
    _48,                                                                                           \
    _49,                                                                                           \
    _50,                                                                                           \
    _51,                                                                                           \
    _52,                                                                                           \
    _53,                                                                                           \
    _54,                                                                                           \
    _55,                                                                                           \
    _56,                                                                                           \
    _57,                                                                                           \
    _58,                                                                                           \
    _59,                                                                                           \
    _60,                                                                                           \
    _61,                                                                                           \
    _62,                                                                                           \
    _63,                                                                                           \
    _64,                                                                                           \
    _65,                                                                                           \
    _66,                                                                                           \
    _67,                                                                                           \
    _68,                                                                                           \
    _69,                                                                                           \
    _70,                                                                                           \
    _71,                                                                                           \
    _72,                                                                                           \
    _73,                                                                                           \
    _74,                                                                                           \
    _75,                                                                                           \
    _76,                                                                                           \
    _77,                                                                                           \
    _78,                                                                                           \
    _79,                                                                                           \
    _80,                                                                                           \
    _81,                                                                                           \
    _82,                                                                                           \
    _83,                                                                                           \
    _84,                                                                                           \
    _85,                                                                                           \
    _86,                                                                                           \
    _87,                                                                                           \
    _88,                                                                                           \
    _89,                                                                                           \
    _90,                                                                                           \
    _91,                                                                                           \
    _92,                                                                                           \
    _93,                                                                                           \
    _94,                                                                                           \
    _95,                                                                                           \
    _96,                                                                                           \
    _97,                                                                                           \
    _98,                                                                                           \
    _99,                                                                                           \
    _100,                                                                                          \
    _101,                                                                                          \
    _102,                                                                                          \
    _103,                                                                                          \
    _104,                                                                                          \
    _105,                                                                                          \
    _106,                                                                                          \
    _107,                                                                                          \
    _108,                                                                                          \
    _109,                                                                                          \
    _110,                                                                                          \
    _111,                                                                                          \
    _112,                                                                                          \
    _113,                                                                                          \
    _114,                                                                                          \
    _115,                                                                                          \
    _116,                                                                                          \
    _117,                                                                                          \
    _118,                                                                                          \
    _119,                                                                                          \
    _120,                                                                                          \
    _121,                                                                                          \
    _122,                                                                                          \
    _123,                                                                                          \
    _124,                                                                                          \
    _125,                                                                                          \
    _126,                                                                                          \
    _127,                                                                                          \
    _128,                                                                                          \
    ...                                                                                            \
)                                                                                                  \
    _128 /* <-- returning the 128-th agument */

/* this just won't work without this helper method */
#define COUNT_VA_ARGS_HELPER(...) GET_128TH_ARG (__VA_ARGS__)

/**
 * @b Count number of arguments given to this macro at compile time.
 *
 * This can only get number of arguments if total number of arguments is less than or 
 * equal to 128. 
 * */
#define COUNT_VA_ARGS(...) COUNT_VA_ARGS_HELPER (__VA_ARGS__, COUNT_LIST_128())




/******************************* APPLY A MACRO MULTIPLE TIMES *************************************/




/** @b Used in @c FOR_EACH_HELPER macro to add paranthesis */
#define PARENS ()

#define EXPAND(...)  EXPAND4 (EXPAND4 (EXPAND4 (EXPAND4 (__VA_ARGS__))))
#define EXPAND4(...) EXPAND3 (EXPAND3 (EXPAND3 (EXPAND3 (__VA_ARGS__))))
#define EXPAND3(...) EXPAND2 (EXPAND2 (EXPAND2 (EXPAND2 (__VA_ARGS__))))
#define EXPAND2(...) EXPAND1 (EXPAND1 (EXPAND1 (EXPAND1 (__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

/**
 * @b Expand variadic argument list for given macro for each element one by one.
 *
 * This is a macro trick to apply a given macro on a variadic argument list 
 * one by one. The only limitiation here is that we cannot expand more than
 * 256 times for the moment. To make it do more than that, we just need to add 
 * one more line to the @c EXPAND macros
 *
 * Reference : https://www.scs.stanford.edu/~dm/blog/va-opt.html
 * */
#define FOR_EACH(macro, ...) __VA_OPT__ (EXPAND (FOR_EACH_HELPER (macro, __VA_ARGS__)))

/** @b Just a helper for @c FOR_EACH macro */
#define FOR_EACH_HELPER(macro, a1, ...)                                                            \
    macro (a1) __VA_OPT__ (FOR_EACH_AGAIN PARENS (macro, __VA_ARGS__))

/** @b Just a helper for @c FOR_EACH macro */
#define FOR_EACH_AGAIN() FOR_EACH_HELPER

#endif // ANVIE_COMMON_H

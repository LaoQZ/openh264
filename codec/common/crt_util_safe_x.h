/*!
 * \copy
 *     Copyright (c)  2010-2013, Cisco Systems
 *     All rights reserved.
 *
 *     Redistribution and use in source and binary forms, with or without
 *     modification, are permitted provided that the following conditions
 *     are met:
 *
 *        * Redistributions of source code must retain the above copyright
 *          notice, this list of conditions and the following disclaimer.
 *
 *        * Redistributions in binary form must reproduce the above copyright
 *          notice, this list of conditions and the following disclaimer in
 *          the documentation and/or other materials provided with the
 *          distribution.
 *
 *     THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *     "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *     LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *     FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *     COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *     INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *     BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *     LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *     CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *     LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *     ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *     POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * \file	crt_util_safe_x.h
 *
 * \brief	Safe CRT like util for cross platfroms support
 *
 * \date	06/04/2010 Created
 *
 *************************************************************************************
 */
#ifndef WELS_CRT_UTIL_SAFE_CROSS_PLATFORMS_H__
#define WELS_CRT_UTIL_SAFE_CROSS_PLATFORMS_H__

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#if defined(_WIN32)
#include <windows.h>
#include <sys/types.h>
#include <sys/timeb.h>
#else
#include <sys/timeb.h>
#include <sys/time.h>
#include "typedefs.h"
#endif//_WIN32

#include "typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define     WELS_FILE_SEEK_SET           SEEK_SET
#define     WELS_FILE_SEEK_CUR           SEEK_CUR
#define     WESL_FILE_SEEK_END           SEEK_END

typedef      FILE  WelsFileHandle;

#ifdef _WIN32
typedef      struct _timeb     SWelsTime;
#else
typedef      struct timeb      SWelsTime;
#endif

int32_t   WelsSnprintf (str_t* buffer,  int32_t sizeOfBuffer,  const str_t* format, ...);
str_t*   WelsStrncpy (str_t* dest, int32_t sizeInBytes, const str_t* src);
str_t*   WelsStrcat (str_t* dest, int32_t sizeInBytes, const str_t* src);
int32_t   WelsVsnprintf (str_t* buffer, int32_t sizeOfBuffer, const str_t* format, va_list argptr);

WelsFileHandle*        WelsFopen (const str_t* filename,  const str_t* mode);
int32_t                WelsFclose (WelsFileHandle*   fp);
int32_t                WelsFread (void* buffer, int32_t size, int32_t count, WelsFileHandle* fp);
int32_t                WelsFwrite (const void* buffer, int32_t size, int32_t count, WelsFileHandle* fp);
int32_t                WelsFseek (WelsFileHandle* fp, int32_t offset, int32_t origin);
int32_t                WelsFflush (WelsFileHandle* fp);

int32_t                WelsGetTimeOfDay (SWelsTime* tp);
int32_t                WelsStrftime (str_t* buffer, int32_t size, const str_t* format, const SWelsTime* tp);
uint16_t               WelsGetMillisecond (const SWelsTime* tp);


#ifdef __cplusplus
}
#endif


/*
 * Safe Lib specific errno codes.  These can be added to the errno.h file
 * if desired.
 */
#define ESNULLP         ( 400 )       /* null ptr                    */
#define ESZEROL         ( 401 )       /* length is zero              */
#define ESLEMIN         ( 402 )       /* length is below min         */
#define ESLEMAX         ( 403 )       /* length exceeds max          */
#define ESOVRLP         ( 404 )       /* overlap undefined           */
#define ESEMPTY         ( 405 )       /* empty string                */
#define ESNOSPC         ( 406 )       /* not enough space for s2     */
#define ESUNTERM        ( 407 )       /* unterminated string         */
#define ESNODIFF        ( 408 )       /* no difference               */
#define ESNOTFND        ( 409 )       /* not found                   */

/* EOK may or may not be defined in errno.h */
#ifndef EOK
#define EOK   0
#endif

#if (defined(WIN32) && defined(_MSC_VER) && (_MSC_VER<1500)) || defined(__GNUC__)

static inline int wels_strcat_s (char* dest, int dmax, const char* src) {
  int orig_dmax;
  char* orig_dest;
  const char* overlap_bumper;

  if (dest == NULL) {
//        invoke_safe_lib_constraint_handler("strcat_s: dest is null",
//                   NULL, ESNULLP);
    return (ESNULLP);
  }

  if (src == NULL) {
//        invoke_safe_lib_constraint_handler("strcat_s: src is null",
//                   NULL, ESNULLP);
    return (ESNULLP);
  }

  if (dmax <= 0) {
//        invoke_safe_lib_constraint_handler("strcat_s: dmax is 0",
//                   NULL, ESZEROL);
    return (ESZEROL);
  }

//    if (dmax > RSIZE_MAX_STR) {
//        invoke_safe_lib_constraint_handler("strcat_s: dmax exceeds max",
//                   NULL, ESLEMAX);
//        return (ESLEMAX);
//    }

  /* hold base of dest in case src was not copied */
  orig_dmax = dmax;
  orig_dest = dest;

  if (dest < src) {
    overlap_bumper = src;

    /* Find the end of dest */
    while (*dest != '\0') {

      if (dest == overlap_bumper) {
//                handle_error(orig_dest, orig_dmax, "strcat_s: overlapping objects", ESOVRLP);
        return (ESOVRLP);
      }

      dest++;
      dmax--;
      if (dmax == 0) {
//                handle_error(orig_dest, orig_dmax, "strcat_s: dest unterminated", ESUNTERM);
        return (ESUNTERM);
      }
    }

    while (dmax > 0) {
      if (dest == overlap_bumper) {
//                handle_error(orig_dest, orig_dmax, "strcat_s: overlapping objects", ESOVRLP);
        return (ESOVRLP);
      }

      *dest = *src;
      if (*dest == '\0') {
#ifdef SAFE_LIB_STR_NULL_SLACK
        /* null slack to clear any data */
        while (dmax) {
          *dest = '\0';
          dmax--;
          dest++;
        }
#endif
        return (EOK);
      }

      dmax--;
      dest++;
      src++;
    }

  } else {
    overlap_bumper = dest;

    /* Find the end of dest */
    while (*dest != '\0') {

      /*
       * NOTE: no need to check for overlap here since src comes first
       * in memory and we're not incrementing src here.
       */
      dest++;
      dmax--;
      if (dmax == 0) {
//                handle_error(orig_dest, orig_dmax, "strcat_s: dest unterminated", ESUNTERM);
        return (ESUNTERM);
      }
    }

    while (dmax > 0) {
      if (src == overlap_bumper) {
//                handle_error(orig_dest, orig_dmax, "strcat_s: overlapping objects", ESOVRLP);
        return (ESOVRLP);
      }

      *dest = *src;
      if (*dest == '\0') {
#ifdef SAFE_LIB_STR_NULL_SLACK
        /* null slack to clear any data */
        while (dmax) {
          *dest = '\0';
          dmax--;
          dest++;
        }
#endif
        return (EOK);
      }

      dmax--;
      dest++;
      src++;
    }
  }

  /*
   * the entire src was not copied, so null the string
   */
//    handle_error(orig_dest, orig_dmax, "strcat_s: not enough space for src", ESNOSPC);

  return (ESNOSPC);
}

#endif//(WIN32 && _MSC_VER && _MSC_VER<1500) || __GNUC__

#if defined(WIN32) && defined(_MSC_VER)
#if _MSC_VER >= 1500	// VS2008
#define STRCAT		strcat_s
#else	// mainly for VC6
#define STRCAT		wels_strcat_s	// override s.t.r.c.a.t here for safe
#endif//_MSC_VER >= 1500

#else//__GNUC__

#define STRCAT		wels_strcat_s	// override s.t.r.c.a.t here for safe

#endif//WIN32

#endif//WELS_CRT_UTIL_SAFE_CROSS_PLATFORMS_H__

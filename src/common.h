/*  $Id$
 *
 *  common.h
 *  nautilus-follow-symlink: Nautilus extension which allows opening the real
 *                           path of symbolic links
 *
 *   Copyright (C) 2006 Toni Corvera
 *
 *   This library is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU Lesser General Public
 *   License as published by the Free Software Foundation; either
 *   version 2.1 of the License, or (at your option) any later version.
 *
 *   This library is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the GNU Lesser General Public
 *   License along with this library; if not, write to the Free Software
 *   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *   Author: Toni Corvera <outlyer@outlyer.net>
 */

#ifndef FOLLOW_SYMLINK_COMMON_H
#define FOLLOW_SYMLINK_COMMON_H

/*
 * This file defines common debug utilities.
 * Also, includes config.h.
 */

#include <glib/gmessages.h> /* g_print() */
#include <glib/gprintf.h>   /* g_printf() */

#include <stdlib.h> /* getenv() (3) */
#include <string.h> /* strcmp() (3) */

#ifdef _DEBUG
    #include <stdarg.h> /* va_list, va_start ... */
#endif

#include "libintl.h"
#define _(STR) gettext(STR)

#ifdef HAVE_CONFIG_H
    #include "config.h"
#endif // HAVE_CONFIG_H

#if !defined(__inline)
    #define __inline
#elif defined(_DEBUG)
    #undef __inline
    #define __inline
#endif

// Based on : http://rlove.org/log/2005102601
#if __GNUC__ >= 3
    #define inline          inline __attribute__((always_inline))
    #define __pure          __attribute__((pure))
    //#define __const       __attribute__((const)) // fails
    #define __constfn       __attribute__((const))
    #define __noreturn      __attribute__((noreturn))
    #define __malloc        __attribute__((malloc))
    #define __must_check    __attribute__((warn_unused_result))
    #define __deprecated    __attribute__((deprecated))
    #define __used          __attribute__((used))
    // __unused gives errors in x86_64
    #define __UNUSED        __attribute__((unused))
    #define __packed        __attribute__((packed))
    #define likely(x)       __builtin_expect (!!(x), 1)
    #define unlikely(x)     __builtin_expect (!!(x), 0)

    /* These are mine: used to hint the compiler in variadic printf-like
     * functions, this way it will warn if format/arguments are incorrect */
    /* printf-like variadic arguments (format is first, args from second) */
    #define __va_printf     __attribute__((format(printf, 1, 2)))
    /* fprintf-like variadic arguments (format is second, args from third */
    #define __va_fprintf    __attribute__((format(printf, 2, 3)))
#else
    #define inline          /* no inline */
    #define __pure          /* no pure */
    #define __constfn       /* no const */
    #define __noreturn      /* no noreturn */
    #define __malloc        /* no malloc */
    #define __must_check    /* no warn_unused_result */
    #define __deprecated    /* no deprecated */
    #define __used          /* no used */
    #define __UNUSED        /* no unused */
    #define __packed        /* no packed */
    #define likely(x)       (x)
    #define unlikely(x)     (x)

    #define __va_printf
    #define __va_fprintf
#endif

/*
 * Uber-anal glib usage: disallow stdlib's functions where glib provides
 * their own
 */
#if 0 && __GNUC__
    #pragma GCC poison printf sprintf
#endif

#ifdef _DEBUG
    /* Debugging facilities */

    /* Prefix for messages */
    #define FSL_                  "nautilus-follow-symlink: "

    enum {
        FINE = 1,
        FINER,

        TRACE = FINER,
    };

    /* Check on runtime the environment variable's value
     *
     * (set to 1 to enable verbosity, to 2 for extra verbosity)
     */
    static inline int VERBOSITY_LEVEL(void)
    {
        const char * const DBGENV = getenv("FSL_DBG");
        if (NULL == DBGENV || 0 == strcmp(DBGENV, "0")) {
            return 0;
        }
        else if (0 == strcmp(DBGENV, "2")) {
            return 2;
        }
        return 1;
    }

    /* Informational message shown on initialisation */
    static inline void FSL_DEBUG_INIT(void)
    {
        g_print( FSL_ "DEBUG mode is available, and ");
        g_printf(" set to %d.\n", VERBOSITY_LEVEL());
        g_print( FSL_ "set the environment variable FSL_DBG to \n"
                 FSL_ "1 to enable it or to any other value to disable it.\n");
    }

    /* Display the name of the current function name */
    #define TRACE() FSL_LOG_WITH_LEVEL(TRACE, FSL_ "trace: %s()", __func__);

    /*
     * Display a log message with a given log level if the level
     * is at least VERBOSITY_LEVEL().
     * Same as FSL_LOG_WITH_LEVEL but taking a va_list, this function
     * provides the implementation used by the other FSL_LOG_*'s
     */
    static inline void __UNUSED FSL_LOG_WITH_LEVEL_IMPL(int level,
                                              gchar * const format,
                                              va_list ap)
    {
        if (VERBOSITY_LEVEL() >= level) {
            g_vprintf(format, ap);
            g_print("\n");
        }
    }

    /*
     * Display a log message with a given log level if the level
     * is at least VERBOSITY_LEVEL().
     */
    static void __UNUSED __va_fprintf FSL_LOG_WITH_LEVEL(int level,
                                                         gchar * const format,
                                                         ...)
    {
        va_list ap;
        va_start(ap, format);

        FSL_LOG_WITH_LEVEL_IMPL(level, format, ap);

        va_end(ap);
    }

    /* Display a message
     *
     * NOTE: Variadic functions can't be inlined
     */
    static void __UNUSED __va_printf FSL_LOG(gchar * const format, ...)
    {
         va_list ap;
         va_start(ap, format);

         FSL_LOG_WITH_LEVEL_IMPL(FINE, format, ap);

         va_end(ap);
    }

    /* Display a message if a condition is true
     *
     * NOTE: Variadic functions can't be inlined
     */
    static void __UNUSED __va_fprintf FSL_LOG_COND(int cond, gchar * const format, ...)
    {
        if (cond) {
            va_list ap;
            va_start(ap, format);

            FSL_LOG_WITH_LEVEL_IMPL(FINE, format, ap);

            va_end(ap);
        }
    }

    /* see below for an explanation */
    #define FSL_LOG1(s) FSL_LOG(s)
#else
    /* Debugging facilities disabled */
    #define FINE
    #define FINER
    #define TRACE()
    #define FSL_DEBUG_INIT()
    /* With variadic functions there's no way (AFAIK) to provide empty
     * alternative macros that won't raise a compiler error if no variable
     * arguments are given, hence this hackish FSL_LOG1, for the cases
     * in which just an argument is used
     */
    #define FSL_LOG1(s)
    #define FSL_LOG(f, ...)
    ///* Unneeded for the time being */ #define FSL_LOG_WITH_LEVEL(l,f,rest...)
    #define FSL_LOG_COND(c,f,...)
#endif

#endif /* FOLLOW_SYMLINK_COMMON_H */
/* vim:set ts=4 et ai: */

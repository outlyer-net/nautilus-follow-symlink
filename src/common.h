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

#if !defined(__fsl_unused)
    // Xref: http://rlove.org/log/2005102601
    #if __GNUC__ >= 3
        #define __fsl_unused   __attribute__ ((unused))
    #else
        #define __fsl_unused
    #endif
#endif

#ifdef _DEBUG
    /* Debugging facilities */

    /* Prefix for messages */
    #define FSL_                  "nautilus-follow-symlink: "
    /* Environment variable, set to 1 to enable verbosity */
    #define DBGENV_               (getenv("FSL_DBG"))
    /* Check on runtime the environment variable's value */
    #define DEBUG_ON_()           (DBGENV_ != NULL && 0 == strcmp(DBGENV_,"1"))

    /* Informational message shown on initialisation */
    #define FSL_DEBUG_INIT()      { \
        const int ENABLED = DEBUG_ON_(); \
        g_print( FSL_ "DEBUG mode is available, and "); \
        g_print( (ENABLED) ? "enabled.\n" : "disabled.\n"); \
        g_print( FSL_ "set the environment variable FSL_DBG to \n" \
                 FSL_ "1 to enable it or to any other value to disable it.\n"); \
    };

    /* Display the name of the current function name */
    #define TRACE()               if (DEBUG_ON_())\
                    g_printf("nautilus-follow-symlink trace: %s()\n", __FUNCTION__);
    /* Display a message */
    #define FSL_LOG(str)          if (DEBUG_ON_()) g_printf("%s\n", (str));
    /* Display a formatted message with one string argument */
    #define FSL_LOG1(str1, str2)  if (DEBUG_ON_()) g_printf("%s %s\n", (str1), (str2));
    #define FSL_LOG_SPRINTF1(s1, s2)    if (DEBUG_ON_()) g_printf((s1), (s2));
#else
    /* Debugging facilities disabled */
    #define TRACE()
    #define FSL_LOG(a)
    #define FSL_LOG1(a,b)
    #define FSL_DEBUG_INIT()
    #define FSL_LOG_SPRINTF1(a,b)
#endif

#endif /* FOLLOW_SYMLINK_COMMON_H */
/* vim:set ts=4 et ai: */

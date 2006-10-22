#ifndef FOLLOW_SYMLINK_COMMON_H
#define FOLLOW_SYMLINK_COMMON_H

#include <glib.h>

#ifdef _DEBUG
#define TRACE()    (g_printf("nautilus-follow-symlink trace: %s()\n", __FUNCTION__));
#else
#define TRACE()
#endif

#ifndef VERSION
#define VERSION "interim.debug"
#endif

#endif /* FOLLOW_SYMLINK_COMMON_H */
/* vim:set ts=4 et ai: */

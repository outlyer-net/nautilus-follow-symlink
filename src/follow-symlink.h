#ifndef FOLLOW_SYMLINK_H
#define FOLLOW_SYMLINK_H

/*
 * This file contains nautilus-follow-symlink's private interface,
 * its core functionality
 */

#include "common.h"

#include <libnautilus-extension/nautilus-menu-provider.h>

#include <glib/gprintf.h>
#include <sys/stat.h>
#include <errno.h>  /* errno (3) */
#include <stdlib.h> /* realpath() (3) */
#include <string.h> /* strlen(), strerror() (3) */

/* Static Prototypes */

static void fsl_callback(NautilusMenuItem *, NautilusFileInfo *);

static GList * fsl_get_file_items(NautilusMenuProvider *, GtkWidget *, GList *);

static GList * fsl_get_background_items(NautilusMenuProvider *, GtkWidget *, NautilusFileInfo *);

static GList * fsl_get_items_impl(GtkWidget *, NautilusFileInfo *, gboolean);

static NautilusMenuItem * fsl_menu_item_new(GdkScreen *, gboolean);

/* Exported Prototypes
 * Here the namespace is a bit more explicit just in case
 */
void fsl_extension_menu_provider_iface_init(NautilusMenuProviderIface *);

#endif /* FOLLOW_SYMLINK_H */
/* vim:set ts=4 et ai: */

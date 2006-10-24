#ifndef FOLLOW_SYMLINK_H
#define FOLLOW_SYMLINK_H

/*
 * This file contains nautilus-follow-symlink's private interface,
 * its core functionality
 */

#include <libnautilus-extension/nautilus-menu-provider.h>

#include "common.h"

/* Static Prototypes */

static void fsl_callback(NautilusMenuItem *, NautilusFileInfo *);

static GList *fsl_get_file_items(NautilusMenuProvider *, GtkWidget *, GList *);

static NautilusMenuItem* fsl_menu_item_new(GdkScreen *, gboolean);

/* Exported Prototypes
 * Here the namespace is a bit more explicit just in case
 */
void fsl_extension_menu_provider_iface_init(NautilusMenuProviderIface *);

#endif /* FOLLOW_SYMLINK_H */
/* vim:set ts=4 et ai: */

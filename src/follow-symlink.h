/*
 *  follow-symlink.h
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

#ifndef FOLLOW_SYMLINK_H
#define FOLLOW_SYMLINK_H

/*
 * This file contains nautilus-follow-symlink's private interface,
 * its core functionality
 */

#include "common.h"

#include <libnautilus-extension/nautilus-menu-provider.h>

#include <glib/gprintf.h>
#include <errno.h>  /* errno (3) */
#include <string.h> /* strlen(), strerror() (3) */
#include <assert.h>

/* Static Prototypes */

static void fsl_callback(NautilusMenuItem *, NautilusFileInfo *);

// This signature can't be changed
static GList * fsl_get_file_items(NautilusMenuProvider *,
                                  GtkWidget *,
                                  GList *);

// This signature can't be changed
static GList * fsl_get_background_items(NautilusMenuProvider *,
                                        GtkWidget *,
                                        NautilusFileInfo *);

static inline GList * fsl_get_items_impl(GtkWidget *,
                                         NautilusFileInfo *,
                                         gboolean,
                                         GList *);

static NautilusMenuItem * fsl_menu_item_new(GdkScreen *, gboolean, const gchar *);

static inline __pure gboolean file_is_directory(const gpointer const);

static gsize printf_string_upper_bound(const gchar *, ...);

/* Exported Prototypes
 * Here the namespace is a bit more explicit just in case
 */
void fsl_extension_menu_provider_iface_init(NautilusMenuProviderIface *);

#endif /* FOLLOW_SYMLINK_H */
/* vim:set ts=4 et ai: */

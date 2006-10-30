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

#ifndef NAUTILUS_EXT_FOLLOW_SYMLINK_H
#define NAUTILUS_EXT_FOLLOW_SYMLINK_H

#include "common.h"

#include <glib-object.h>
#include <libnautilus-extension/nautilus-menu-provider.h>

#include <locale.h>
#include <libintl.h>

#include <glib/gprintf.h>

/*
 * This file contains nautilus-follow-symlink's "public" interface,
 * the functions required to bind the extension to nautilus
 */

void nautilus_module_initialize(GTypeModule *);

void nautilus_module_shutdown(void);

void nautilus_module_list_types(const GType **, int *);

/* These ones don't need public visibility */

static void fsl_register_type(GTypeModule *);

#if 0
static __constfn GType fsl_get_type(void);
#endif

/* Data Types */

struct _FsymlinkExtensionClass {
    GObjectClass parent_slot;
};

struct _FsymlinkExtension {
    GObject parent_slot;
};

typedef struct _FsymlinkExtensionClass FsymlinkExtensionClass;

typedef struct _FsymlinkExtension FsymlinkExtension;

/* Data initializers, not really needed */
#if 0
static void fsl_class_init (FsymlinkExtensionClass *class);

static void fsl_instance_init (FsymlinkExtension *cvs);
#endif

/* Defined in the private interface */
extern void fsl_extension_menu_provider_iface_init(NautilusMenuProviderIface *iface);

#endif /* NAUTILUS_EXT_FOLLOW_SYMLINK_H */
/* vim:set ts=4 et ai: */

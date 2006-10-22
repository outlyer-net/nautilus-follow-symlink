#ifndef NAUTILUS_EXT_FOLLOW_SYMLINK_H
#define NAUTILUS_EXT_FOLLOW_SYMLINK_H

#include <glib-object.h>
#include <libnautilus-extension/nautilus-menu-provider.h>

#include "common.h"

/*
 * This file contains nautilus-follow-symlink's "public" interface,
 * the functions required to bind the extension to nautilus
 */

void nautilus_module_initialize(GTypeModule *);

void nautilus_module_shutdown(void);

void nautilus_module_list_types(const GType **, int *);

/* These ones don't need public visibility */

static void fsl_register_type(GTypeModule *);

static GType fsl_get_type(void);

/* Data Types */

struct _FsymlinkExtensionClass {
    GObjectClass parent_slot;
};

struct _FsymlinkExtension {
    GObject parent_slot;
};

typedef struct _FsymlinkExtensionClass FsymlinkExtensionClass;

typedef struct _FsymlinkExtension FsymlinkExtension;

/* Data initializers */
static void fsl_class_init (FsymlinkExtensionClass *class);

static void fsl_instance_init (FsymlinkExtension *cvs);

/* Defined in the private interface */
extern void fsl_extension_menu_provider_iface_init(NautilusMenuProviderIface *iface);

#endif /* NAUTILUS_MODULE_FOLLOW_SYMLINK_H */
/* vim:set ts=4 et ai: */

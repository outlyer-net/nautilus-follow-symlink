/*  $Id$
 *
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

#include "nautilus-ext-follow-symlink.h"

/* Public interface */
static GType fsl_type;
static GType provider_types[1];

void nautilus_module_initialize (GTypeModule  *module)
{
    TRACE();
    FSL_DEBUG_INIT();

    setlocale(LC_ALL, "");
    bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
    textdomain(GETTEXT_PACKAGE);

    g_printf("Initializing nautilus-follow-symlink extension (v%s)\n", VERSION);

    fsl_register_type(module);
# if 0
    provider_types[0] = fsl_get_type();
#endif
    provider_types[0] = fsl_type;
}

void nautilus_module_shutdown (void)
{
    TRACE();

    /* Module-specific shutdown */
    g_print ("Shutting down nautilus-follow-symlink extension\n");
}

void nautilus_module_list_types (const GType **types, int *num_types)
{
    TRACE();

    *types = provider_types;
    *num_types = G_N_ELEMENTS(provider_types);
}

void fsl_register_type (GTypeModule * module)
{
    TRACE();

    static const GTypeInfo info = {
        sizeof(FsymlinkExtensionClass),
        (GBaseInitFunc) NULL,
        (GBaseFinalizeFunc) NULL,
        //TODO: Does nullifyng this have side effects?
        NULL, //(GClassInitFunc) fsl_class_init,
        NULL,
        NULL,
        sizeof (FsymlinkExtension),
        0,
        NULL, //(GInstanceInitFunc) fsl_instance_init,
    };

    fsl_type = g_type_module_register_type (module,
                                            G_TYPE_OBJECT,
                                            "FsymlinkExtension",
                                            &info, 0);
    /* Menu provider interface */
    static const GInterfaceInfo menu_provider_iface_info = {
        (GInterfaceInitFunc)fsl_extension_menu_provider_iface_init,
        NULL,
        NULL,
    };

    g_type_module_add_interface(module, fsl_type,
            NAUTILUS_TYPE_MENU_PROVIDER, &menu_provider_iface_info);

    /* Other Interfaces */
}

#if 0
GType fsl_get_type(void)
{
    TRACE();

    return fsl_type;
}
#endif

#if 0
void fsl_instance_init(FsymlinkExtension *instance)
{
    TRACE();
}

void fsl_class_init(FsymlinkExtensionClass *class)
{
    TRACE();
}
#endif

/* vim:set ts=4 et ai: */

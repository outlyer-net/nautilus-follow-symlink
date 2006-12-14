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

#include "follow-symlink.h"

extern int errno;

/* Menu initialization */
void fsl_extension_menu_provider_iface_init(NautilusMenuProviderIface *iface)
{
    TRACE();

    iface->get_file_items = fsl_get_file_items;
    iface->get_background_items = fsl_get_background_items;
}

/* Implementation of the menu attachment,
 * this is slightly different whith file items and with background (one folder)
 * items, but shares most of the code, so the common part is here.
 */
GList * fsl_get_items_impl(GtkWidget * window,
                           NautilusFileInfo * file_info,
                           gboolean is_file_item,
                           GList * input)
{
    TRACE();

    NautilusMenuItem *item;

    // Only file uris
    {
        // TODO: what about GnomeVFSFileInfo's is_local ?
        gchar * uri_scheme = nautilus_file_info_get_uri_scheme(file_info);
        if (strcmp(uri_scheme, "file") != 0) {
            FSL_LOG1( "Not file scheme" );
            return NULL;
        }
        g_free(uri_scheme);
    }

    // We know the file is either a directory or a symlink to a directory
    // TODO: Has glib/gnome any better/faster alternatives?
    GnomeVFSFileInfo * gfi = nautilus_file_info_get_vfs_file_info(file_info);

    /* TODO: In which situations might the flags field be invalid?
     * Hence, can the older stat version be dumped safely?
     */
    g_assert( (gfi->valid_fields & GNOME_VFS_FILE_INFO_FIELDS_FLAGS) > 0 );

    if ( (gfi->flags & GNOME_VFS_FILE_FLAGS_SYMLINK) == 0 ) {
        FSL_LOG("GnomeVFS Flags: ! SYMLINK in %s", nautilus_file_info_get_uri(file_info));
        return NULL;
    }

    item = fsl_menu_item_new(gtk_widget_get_screen(window),
                             is_file_item,
                             nautilus_file_info_get_name(file_info));
    g_signal_connect(item, "activate", G_CALLBACK(fsl_callback), file_info);

    return g_list_append(input, item);
}

GList *
fsl_get_background_items(NautilusMenuProvider * provider __UNUSED,
                         GtkWidget * window,
                         NautilusFileInfo * current_folder)
{
    TRACE();

    if (NULL == current_folder) { // XXX: Does this ever happen?
        FSL_LOG1( "No folder selected");
     }
    return fsl_get_items_impl(window, current_folder, FALSE, NULL);
}


gboolean file_is_directory (const gpointer const file_data)
{
    TRACE();

    /*
     * Apparently type is never GNOME_VFS_FILE_TYPE_SYMBOLIC_LINK and symlinks
     * are resolved to the target type
     */
    const GnomeVFSFileInfo * const gfi = nautilus_file_info_get_vfs_file_info(file_data);
    return gfi->type == GNOME_VFS_FILE_TYPE_DIRECTORY;
}

/*
 * Bind to the menu if needed
 *
 * 
 */
GList * fsl_get_file_items (NautilusMenuProvider * provider __UNUSED,
                            GtkWidget * window,
                            GList * files)
{
    TRACE();

    // Number of files = g_list_length(files)
    // Directory = nautilus_file_info_is_directory(files->data)
    if (NULL == files) {
        FSL_LOG1("No file selected");
        return NULL;
    }

    assert( g_list_length(files) > 0 );

    FSL_LOG_COND( g_list_length(files) > 1, "More than one file selected (%d)", g_list_length(files) );

    GList * items = NULL;

    for (int i=0; i<g_list_length(files); ++i) {
        const gpointer file_info = g_list_nth_data(files, i);
        if (!file_is_directory(file_info)) {
            FSL_LOG ( "File %s is not a directory, discarded",
                      nautilus_file_info_get_name(file_info) );
            continue;
        }
        FSL_LOG( "%s is a directory", nautilus_file_info_get_name(file_info) );
        // TODO: Am I loosing memory?
        GList * ret = fsl_get_items_impl(window, file_info, TRUE, items);
        if (NULL != ret) {
            items = ret;
        }
    }

    return items;
}

/*
 * Opens the real path of the symbolic link
 *
 * file_info: The symbolic link
 */
void fsl_callback (NautilusMenuItem * item __UNUSED, NautilusFileInfo * file_info)
{
    TRACE();

    gchar ** argv;
    const GnomeVFSFileInfo * gfi = nautilus_file_info_get_vfs_file_info(file_info);
    // See  /usr/include/gnome-vfs-2.0/libgnomevfs/gnome-vfs-file-info.h,
    // this one is the "realpath()" (3), also it isn't urlencoded
    const gchar const * target = gfi->symlink_name;

    const gchar const * BASE_CMD = "nautilus --no-desktop --no-default-window '%s'";
    const gsize mem_block_size = printf_string_upper_bound(BASE_CMD, target);
    gchar * command_line = g_try_malloc( mem_block_size );

    if (NULL == command_line) {
        g_printerr(__FILE__ ":%d: Failed to allocate enough memory "
                   "for command line, can't spawn new nautilus.\n", __LINE__);
        // Redundant, but issues a CRITICAL message
        g_return_if_fail( NULL != command_line );
    }

    g_sprintf(command_line, BASE_CMD, target);

    if (FALSE == g_shell_parse_argv(command_line, NULL, &argv, NULL)) {
        g_free(command_line);

        g_printerr("Failed in creating the arguments for the child nautilus.\n");
        //g_return_if_fail( FALSE );
        g_return_if_reached();
    }

    g_printf("nautilus-follow-symlink: Spawning nautilus with\n '%s'\n", command_line);

    g_spawn_async( NULL, // Inherit CWD
                   argv,
                   NULL,
                   G_SPAWN_SEARCH_PATH, //| G_SPAWN_DO_NOT_REAP_CHILD,
                   NULL, NULL, NULL, NULL);

    g_free(command_line);
    g_strfreev(argv);
}

/*
 * Creates the new menu item
 *
 * is_file_item: TRUE if we're working over a (selected) file, FALSE if working
 *               over a (opened) folder
 * base_name: file name, without path, of the given file
 */
NautilusMenuItem * fsl_menu_item_new(GdkScreen *screen __UNUSED,
                                     gboolean is_file_item,
                                     const gchar * a_base_name)
{
    TRACE();

    NautilusMenuItem *ret;

    gchar * name, * tooltip, * fmt_name, * fmt_tooltip, * unique_name;

    if (is_file_item) {
        fmt_name = _("Follow symbolic _link '%s'");
        fmt_tooltip = _("Open the directory pointed by the symbolic link '%s'");
    }
    else {
        fmt_name = _("Open _real path of '%s'");
        fmt_tooltip = _("Open the real path of the folder pointed by '%s'");
    }

    gchar * base_name = (gchar*)a_base_name;

    // Replace any _ in the file name with __ (to display correctly in the
    // context menu)
    {
        // Count them
        size_t count = 0;
        for (size_t i=0; i<strlen(base_name); ++i) {
            if (*(base_name + i) == '_') {
                ++count;
            }
        }

        // Escape the string if needed
        if (count > 0) {
            gchar * escaped_name = g_malloc( (strlen(base_name) + count)*sizeof(gchar) );
            gchar * src = base_name, * dst = escaped_name;

            while (count > 0) {
                const gchar c = *src;
                if (c == '_') {
                    *dst = '_';
                    dst++;

                    --count;
                }
                *dst = *src;
                dst++; src++;
            }
            g_stpcpy(dst, src);
            base_name = escaped_name;
        }
    }

    // Trial and error showed that the menu item name must be different
    // when various are to be shown (multiple selections), and also that the
    // name should always be the same for a given file, hence the base name is
    // appended to the command name
    static const gchar * const ITEM_NAME_FMT = "FsymlinkExtension::follow_symlink_%s";

    name = g_try_malloc( printf_string_upper_bound(fmt_name, base_name) );
    tooltip = g_try_malloc( printf_string_upper_bound(fmt_tooltip, a_base_name) );
    unique_name = g_try_malloc( printf_string_upper_bound(ITEM_NAME_FMT, a_base_name) );


    if (NULL == name || NULL == tooltip || NULL == unique_name) {
        // Let's try to keep nautilus going if no allocation is possible

        if (name) g_free(name);
        if (tooltip) g_free(tooltip);
        if (unique_name) g_free(unique_name);

        g_printerr(__FILE__ ":%d: Failed to allocate enough memory for "
                   "the new menu item, dummier menu item in use.\n", __LINE__);

        // Can't return NULL neither a nautilus_menu_item_new(NULL,NULL,NULL,NULL)
        ret = nautilus_menu_item_new("Fsymlink::allocation_error",
                                     _("Follow symbolic _link"),
                                     _("Open the symbolic link"),
                                     FSL_ICON);
        g_return_val_if_fail(NULL!=name && NULL!=tooltip && NULL!=unique_name,
                             ret);
    }

    g_sprintf(name, fmt_name, base_name);
    g_sprintf(tooltip, fmt_tooltip, a_base_name);
    g_sprintf(unique_name, ITEM_NAME_FMT, a_base_name);

    // (name, label, tip, icon)
    ret = nautilus_menu_item_new(unique_name, name, tooltip, FSL_ICON);

    if (base_name != a_base_name) {
        g_free(base_name);
    }
    g_free(unique_name);
    g_free(name);
    g_free(tooltip);

    //g_object_set_data(G_OBJECT(ret), "FsymlinkExtension::screen", screen);
    return ret;
}

/*** Utility(-es) ***/

/*
 * Wrapper for g_printf_string_upper_bound with a variadic signature
 *
 * Returns the maximum space needed to store the formatted string
 */
gsize printf_string_upper_bound(const gchar * format, ...) {
    va_list ap;
    va_start(ap, format);

    gsize retval = g_printf_string_upper_bound(format, ap);

    va_end(ap);

    return retval;
}

/* vim:set ts=4 et ai: */

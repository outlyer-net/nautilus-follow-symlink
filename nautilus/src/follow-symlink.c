/*  $Id$
 *
 *  follow-symlink.c
 *  nautilus-follow-symlink: Nautilus extension which allows opening the real
 *                           path of symbolic links
 *
 *   Copyright (C) 2006, 2008, 2009, 2011, 2012 Toni Corvera
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

#include <gio/gio.h>
#include <gtk/gtk.h>

// Symbolic name of the icon, use to be in /usr/share/icons/gnome/[16x16]/emblems/
#define FSL_ICON_NAME "emblem-symbolic-link"

extern int errno;

// References for gio and the newer nautilus-extension: (required to support GNOME 2.22)
// http://library.gnome.org/devel/gio/unstable/GFile.html
// http://library.gnome.org/devel/gio/unstable/GFileInfo.html
// http://library.gnome.org/devel/libnautilus-extension/unstable/libnautilus-extension-nautilus-file-info.html

#if 0 // See comments below
/* Obtains the icon path
 * The returned string should be freed with g_free()
 *
 * If the real icon path can't be obtained, a symbolic icon name is returned
 * instead.
 *
 * XXX: This is technically unneeded, passing FSL_ICON_NAME is enough for nautilus
 *      to resolve the icon in GNOME3.
 *      But neither method seems to work anymore in GNOME2 for some
 *      reason, other plugins don't show icons either so I guess that's normal.
 */
static gchar * fsl_icon_name() {
    TRACE();

    GtkIconTheme * git = gtk_icon_theme_get_default(); // Don't deallocate
    g_assert( NULL != git );
    GtkIconInfo * gii = gtk_icon_theme_lookup_icon(git,
                FSL_ICON_NAME,
                16, // Size
                GTK_ICON_LOOKUP_GENERIC_FALLBACK); // Flags
    if (NULL == gii) {
        // Lookup failed
        return g_strdup(FSL_ICON_NAME);    
    }
    // gtk_icon_info_get_filename() returns an string owned by
    // GTK. It was quickly overwritten in my tests.
    // g_strdup() is safe to be called on NULL (return NULL)
    gchar * fn = g_strdup(gtk_icon_info_get_filename(gii));
    gtk_icon_info_free(gii);
    if (NULL == fn) { // No filename associated
        return g_strdup(FSL_ICON_NAME);
    }
    return fn;
}
#endif

/* Menu initialization */
void fsl_extension_menu_provider_iface_init(NautilusMenuProviderIface *iface)
{
    TRACE();

    iface->get_file_items = fsl_get_file_items;
    iface->get_background_items = fsl_get_background_items;
}

/* Implementation of the menu attachment,
 * this is slightly different with file items and with background (one folder)
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

    // TODO: Once the older code is deprecated, some nautilus_* function can be
    //          translated to their g_file_* counterparts.

    // Only process symlinks to directories, we know already the file is a
    // directory or a symlink to one
    GFile * gf = nautilus_file_info_get_location(file_info); // Get the pointed GFile
    // FIXME: Can NULL be passed as last argument???
    GFileInfo* gfi = g_file_query_info(gf,
                                       G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK,
                                       G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, // <== Important :P
                                       NULL,
                                       NULL); // Retrieve the symlink attribute

    if (!g_file_info_get_is_symlink(gfi)) {
        FSL_LOG("No " G_FILE_ATTRIBUTE_STANDARD_IS_SYMLINK  " attribute in %s",
                                         nautilus_file_info_get_uri(file_info));
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
     * The "effective" type is returned, a symlink to a directory is a directory
     */
    return G_FILE_TYPE_DIRECTORY == nautilus_file_info_get_file_type(file_data);
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

    GFile * gf = nautilus_file_info_get_location(file_info); // Get the pointed GFile
    // FIXME: Can NULL be passed as last argument???
    GFileInfo* gfi = g_file_query_info(gf,
                                       G_FILE_ATTRIBUTE_STANDARD_SYMLINK_TARGET,
                                       G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS, // <== Important :P
                                       NULL,
                                       NULL); // Retrieve the symlink attribute

    // The result will relative if the symlink is, must use the correct CWD
    const char * target = g_file_info_get_symlink_target(gfi);

    GFile * parent = nautilus_file_info_get_parent_location(file_info);

    gchar * escaped_tgt = g_shell_quote(target);
    gchar * command_line = g_strdup_printf(NAUTILUS_COMMAND " --no-desktop --no-default-window %s", escaped_tgt);
    g_free(escaped_tgt);

    if (G_UNLIKELY(FALSE == g_shell_parse_argv(command_line, NULL, &argv, NULL))) {
        g_free(command_line);
        g_printerr("Failed preparing " NAUTILUS_COMMAND " command-line.\n");
        g_return_if_reached();
    }

    //g_printf(FSL_MSG_PREFIX ": Spawning with [PWD=%s]\n `%s`\n", g_file_get_path(parent), command_line);
    g_printf(FSL_MSG_PREFIX ": Spawning with\n `%s`\n", command_line);

    g_spawn_async( g_file_get_path(parent), // Inherit CWD
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

    // g_try_malloc would allow nautilus to stay up even if some allocation fails
    // but we'd be in trouble anyway. Apparently returning NULL is not an option either.
    name = g_malloc( printf_string_upper_bound(fmt_name, base_name) );
    tooltip = g_malloc( printf_string_upper_bound(fmt_tooltip, a_base_name) );
    unique_name = g_malloc( printf_string_upper_bound(ITEM_NAME_FMT, a_base_name) );

    g_sprintf(name, fmt_name, base_name);
    g_sprintf(tooltip, fmt_tooltip, a_base_name);
    g_sprintf(unique_name, ITEM_NAME_FMT, a_base_name);

    // (name, label, tip, icon)
    ret = nautilus_menu_item_new(unique_name, name, tooltip, FSL_ICON_NAME);

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

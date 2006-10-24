#include "follow-symlink.h"

// Offset at char 7 to remove file://
static const unsigned short URI_OFFSET = 7 * sizeof(gchar);
static const size_t PATH_LENGTH_BYTES = sizeof(gchar) * (PATH_MAX + 1);

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
                           gboolean is_file_item)
{
    TRACE();

    NautilusMenuItem *item;

    // Only file uris
    {
        gchar * uri_scheme = nautilus_file_info_get_uri_scheme(file_info);
        if (strcmp(uri_scheme, "file") != 0) {
            FSL_LOG( "Not file scheme" );
            return NULL;
        }
        g_free(uri_scheme);
    }

    // We know the file is either a directory or a symlink to a directory
    // TODO: Has glib/gnome any better/faster alternatives?
    {
        struct stat file_stat;
        // Note ..._get_name doesn't give the full path
        const gchar * const file_name = nautilus_file_info_get_uri(file_info) + URI_OFFSET;
        lstat(file_name, &file_stat);
        if (! S_ISLNK(file_stat.st_mode)) {
            FSL_LOG1( "Not S_ISLNK:", file_name );
            return NULL;
        }
    }

    item = fsl_menu_item_new(gtk_widget_get_screen(window), is_file_item);
    g_signal_connect(item, "activate", G_CALLBACK(fsl_callback),
                     file_info);

    return g_list_append(NULL, item);
}

GList *
fsl_get_background_items(NautilusMenuProvider * provider,
                         GtkWidget * window,
                         NautilusFileInfo * current_folder)
{
    TRACE();

    if (NULL == current_folder) { // XXX: Does this ever happen?
        FSL_LOG( "No folder selected" );
     }
    return fsl_get_items_impl(window, current_folder, FALSE);
}

/* Bind to menu if needed */
GList *
fsl_get_file_items (NautilusMenuProvider * provider,
                                   GtkWidget * window,
                                   GList * files)
{
    TRACE();

    // Number of files = g_list_length(files)
    // Directory = nautilus_file_info_is_directory(files->data)

    if (NULL==files || g_list_length(files) != 1) {
        FSL_LOG( (NULL==files) ? "No file" : "More than one file" );
        return NULL;
    }

    GnomeVFSFileInfo * gfi = nautilus_file_info_get_vfs_file_info(files->data);
    /*
     * Aparently type is never GNOME_VFS_FILE_TYPE_SYMBOLIC_LINK and symlinks
     * are resolved to the target type
     */
    if (gfi->type != GNOME_VFS_FILE_TYPE_DIRECTORY) {
        FSL_LOG( "Not directory" );
        return NULL;
    }


    return fsl_get_items_impl(window, files->data, TRUE);
}

void fsl_callback (NautilusMenuItem * item, NautilusFileInfo * file_info)
{
    TRACE();

    gchar ** argv;
    const gchar * link_name = nautilus_file_info_get_uri(file_info) + URI_OFFSET;
    gchar * target = g_malloc(PATH_LENGTH_BYTES);

    /* unlike readlink(man 2), realpath(man 3) resolves the symlink, while
     * readlink returns the pointed file, which might be a relative path
     * Xref: <http://www.gnu.org/software/libc/manual/html_node/Symbolic-Links.html>
     */
    if (NULL == realpath(link_name, target)) {
        g_printf("ERROR in realpath(): %s\n", strerror(errno));
        g_assert( FALSE );
    }

    const gchar const * BASE_CMD = "nautilus --no-desktop --no-default-window \"";
    gchar * command_line = g_malloc( sizeof(gchar) * ( strlen(BASE_CMD) + strlen(target) + 2 ) );

    gchar * offset = g_stpcpy(command_line, BASE_CMD);
    //offset = g_stpcpy(offset, "file://"); // unneeded; also makes nautilus think it may be incorrect
    //                                         if it contains spaces (instead of %20's)
    offset = g_stpcpy(offset, target);
    g_stpcpy(offset, "\"");

    if (FALSE == g_shell_parse_argv(command_line, NULL, &argv, NULL)) {
        g_assert( FALSE );
    }

    g_printf("nautilus-follow-symlink: Spawning nautilus with\n '%s'\n", command_line);

    g_spawn_async( nautilus_file_info_get_parent_uri(file_info) + URI_OFFSET,
                   argv,
                   NULL,
                   G_SPAWN_SEARCH_PATH,
                   NULL, NULL, NULL, NULL);

    g_free(command_line);
    g_strfreev(argv);
}

/* Create the new menu item */
NautilusMenuItem *
fsl_menu_item_new(GdkScreen *screen, gboolean is_file_item)
{
    TRACE();

    NautilusMenuItem *ret;
    const char *name;
    const char *tooltip;

    if (is_file_item) {
        name = _("Follow symbolic _link");
        tooltip = _("Open the directory pointed by the currently selected symbolic link");
    }
    else {
        name = _("Open real path");
        tooltip = _("Open the real path of the folder pointed by this symbolic link");
    }

    // (name, label, tip, icon)
    ret = nautilus_menu_item_new("FsymlinkExtension::follow_symlink",
                name, tooltip, NULL);
    //g_object_set_data(G_OBJECT(ret), "FsymlinkExtension::screen", screen);
    return ret;
}

/* vim:set ts=4 et ai: */

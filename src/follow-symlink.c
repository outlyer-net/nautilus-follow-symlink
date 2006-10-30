#include "follow-symlink.h"

// Offset at char 7 to remove file://
//static const unsigned short URI_OFFSET = 7 * sizeof(gchar);
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
fsl_get_background_items(NautilusMenuProvider * provider __unused,
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
GList * fsl_get_file_items (NautilusMenuProvider * provider __unused,
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
void fsl_callback (NautilusMenuItem * item __unused, NautilusFileInfo * file_info)
{
    TRACE();

    gchar ** argv;
    const GnomeVFSFileInfo * gfi = nautilus_file_info_get_vfs_file_info(file_info);
    // See  /usr/include/gnome-vfs-2.0/libgnomevfs/gnome-vfs-file-info.h,
    // this one is the "realpath()" (3), also it isn't urlencoded
    const gchar const * target = gfi->symlink_name;

    const gchar const * BASE_CMD = "nautilus --no-desktop --no-default-window '%s'";
    gchar * command_line = g_malloc( sizeof(gchar) * ( strlen(BASE_CMD) + strlen(target) + 1 ) );
    g_sprintf(command_line, BASE_CMD, target);

    if (FALSE == g_shell_parse_argv(command_line, NULL, &argv, NULL)) {
        g_assert( FALSE );
    }

    g_printf("nautilus-follow-symlink: Spawning nautilus with\n '%s'\n", command_line);

    // FIXME: const gchar * cwd = nautilus_file_info_get_parent_uri(file_info) + URI_OFFSET;
    // TODO: does the cwd used for spawn have any side-effect ?
    g_spawn_async( ".",
                   argv,
                   NULL,
                   G_SPAWN_SEARCH_PATH,
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
NautilusMenuItem * fsl_menu_item_new(GdkScreen *screen __unused,
                                     gboolean is_file_item,
                                     const gchar * base_name)
{
    TRACE();

    NautilusMenuItem *ret;

    char * name;
    char * tooltip;

    if (is_file_item) {
        const gchar * fmt_name = _("Follow symbolic _link '%s'");
        const gchar * fmt_tooltip = _("Open the directory pointed by the "
                                      "symbolic link '%s'");

        name = g_malloc(sizeof(gchar) * (strlen(fmt_name) + strlen(base_name)));
        tooltip = g_malloc(sizeof(gchar) * (strlen(fmt_tooltip) + strlen(base_name)));
        g_sprintf(name, fmt_name, base_name);
        g_sprintf(tooltip, fmt_tooltip, base_name);
    }
    else {
        const gchar * fmt_name = _("Open real path of '%s'");
        const gchar * fmt_tooltip = _("Open the real path of the folder "
                                      "pointed by '%s'");

        name = g_malloc(sizeof(gchar) * (strlen(fmt_name) + strlen(base_name + 1)));
        tooltip = g_malloc(sizeof(gchar) * (strlen(fmt_tooltip) + strlen(base_name + 1)));
        g_sprintf(name, fmt_name, base_name);
        g_sprintf(tooltip, fmt_tooltip, base_name);
    }

    // Trial and error showed that the menu item name must be different
    // when various are to be shown, and also that the name should always be
    // the same for a given file
    static const gchar * const ITEM_NAME_FMT = "FsymlinkExtension::follow_symlink_%s";
    gchar * unique_name = g_malloc(strlen(ITEM_NAME_FMT) + strlen(base_name));
    g_sprintf(unique_name, ITEM_NAME_FMT, base_name);
    // (name, label, tip, icon)
    ret = nautilus_menu_item_new(unique_name, name, tooltip, FSL_ICON);

    g_free(unique_name);
    g_free(name);
    g_free(tooltip);

    //g_object_set_data(G_OBJECT(ret), "FsymlinkExtension::screen", screen);
    return ret;
}

/* vim:set ts=4 et ai: */

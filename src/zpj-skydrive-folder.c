/*
 * Zapojit - GLib/GObject wrapper for the SkyDrive and Hotmail REST APIs
 * Copyright © 2012 Red Hat, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301, USA.
 */


#include "config.h"

#include "zpj-skydrive-folder.h"


G_DEFINE_TYPE (ZpjSkydriveFolder, zpj_skydrive_folder, ZPJ_TYPE_SKYDRIVE_ENTRY);


/**
 * SECTION:zpj-skydrive-folder
 * @title: ZpjSkydriveFolder
 * @short_description: Skydrive folder object.
 * @include: zpj/zpj.h
 *
 * #ZpjSkydriveFolder represents a <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#folder">
 * Skydrive folder</ulink> object.
 */


static void
zpj_skydrive_folder_init (ZpjSkydriveFolder *self)
{
}


static void
zpj_skydrive_folder_class_init (ZpjSkydriveFolderClass *class)
{
}


/**
 * zpj_skydrive_folder_new:
 * @node: A #JsonNode returned by the server.
 *
 * Creates a new #ZpjSkydriveFolder from the given @node. If you
 * already know the ID of the folder then you can use
 * zpj_skydrive_query_info_from_id().
 *
 * Returns: (transfer full): A new #ZpjSkydriveFolder. Free the
 * returned object with g_object_unref().
 */
ZpjSkydriveEntry *
zpj_skydrive_folder_new (JsonNode *node)
{
  return g_object_new (ZPJ_TYPE_SKYDRIVE_FOLDER, "json", node, NULL);
}

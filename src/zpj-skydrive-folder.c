/*
 * Zapojit - GLib/GObject wrapper for Skydrive and Hotmail
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


static void
zpj_skydrive_folder_init (ZpjSkydriveFolder *self)
{
}


static void
zpj_skydrive_folder_class_init (ZpjSkydriveFolderClass *class)
{
}


ZpjSkydriveEntry *
zpj_skydrive_folder_new (const gchar *id, const gchar *name, const gchar *description, const gchar *parent_id)
{
  return g_object_new (ZPJ_TYPE_SKYDRIVE_FOLDER,
                       "description", description,
                       "id", id,
                       "name", name,
                       "parent-id", parent_id,
                       "type", ZPJ_SKYDRIVE_ENTRY_TYPE_FOLDER,
                       NULL);
}

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

#include "zpj-skydrive-file.h"


G_DEFINE_TYPE (ZpjSkydriveFile, zpj_skydrive_file, ZPJ_TYPE_SKYDRIVE_ENTRY);


static void
zpj_skydrive_file_init (ZpjSkydriveFile *self)
{
}


static void
zpj_skydrive_file_class_init (ZpjSkydriveFileClass *class)
{
}


ZpjSkydriveEntry *
zpj_skydrive_file_new (JsonNode *node)
{
  return g_object_new (ZPJ_TYPE_SKYDRIVE_FILE, "json", node, NULL);
}

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

#include "zpj-skydrive-photo.h"


/**
 * SECTION:zpj-skydrive-photo
 * @title: ZpjSkydrivePhoto
 * @short_description: Skydrive photo object.
 * @include: zpj/zpj.h
 *
 * #ZpjSkydrivePhoto represents a <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#photo">
 * Skydrive photo</ulink> object.
 */


G_DEFINE_TYPE (ZpjSkydrivePhoto, zpj_skydrive_photo, ZPJ_TYPE_SKYDRIVE_FILE);


static void
zpj_skydrive_photo_init (ZpjSkydrivePhoto *self)
{
}


static void
zpj_skydrive_photo_class_init (ZpjSkydrivePhotoClass *class)
{
}


/**
 * zpj_skydrive_photo_new:
 * @node: A #JsonNode returned by the server.
 *
 * Creates a new #ZpjSkydrivePhoto from the given @node. If you already
 * know the ID of the photo then you can use
 * zpj_skydrive_query_info_from_id().
 *
 * Returns: (transfer full): A new #ZpjSkydrivePhoto. Free the returned
 * object with g_object_unref().
 */
ZpjSkydriveEntry *
zpj_skydrive_photo_new (JsonNode *node)
{
  return g_object_new (ZPJ_TYPE_SKYDRIVE_PHOTO, "json", node, NULL);
}

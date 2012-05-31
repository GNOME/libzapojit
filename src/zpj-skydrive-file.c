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

#include "zpj-skydrive-file.h"


/**
 * SECTION:zpj-skydrive-file
 * @title: ZpjSkydriveFile
 * @short_description: Skydrive file object.
 * @include: zpj/zpj.h
 *
 * #ZpjSkydriveFile represents a <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#file">
 * Skydrive file</ulink> object.
 */


struct _ZpjSkydriveFilePrivate
{
  goffset size;
};

enum
{
  PROP_0,
  PROP_SIZE
};


G_DEFINE_TYPE (ZpjSkydriveFile, zpj_skydrive_file, ZPJ_TYPE_SKYDRIVE_ENTRY);


static void
zpj_skydrive_file_parse_json_node (ZpjSkydriveEntry *entry, JsonNode *node)
{
  ZpjSkydriveFile *self = ZPJ_SKYDRIVE_FILE (entry);
  JsonObject *object;

  ZPJ_SKYDRIVE_ENTRY_CLASS (zpj_skydrive_file_parent_class)->parse_json_node (entry, node);

  object = json_node_get_object (node);
  self->priv->size = json_object_get_int_member (object, "size");
}


static void
zpj_skydrive_file_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  ZpjSkydriveFile *self = ZPJ_SKYDRIVE_FILE (object);

  switch (prop_id)
    {
    case PROP_SIZE:
      g_value_set_int64 (value, self->priv->size);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_skydrive_file_init (ZpjSkydriveFile *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ZPJ_TYPE_SKYDRIVE_FILE, ZpjSkydriveFilePrivate);
}


static void
zpj_skydrive_file_class_init (ZpjSkydriveFileClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  ZpjSkydriveEntryClass *entry_class = ZPJ_SKYDRIVE_ENTRY_CLASS (class);

  object_class->get_property = zpj_skydrive_file_get_property;
  entry_class->parse_json_node = zpj_skydrive_file_parse_json_node;

  g_object_class_install_property (object_class,
                                   PROP_SIZE,
                                   g_param_spec_int64 ("size",
                                                       "Size",
                                                       "Size of the file in bytes.",
                                                       -1,
                                                       G_MAXINT64,
                                                       -1,
                                                       G_PARAM_READABLE));

  g_type_class_add_private (class, sizeof (ZpjSkydriveFilePrivate));
}


/**
 * zpj_skydrive_file_new:
 * @node: A #JsonNode returned by the server.
 *
 * Creates a new #ZpjSkydriveFile from the given @node. If you already
 * know the ID of the file then you can use
 * zpj_skydrive_query_info_from_id().
 *
 * Returns: (transfer full): A new #ZpjSkydriveFile. Free the returned
 * object with g_object_unref().
 */
ZpjSkydriveEntry *
zpj_skydrive_file_new (JsonNode *node)
{
  return g_object_new (ZPJ_TYPE_SKYDRIVE_FILE, "json", node, NULL);
}


/**
 * zpj_skydrive_file_get_size:
 * @self: A #ZpjSkydriveFile.
 *
 * Gets the size of @self in bytes. This is the value of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#file">
 * size</ulink> member in the JSON returned by the server.
 *
 * Returns: The entry's size in bytes.
 */
goffset
zpj_skydrive_file_get_size (ZpjSkydriveFile *self)
{
  return self->priv->size;
}

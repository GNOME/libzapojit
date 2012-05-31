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

#include <glib.h>

#include "zpj-enums.h"
#include "zpj-skydrive-entry.h"
#include "zpj-skydrive-file.h"
#include "zpj-skydrive-folder.h"
#include "zpj-skydrive-photo.h"


/**
 * SECTION:zpj-skydrive-entry
 * @title: ZpjSkydriveEntry
 * @short_description: Abstract base class for file, folder and photo
 *   objects.
 * @include: zpj/zpj.h
 *
 * #ZpjSkydriveEntry is an abstract base class for Skydrive file,
 * folder and photo
 * <ulink url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * objects</ulink>.
 *
 * Subclasses may implement
 * #ZpjSkydriveEntryClass.parse_json_node (should chain up to its
 * parent class) to parse data specific to them in the JSON returned
 * by the server.
 */


struct _ZpjSkydriveEntryPrivate
{
  GDateTime *created_time;
  GDateTime *updated_time;
  ZpjSkydriveEntryType type;
  gchar *description;
  gchar *from_id;
  gchar *from_name;
  gchar *id;
  gchar *name;
  gchar *parent_id;
};

enum
{
  PROP_0,
  PROP_CREATED_TIME,
  PROP_DESCRIPTION,
  PROP_FROM_ID,
  PROP_FROM_NAME,
  PROP_ID,
  PROP_JSON,
  PROP_NAME,
  PROP_PARENT_ID,
  PROP_TYPE,
  PROP_UPDATED_TIME
};


G_DEFINE_ABSTRACT_TYPE (ZpjSkydriveEntry, zpj_skydrive_entry, G_TYPE_OBJECT);


static void
zpj_skydrive_entry_default_parse_json_node (ZpjSkydriveEntry *self, JsonNode *node)
{
  ZpjSkydriveEntryPrivate *priv = self->priv;
  GTimeVal tv;
  JsonObject *from;
  JsonObject *object;
  const gchar *description;
  const gchar *from_id;
  const gchar *from_name;
  const gchar *id;
  const gchar *created_time;
  const gchar *updated_time;
  const gchar *name;
  const gchar *parent_id;
  const gchar *type;

  object = json_node_get_object (node);

  description = json_object_get_string_member (object, "description");
  priv->description = g_strdup (description);

  from = json_object_get_object_member (object, "from");
  from_id = json_object_get_string_member (from, "id");
  priv->from_id = g_strdup (from_id);
  from_name = json_object_get_string_member (from, "name");
  priv->from_name = g_strdup (from_name);

  id = json_object_get_string_member (object, "id");
  priv->id = g_strdup (id);

  created_time = json_object_get_string_member (object, "created_time");
  if (g_time_val_from_iso8601 (created_time, &tv))
    priv->created_time = g_date_time_new_from_timeval_local (&tv);

  updated_time = json_object_get_string_member (object, "updated_time");
  if (g_time_val_from_iso8601 (updated_time, &tv))
    priv->updated_time = g_date_time_new_from_timeval_local (&tv);

  name = json_object_get_string_member (object, "name");
  priv->name = g_strdup (name);

  parent_id = json_object_get_string_member (object, "parent_id");
  priv->parent_id = g_strdup (parent_id);

  type = json_object_get_string_member (object, "type");
  if (g_strcmp0 (type, "file") == 0)
    {
      g_assert_cmpuint (G_OBJECT_TYPE (self), ==, ZPJ_TYPE_SKYDRIVE_FILE);
      priv->type = ZPJ_SKYDRIVE_ENTRY_TYPE_FILE;
    }
  else if (g_strcmp0 (type, "album") == 0 || g_strcmp0 (type, "folder") == 0)
    {
      g_assert_cmpuint (G_OBJECT_TYPE (self), ==, ZPJ_TYPE_SKYDRIVE_FOLDER);
      priv->type = ZPJ_SKYDRIVE_ENTRY_TYPE_FOLDER;
    }
  else if (g_strcmp0 (type, "photo") == 0)
    {
      g_assert_cmpuint (G_OBJECT_TYPE (self), ==, ZPJ_TYPE_SKYDRIVE_PHOTO);
      priv->type = ZPJ_SKYDRIVE_ENTRY_TYPE_PHOTO;
    }
  else
    g_warning ("unknown type: %s", type);
}


static void
zpj_skydrive_entry_dispose (GObject *object)
{
  ZpjSkydriveEntry *self = ZPJ_SKYDRIVE_ENTRY (object);
  ZpjSkydriveEntryPrivate *priv = self->priv;

  if (priv->updated_time != NULL)
    {
      g_date_time_unref (priv->updated_time);
      priv->updated_time = NULL;
    }

  G_OBJECT_CLASS (zpj_skydrive_entry_parent_class)->dispose (object);
}


static void
zpj_skydrive_entry_finalize (GObject *object)
{
  ZpjSkydriveEntry *self = ZPJ_SKYDRIVE_ENTRY (object);
  ZpjSkydriveEntryPrivate *priv = self->priv;

  g_free (priv->description);
  g_free (priv->from_id);
  g_free (priv->from_name);
  g_free (priv->id);
  g_free (priv->name);
  g_free (priv->parent_id);

  G_OBJECT_CLASS (zpj_skydrive_entry_parent_class)->finalize (object);
}


static void
zpj_skydrive_entry_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  ZpjSkydriveEntry *self = ZPJ_SKYDRIVE_ENTRY (object);
  ZpjSkydriveEntryPrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_CREATED_TIME:
      g_value_set_boxed (value, priv->created_time);
      break;

    case PROP_DESCRIPTION:
      g_value_set_string (value, priv->description);
      break;

    case PROP_FROM_ID:
      g_value_set_string (value, priv->from_id);
      break;

    case PROP_FROM_NAME:
      g_value_set_string (value, priv->from_name);
      break;

    case PROP_ID:
      g_value_set_string (value, priv->id);
      break;

    case PROP_NAME:
      g_value_set_string (value, priv->name);
      break;

    case PROP_PARENT_ID:
      g_value_set_string (value, priv->parent_id);
      break;

    case PROP_TYPE:
      g_value_set_enum (value, priv->type);
      break;

    case PROP_UPDATED_TIME:
      g_value_set_boxed (value, priv->updated_time);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_skydrive_entry_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  ZpjSkydriveEntry *self = ZPJ_SKYDRIVE_ENTRY (object);
  ZpjSkydriveEntryPrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_JSON:
      {
        JsonNode *node;

        node = (JsonNode *) g_value_get_boxed (value);
        if (node == NULL)
          break;

        ZPJ_SKYDRIVE_ENTRY_GET_CLASS (self)->parse_json_node (self, node);
        break;
      }

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_skydrive_entry_init (ZpjSkydriveEntry *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ZPJ_TYPE_SKYDRIVE_ENTRY, ZpjSkydriveEntryPrivate);
}


static void
zpj_skydrive_entry_class_init (ZpjSkydriveEntryClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = zpj_skydrive_entry_dispose;
  object_class->finalize = zpj_skydrive_entry_finalize;
  object_class->get_property = zpj_skydrive_entry_get_property;
  object_class->set_property = zpj_skydrive_entry_set_property;
  class->parse_json_node = zpj_skydrive_entry_default_parse_json_node;

  g_object_class_install_property (object_class,
                                   PROP_CREATED_TIME,
                                   g_param_spec_boxed ("created-time",
                                                       "Created Time",
                                                       "The date and time when the entry was created.",
                                                       G_TYPE_DATE_TIME,
                                                       G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_DESCRIPTION,
                                   g_param_spec_string ("description",
                                                        "Description",
                                                        "A brief description of this entry.",
                                                        NULL,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_FROM_ID,
                                   g_param_spec_string ("from-id",
                                                        "From ID",
                                                        "The ID of the user created this entry.",
                                                        NULL,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_FROM_NAME,
                                   g_param_spec_string ("from-name",
                                                        "From Name",
                                                        "The name of the user created this entry.",
                                                        NULL,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_ID,
                                   g_param_spec_string ("id",
                                                        "ID",
                                                        "Unique identifier corresponding to this entry.",
                                                        NULL,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_JSON,
                                   g_param_spec_boxed ("json",
                                                       "JSON",
                                                       "The JSON node representing this entry.",
                                                       JSON_TYPE_NODE,
                                                       G_PARAM_CONSTRUCT_ONLY | G_PARAM_WRITABLE));

  g_object_class_install_property (object_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "Human readable name of this entry.",
                                                        NULL,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_PARENT_ID,
                                   g_param_spec_string ("parent-id",
                                                        "Parent ID",
                                                        "Unique identifier corresponding to the parent entry.",
                                                        NULL,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_TYPE,
                                   g_param_spec_enum ("type",
                                                      "Type",
                                                      "Indicates whether this entry is a file or a entry.",
                                                      ZPJ_TYPE_SKYDRIVE_ENTRY_TYPE,
                                                      ZPJ_SKYDRIVE_ENTRY_TYPE_INVALID,
                                                      G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_UPDATED_TIME,
                                   g_param_spec_boxed ("updated-time",
                                                       "Updated Time",
                                                       "The date and time when the entry was last updated.",
                                                       G_TYPE_DATE_TIME,
                                                       G_PARAM_READABLE));

  g_type_class_add_private (class, sizeof (ZpjSkydriveEntryPrivate));
}


/**
 * zpj_skydrive_entry_get_created_time:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the time at which @self was created. This is the value of the
 * <ulink url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * updated_time</ulink> member in the JSON returned by the server.
 *
 * Returns: (transfer none): a #GDateTime representing the time at
 * which the entry was created. The returned time is owned by the
 * #ZpjSkydriveEntry and should not be unreferenced.
 */
GDateTime *
zpj_skydrive_entry_get_created_time (ZpjSkydriveEntry *self)
{
  return self->priv->created_time;
}


/**
 * zpj_skydrive_entry_get_description:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the description of @self. This is the value of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * description</ulink> member in the JSON returned by the server.
 *
 * Returns: (transfer none): description of the entry. This string is
 * owned by the #ZpjSkydriveEntry and should not be modified or freed.
 */
const gchar *
zpj_skydrive_entry_get_description (ZpjSkydriveEntry *self)
{
  return self->priv->description;
}


/**
 * zpj_skydrive_entry_get_from_id:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the ID of the user who created or uploaded @self. This is part
 * of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * from</ulink> object in the JSON returned by the server.
 *
 * Returns: (transfer none): ID of the user who created the entry.
 * This string is by the #ZpjSkydriveEntry and should not be modified
 * or freed.
 */
const gchar *
zpj_skydrive_entry_get_from_id (ZpjSkydriveEntry *self)
{
  return self->priv->from_id;
}


/**
 * zpj_skydrive_entry_get_from_name:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the name of the user who created or uploaded @self. This is
 * part of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * from</ulink> object in the JSON returned by the server.
 *
 * Returns: (transfer none): name of the user who created the entry.
 * This string is by the #ZpjSkydriveEntry and should not be modified
 * or freed.
 */
const gchar *
zpj_skydrive_entry_get_from_name (ZpjSkydriveEntry *self)
{
  return self->priv->from_name;
}


/**
 * zpj_skydrive_entry_get_id:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the ID of @self. This is the value of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * id</ulink> member in the JSON returned by the server.
 *
 * Returns: (transfer none): the entry's ID. This string is
 * owned by the #ZpjSkydriveEntry and should not be modified or freed.
 */
const gchar *
zpj_skydrive_entry_get_id (ZpjSkydriveEntry *self)
{
  return self->priv->id;
}


/**
 * zpj_skydrive_entry_get_name:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the name of @self. This is the value of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * name</ulink> member in the JSON returned by the server.
 *
 * Returns: (transfer none): name of the entry. This string is
 * owned by the #ZpjSkydriveEntry and should not be modified or freed.
 */
const gchar *
zpj_skydrive_entry_get_name (ZpjSkydriveEntry *self)
{
  return self->priv->name;
}


/**
 * zpj_skydrive_entry_get_parent_id:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the ID of the folder containing @self. This is the value of
 * the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * parent_id</ulink> member in the JSON returned by the server.
 *
 * Returns: (transfer none): ID of the parent folder. This string is
 * owned by the #ZpjSkydriveEntry and should not be modified or freed.
 */
const gchar *
zpj_skydrive_entry_get_parent_id (ZpjSkydriveEntry *self)
{
  return self->priv->parent_id;
}


/**
 * zpj_skydrive_entry_get_updated_time:
 * @self: A #ZpjSkydriveEntry.
 *
 * Gets the time at which @self was last updated. This is the value of
 * the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * updated_time</ulink> member in the JSON returned by the server.
 *
 * Returns: (transfer none): a #GDateTime representing the time at
 * which the entry was last updated. The returned time is owned by the
 * #ZpjSkydriveEntry and should not be unreferenced.
 */
GDateTime *
zpj_skydrive_entry_get_updated_time (ZpjSkydriveEntry *self)
{
  return self->priv->updated_time;
}


/**
 * zpj_skydrive_entry_is_folder:
 * @self: A #ZpjSkydriveEntry.
 *
 * Whether @self is a folder.
 *
 * Returns: %TRUE if the entry is a folder.
 */
gboolean
zpj_skydrive_entry_is_folder (ZpjSkydriveEntry *self)
{
  return self->priv->type == ZPJ_SKYDRIVE_ENTRY_TYPE_FOLDER;
}

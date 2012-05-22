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

#include "zpj-enums.h"
#include "zpj-skydrive-entry.h"


struct _ZpjSkydriveEntryPrivate
{
  GDateTime *updated_time;
  ZpjSkydriveEntryType type;
  gchar *description;
  gchar *id;
  gchar *name;
  gchar *parent_id;
};

enum
{
  PROP_0,
  PROP_DESCRIPTION,
  PROP_ID,
  PROP_NAME,
  PROP_PARENT_ID,
  PROP_TYPE,
  PROP_UPDATED_TIME
};


G_DEFINE_ABSTRACT_TYPE (ZpjSkydriveEntry, zpj_skydrive_entry, G_TYPE_OBJECT);


static void
zpj_skydrive_entry_finalize (GObject *object)
{
  ZpjSkydriveEntry *self = ZPJ_SKYDRIVE_ENTRY (object);
  ZpjSkydriveEntryPrivate *priv = self->priv;

  g_free (priv->description);
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
    case PROP_DESCRIPTION:
      g_value_set_string (value, priv->description);
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
    case PROP_DESCRIPTION:
      priv->description = g_value_dup_string (value);
      break;

    case PROP_ID:
      priv->id = g_value_dup_string (value);
      break;

    case PROP_NAME:
      priv->name = g_value_dup_string (value);
      break;

    case PROP_PARENT_ID:
      priv->parent_id = g_value_dup_string (value);
      break;

    case PROP_TYPE:
      priv->type = g_value_get_enum (value);
      break;

    case PROP_UPDATED_TIME:
      priv->updated_time = g_value_dup_boxed (value);
      break;

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

  object_class->finalize = zpj_skydrive_entry_finalize;
  object_class->get_property = zpj_skydrive_entry_get_property;
  object_class->set_property = zpj_skydrive_entry_set_property;

  g_object_class_install_property (object_class,
                                   PROP_DESCRIPTION,
                                   g_param_spec_string ("description",
                                                        "Description",
                                                        "A brief description of this entry.",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_ID,
                                   g_param_spec_string ("id",
                                                        "ID",
                                                        "Unique identifier corresponding to this entry.",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_NAME,
                                   g_param_spec_string ("name",
                                                        "Name",
                                                        "Human readable name of this entry.",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_PARENT_ID,
                                   g_param_spec_string ("parent-id",
                                                        "Parent ID",
                                                        "Unique identifier corresponding to the parent entry.",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_TYPE,
                                   g_param_spec_enum ("type",
                                                      "Type",
                                                      "Indicates whether this entry is a file or a entry.",
                                                      ZPJ_TYPE_SKYDRIVE_ENTRY_TYPE,
                                                      ZPJ_SKYDRIVE_ENTRY_TYPE_INVALID,
                                                      G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_UPDATED_TIME,
                                   g_param_spec_boxed ("updated-time",
                                                       "Updated Time",
                                                       "The date and time when the entry was last updated.",
                                                       G_TYPE_DATE_TIME,
                                                       G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_type_class_add_private (class, sizeof (ZpjSkydriveEntryPrivate));
}


const gchar *
zpj_skydrive_entry_get_description (ZpjSkydriveEntry *self)
{
  return self->priv->description;
}


const gchar *
zpj_skydrive_entry_get_id (ZpjSkydriveEntry *self)
{
  return self->priv->id;
}


const gchar *
zpj_skydrive_entry_get_name (ZpjSkydriveEntry *self)
{
  return self->priv->name;
}


const gchar *
zpj_skydrive_entry_get_parent_id (ZpjSkydriveEntry *self)
{
  return self->priv->parent_id;
}


GDateTime *
zpj_skydrive_entry_get_updated_time (ZpjSkydriveEntry *self)
{
  return self->priv->updated_time;
}


gboolean
zpj_skydrive_entry_is_folder (ZpjSkydriveEntry *self)
{
  return self->priv->type == ZPJ_SKYDRIVE_ENTRY_TYPE_FOLDER;
}

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

#ifndef ZPJ_SKYDRIVE_ENTRY_H
#define ZPJ_SKYDRIVE_ENTRY_H

#include <glib-object.h>
#include <json-glib/json-glib.h>

G_BEGIN_DECLS

#define ZPJ_TYPE_SKYDRIVE_ENTRY (zpj_skydrive_entry_get_type ())

#define ZPJ_SKYDRIVE_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   ZPJ_TYPE_SKYDRIVE_ENTRY, ZpjSkydriveEntry))

#define ZPJ_SKYDRIVE_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   ZPJ_TYPE_SKYDRIVE_ENTRY, ZpjSkydriveEntryClass))

#define ZPJ_IS_SKYDRIVE_ENTRY(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   ZPJ_TYPE_SKYDRIVE_ENTRY))

#define ZPJ_IS_SKYDRIVE_ENTRY_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   ZPJ_TYPE_SKYDRIVE_ENTRY))

#define ZPJ_SKYDRIVE_ENTRY_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   ZPJ_TYPE_SKYDRIVE_ENTRY, ZpjSkydriveEntryClass))

/**
 * ZpjSkydriveEntryType:
 * @ZPJ_SKYDRIVE_ENTRY_TYPE_FILE: A file.
 * @ZPJ_SKYDRIVE_ENTRY_TYPE_FOLDER: A folder.
 * @ZPJ_SKYDRIVE_ENTRY_TYPE_PHOTO: A photo.
 * @ZPJ_SKYDRIVE_ENTRY_TYPE_INVALID: Invalid or unknown object.
 *
 * The types of Skydrive
 * <ulink url="http://msdn.microsoft.com/en-us/library/live/hh243648">
 * objects</ulink>.
 *
 * This enumeration can be expanded at a later date.
 */
typedef enum
{
  ZPJ_SKYDRIVE_ENTRY_TYPE_FILE,
  ZPJ_SKYDRIVE_ENTRY_TYPE_FOLDER,
  ZPJ_SKYDRIVE_ENTRY_TYPE_PHOTO,
  ZPJ_SKYDRIVE_ENTRY_TYPE_INVALID
} ZpjSkydriveEntryType;

typedef struct _ZpjSkydriveEntry        ZpjSkydriveEntry;
typedef struct _ZpjSkydriveEntryClass   ZpjSkydriveEntryClass;
typedef struct _ZpjSkydriveEntryPrivate ZpjSkydriveEntryPrivate;

/**
 * ZpjSkydriveEntry:
 *
 * The #ZpjSkydriveEntry structure contains only private data and
 * should only be accessed using the provided API.
 */
struct _ZpjSkydriveEntry
{
  GObject parent_instance;
  ZpjSkydriveEntryPrivate *priv;
};

/**
 * ZpjSkydriveEntryClass:
 * @parent_class: The parent class.
 * @parse_json_node: Virtual function that subclasses may implement
 *   to parse data specific to them in the JSON returned by the server.
 *   Implementations must chain up to their parent classes.
 *
 * Class structure for #ZpjSkydriveEntry.
 */
struct _ZpjSkydriveEntryClass
{
  GObjectClass parent_class;

  void    (*parse_json_node)    (ZpjSkydriveEntry *self, JsonNode *node);
};

GType               zpj_skydrive_entry_get_type           (void) G_GNUC_CONST;

GDateTime          *zpj_skydrive_entry_get_created_time   (ZpjSkydriveEntry *self);

const gchar        *zpj_skydrive_entry_get_description    (ZpjSkydriveEntry *self);

const gchar        *zpj_skydrive_entry_get_from_id        (ZpjSkydriveEntry *self);

const gchar        *zpj_skydrive_entry_get_from_name      (ZpjSkydriveEntry *self);

const gchar        *zpj_skydrive_entry_get_id             (ZpjSkydriveEntry *self);

const gchar        *zpj_skydrive_entry_get_name           (ZpjSkydriveEntry *self);

const gchar        *zpj_skydrive_entry_get_parent_id      (ZpjSkydriveEntry *self);

GDateTime          *zpj_skydrive_entry_get_updated_time   (ZpjSkydriveEntry *self);

gboolean            zpj_skydrive_entry_is_folder          (ZpjSkydriveEntry *self);

G_END_DECLS

#endif /* ZPJ_SKYDRIVE_ENTRY_H */

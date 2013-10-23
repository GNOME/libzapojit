/*
 * Zapojit - GLib/GObject wrapper for the SkyDrive and Hotmail REST APIs
 * Copyright © 2013 Red Hat, Inc.
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

#include "zpj-skydrive-video.h"


/**
 * SECTION:zpj-skydrive-video
 * @title: ZpjSkydriveVideo
 * @short_description: Skydrive video object.
 * @include: zpj/zpj.h
 *
 * #ZpjSkydriveVideo represents a <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#video">
 * Skydrive video</ulink> object.
 */


struct _ZpjSkydriveVideoPrivate
{
  guint bitrate;
  guint height;
  guint width;
  guint64 duration;
};

enum
{
  PROP_0,
  PROP_BITRATE,
  PROP_DURATION,
  PROP_HEIGHT,
  PROP_WIDTH
};


G_DEFINE_TYPE (ZpjSkydriveVideo, zpj_skydrive_video, ZPJ_TYPE_SKYDRIVE_FILE);


static void
zpj_skydrive_video_parse_json_node (ZpjSkydriveEntry *entry, JsonNode *node)
{
  ZpjSkydriveVideo *self = ZPJ_SKYDRIVE_VIDEO (entry);
  ZpjSkydriveVideoPrivate *priv = self->priv;
  JsonObject *object;
  gint64 bitrate;
  gint64 duration;
  gint64 height;
  gint64 width;

  ZPJ_SKYDRIVE_ENTRY_CLASS (zpj_skydrive_video_parent_class)->parse_json_node (entry, node);

  object = json_node_get_object (node);

  bitrate = json_object_get_int_member (object, "bitrate");
  priv->bitrate = (guint) bitrate;

  duration = json_object_get_int_member (object, "duration");
  priv->duration = (guint64) duration;

  height = json_object_get_int_member (object, "height");
  priv->height = (guint) height;

  width = json_object_get_int_member (object, "width");
  priv->width = (guint) width;
}


static void
zpj_skydrive_video_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  ZpjSkydriveVideo *self = ZPJ_SKYDRIVE_VIDEO (object);
  ZpjSkydriveVideoPrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_BITRATE:
      g_value_set_uint (value, priv->bitrate);
      break;

    case PROP_DURATION:
      g_value_set_uint64 (value, priv->duration);
      break;

    case PROP_HEIGHT:
      g_value_set_uint (value, priv->height);
      break;

    case PROP_WIDTH:
      g_value_set_uint (value, priv->width);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_skydrive_video_init (ZpjSkydriveVideo *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ZPJ_TYPE_SKYDRIVE_VIDEO, ZpjSkydriveVideoPrivate);
}


static void
zpj_skydrive_video_class_init (ZpjSkydriveVideoClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);
  ZpjSkydriveEntryClass *entry_class = ZPJ_SKYDRIVE_ENTRY_CLASS (class);

  object_class->get_property = zpj_skydrive_video_get_property;
  entry_class->parse_json_node = zpj_skydrive_video_parse_json_node;

  g_object_class_install_property (object_class,
                                   PROP_BITRATE,
                                   g_param_spec_uint ("bitrate",
                                                      "Bit rate",
                                                      "Bit rate of the video in bits per second.",
                                                      0,
                                                      G_MAXUINT,
                                                      0,
                                                      G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_DURATION,
                                   g_param_spec_uint64 ("duration",
                                                        "Duration",
                                                        "Run time duration of the video in milliseconds.",
                                                        0,
                                                        G_MAXUINT64,
                                                        0,
                                                        G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_HEIGHT,
                                   g_param_spec_uint ("height",
                                                      "Height",
                                                      "Height of the video in pixels.",
                                                      0,
                                                      G_MAXUINT,
                                                      0,
                                                      G_PARAM_READABLE));

  g_object_class_install_property (object_class,
                                   PROP_WIDTH,
                                   g_param_spec_uint ("width",
                                                      "Width",
                                                      "Width of the video in pixels.",
                                                      0,
                                                      G_MAXUINT,
                                                      0,
                                                      G_PARAM_READABLE));

  g_type_class_add_private (class, sizeof (ZpjSkydriveVideoPrivate));
}


/**
 * zpj_skydrive_video_new:
 * @node: A #JsonNode returned by the server.
 *
 * Creates a new #ZpjSkydriveVideo from the given @node. If you already
 * know the ID of the video then you can use
 * zpj_skydrive_query_info_from_id().
 *
 * Returns: (transfer full): A new #ZpjSkydriveVideo. Free the returned
 * object with g_object_unref().
 */
ZpjSkydriveEntry *
zpj_skydrive_video_new (JsonNode *node)
{
  return g_object_new (ZPJ_TYPE_SKYDRIVE_VIDEO, "json", node, NULL);
}


/**
 * zpj_skydrive_video_get_bitrate:
 * @self: A #ZpjSkydriveVideo.
 *
 * Gets the bit rate of @self in bits per second. This is the value of
 * the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#video">
 * bitrate</ulink> member in the JSON returned by the server.
 *
 * Returns: The video's bit rate in bits per second.
 */
guint
zpj_skydrive_video_get_bitrate (ZpjSkydriveVideo *self)
{
  return self->priv->bitrate;
}


/**
 * zpj_skydrive_video_get_duration:
 * @self: A #ZpjSkydriveVideo.
 *
 * Gets the run time duration of @self in milliseconds. This is the
 * value of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#video">
 * duration</ulink> member in the JSON returned by the server.
 *
 * Returns: The video's run time duration in milliseconds.
 */
guint64
zpj_skydrive_video_get_duration (ZpjSkydriveVideo *self)
{
  return self->priv->duration;
}


/**
 * zpj_skydrive_video_get_height:
 * @self: A #ZpjSkydriveVideo.
 *
 * Gets the height of @self in pixels. This is the value of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#video">
 * height</ulink> member in the JSON returned by the server.
 *
 * Returns: The video's height in pixels.
 */
guint
zpj_skydrive_video_get_height (ZpjSkydriveVideo *self)
{
  return self->priv->height;
}


/**
 * zpj_skydrive_video_get_width:
 * @self: A #ZpjSkydriveVideo.
 *
 * Gets the width of @self in pixels. This is the value of the <ulink
 * url="http://msdn.microsoft.com/en-us/library/live/hh243648#video">
 * width</ulink> member in the JSON returned by the server.
 *
 * Returns: The video's width in pixels.
 */
guint
zpj_skydrive_video_get_width (ZpjSkydriveVideo *self)
{
  return self->priv->width;
}

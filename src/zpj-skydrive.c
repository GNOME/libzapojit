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

#include <json-glib/json-glib.h>
#include <libsoup/soup.h>
#include <libsoup/soup-request.h>
#include <libsoup/soup-request-http.h>
#include <libsoup/soup-requester.h>
#include <rest/rest-proxy.h>
#include <rest/rest-proxy-call.h>

#include "zpj-skydrive.h"
#include "zpj-skydrive-file.h"
#include "zpj-skydrive-photo.h"


struct _ZpjSkydrivePrivate
{
  ZpjAuthorizer *authorizer;
};

enum
{
  PROP_0,
  PROP_AUTHORIZER
};


G_DEFINE_TYPE (ZpjSkydrive, zpj_skydrive, G_TYPE_OBJECT);


typedef struct _ZpjSkydriveAsyncData ZpjSkydriveAsyncData;

struct _ZpjSkydriveAsyncData
{
  GCancellable *cancellable;
  GError **error;
  GMainLoop *loop;
  GOutputStream *ostream;
};

static const gchar *live_endpoint = "https://apis.live.net/v5.0/";


static ZpjSkydriveEntry *
zpj_skydrive_create_entry_from_json_node (JsonNode *node)
{
  ZpjSkydriveEntry *entry = NULL;
  JsonObject *object;
  const gchar *type;

  object = json_node_get_object (node);

  type = json_object_get_string_member (object, "type");
  if (g_strcmp0 (type, "file") == 0)
    entry = zpj_skydrive_file_new (node);
  else if (g_strcmp0 (type, "album") == 0 || g_strcmp0 (type, "folder") == 0)
    entry = zpj_skydrive_folder_new (node);
  else if (g_strcmp0 (type, "photo") == 0)
    entry = zpj_skydrive_photo_new (node);
  else
    g_warning ("unknown type: %s", type);

  return entry;
}


static void
zpj_skydrive_download_file_complete (SoupSession *session, SoupMessage *message, gpointer user_data)
{
  ZpjSkydriveAsyncData *data = (ZpjSkydriveAsyncData *) user_data;
  g_main_loop_quit (data->loop);
}


static void
zpj_skydrive_download_file_got_chunk (SoupMessage *message, SoupBuffer *chunk, gpointer user_data)
{
  ZpjSkydriveAsyncData *data = (ZpjSkydriveAsyncData *) user_data;
  gsize bytes_written;

  g_output_stream_write_all (data->ostream,
                             chunk->data,
                             chunk->length,
                             &bytes_written,
                             data->cancellable,
                             data->error);
}


static void
zpj_skydrive_list_json_array_foreach_folder (JsonArray *array,
                                             guint index,
                                             JsonNode *element_node,
                                             gpointer user_data)
{
  GList **list = (GList **) user_data;
  ZpjSkydriveEntry *entry;

  entry = zpj_skydrive_create_entry_from_json_node (element_node);
  *list = g_list_prepend (*list, entry);
}


static void
zpj_skydrive_dispose (GObject *object)
{
  ZpjSkydrive *self = ZPJ_SKYDRIVE (object);
  ZpjSkydrivePrivate *priv = self->priv;

  g_clear_object (&priv->authorizer);

  G_OBJECT_CLASS (zpj_skydrive_parent_class)->dispose (object);
}


static void
zpj_skydrive_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  ZpjSkydrive *self = ZPJ_SKYDRIVE (object);
  ZpjSkydrivePrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_AUTHORIZER:
      g_value_set_object (value, priv->authorizer);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_skydrive_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  ZpjSkydrive *self = ZPJ_SKYDRIVE (object);

  switch (prop_id)
    {
    case PROP_AUTHORIZER:
      zpj_skydrive_set_authorizer (self, g_value_get_object (value));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_skydrive_init (ZpjSkydrive *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ZPJ_TYPE_SKYDRIVE, ZpjSkydrivePrivate);
}


static void
zpj_skydrive_class_init (ZpjSkydriveClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = zpj_skydrive_dispose;
  object_class->get_property = zpj_skydrive_get_property;
  object_class->set_property = zpj_skydrive_set_property;

  g_object_class_install_property (object_class,
                                   PROP_AUTHORIZER,
                                   g_param_spec_object ("authorizer",
                                                        "Authorizer",
                                                        "An authorizer object to provide an access token for each "
                                                        "request",
                                                        ZPJ_TYPE_AUTHORIZER,
                                                        G_PARAM_CONSTRUCT | G_PARAM_READWRITE));

  g_type_class_add_private (class, sizeof (ZpjSkydrivePrivate));
}


ZpjSkydrive *
zpj_skydrive_new (ZpjAuthorizer *authorizer)
{
  return g_object_new (ZPJ_TYPE_SKYDRIVE, "authorizer", authorizer, NULL);
}


gboolean
zpj_skydrive_create_folder (ZpjSkydrive *self,
                            ZpjSkydriveFolder *folder,
                            GCancellable *cancellable,
                            GError **error)
{
  const gchar *name;
  const gchar *parent_id;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), FALSE);
  g_return_val_if_fail (ZPJ_IS_SKYDRIVE_FOLDER (folder), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  parent_id = zpj_skydrive_entry_get_parent_id (ZPJ_SKYDRIVE_ENTRY (folder));
  g_return_val_if_fail (parent_id != NULL && parent_id[0] != '\0', FALSE);

  name = zpj_skydrive_entry_get_name (ZPJ_SKYDRIVE_ENTRY (folder));
  return zpj_skydrive_create_folder_from_name (self, name, parent_id, cancellable, error);
}


gboolean
zpj_skydrive_create_folder_from_name (ZpjSkydrive *self,
                                      const gchar *name,
                                      const gchar *parent_id,
                                      GCancellable *cancellable,
                                      GError **error)
{
  ZpjSkydrivePrivate *priv = self->priv;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), FALSE);
  g_return_val_if_fail (parent_id != NULL && parent_id[0] != '\0', FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (!zpj_authorizer_refresh_authorization (priv->authorizer, cancellable, error))
    goto out;

 out:
  return FALSE;
}


gboolean
zpj_skydrive_delete_entry_id (ZpjSkydrive *self, const gchar *entry_id, GCancellable *cancellable, GError **error)
{
  ZpjSkydrivePrivate *priv = self->priv;
  SoupMessage *message = NULL;
  SoupSession *session = NULL;
  gboolean ret_val = FALSE;
  gchar *url = NULL;
  guint status;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), FALSE);
  g_return_val_if_fail (entry_id != NULL && entry_id[0] != '\0', FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (!zpj_authorizer_refresh_authorization (priv->authorizer, cancellable, error))
    goto out;

  session = soup_session_sync_new ();

  url = g_strconcat (live_endpoint, entry_id, NULL);
  message = soup_message_new ("DELETE", url);
  zpj_authorizer_process_message (priv->authorizer, NULL, message);

  status = soup_session_send_message (session, message);
  if (status != 204)
    {
      /* TODO: set error */
      goto out;
    }

  ret_val = TRUE;

 out:
  g_free (url);
  g_clear_object (&message);
  g_clear_object (&session);
  return ret_val;
}


GInputStream *
zpj_skydrive_download_file_id_to_stream (ZpjSkydrive *self,
                                         const gchar *file_id,
                                         GCancellable *cancellable,
                                         GError **error)
{
  ZpjSkydrivePrivate *priv = self->priv;
  GInputStream *ret_val = NULL;
  SoupMessage *message = NULL;
  SoupRequest *request = NULL;
  SoupRequester *requester = NULL;
  SoupSession *session= NULL;
  gchar *url = NULL;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), NULL);
  g_return_val_if_fail (file_id != NULL && file_id[0] != '\0', NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (!zpj_authorizer_refresh_authorization (priv->authorizer, cancellable, error))
    goto out;

  session = soup_session_sync_new ();
  requester = soup_requester_new ();
  soup_session_add_feature (session, SOUP_SESSION_FEATURE (requester));

  url = g_strconcat (live_endpoint, file_id, "/content", NULL);
  request = soup_requester_request (requester, url, error);
  if (request == NULL)
    goto out;

  message = soup_request_http_get_message (SOUP_REQUEST_HTTP (request));
  zpj_authorizer_process_message (priv->authorizer, NULL, message);

  ret_val = soup_request_send (request, cancellable, error);
  if (ret_val == NULL)
    goto out;

  /* The session is needed to use the input stream */
  g_object_weak_ref (G_OBJECT (ret_val), (GWeakNotify) g_object_unref, session);

 out:
  g_clear_object (&message);
  g_clear_object (&request);
  g_free (url);
  g_clear_object (&requester);

  return ret_val;
}


GInputStream *
zpj_skydrive_download_file_to_stream (ZpjSkydrive *self,
                                      ZpjSkydriveFile *file,
                                      GCancellable *cancellable,
                                      GError **error)
{
  const gchar *file_id;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), NULL);
  g_return_val_if_fail (ZPJ_IS_SKYDRIVE_FILE (file), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  file_id = zpj_skydrive_entry_get_id (ZPJ_SKYDRIVE_ENTRY (file));
  g_return_val_if_fail (file_id != NULL && file_id[0] != '\0', NULL);

  return zpj_skydrive_download_file_id_to_stream (self, file_id, cancellable, error);
}


gboolean
zpj_skydrive_download_file_id_to_path (ZpjSkydrive *self,
                                       const gchar *file_id,
                                       const gchar *path,
                                       GCancellable *cancellable,
                                       GError **error)
{
  ZpjSkydrivePrivate *priv = self->priv;
  ZpjSkydriveAsyncData data;
  GFile *file_dest = NULL;
  GFile *file_tmp = NULL;
  GFileIOStream *iostream = NULL;
  GMainContext *context = NULL;
  SoupMessage *message;
  SoupSession *session = NULL;
  gboolean ret_val = FALSE;
  gchar *url = NULL;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), FALSE);
  g_return_val_if_fail (file_id != NULL && file_id[0] != '\0', FALSE);
  g_return_val_if_fail (path != NULL && path[0] != '\0', FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  data.loop = NULL;

  if (!zpj_authorizer_refresh_authorization (priv->authorizer, cancellable, error))
    goto out;

  file_tmp = g_file_new_tmp (NULL, &iostream, error);
  if (file_tmp == NULL)
    goto out;

  data.cancellable = cancellable;
  data.error = error;
  data.ostream = g_io_stream_get_output_stream (G_IO_STREAM (iostream));

  context = g_main_context_new ();
  g_main_context_push_thread_default (context);
  data.loop = g_main_loop_new (context, FALSE);

  session = soup_session_async_new_with_options (SOUP_SESSION_USE_THREAD_CONTEXT, TRUE, NULL);

  url = g_strconcat (live_endpoint, file_id, "/content", NULL);
  message = soup_message_new ("GET", url);
  zpj_authorizer_process_message (priv->authorizer, NULL, message);

  soup_message_body_set_accumulate (message->response_body, FALSE);
  g_signal_connect (message, "got-chunk", G_CALLBACK (zpj_skydrive_download_file_got_chunk), &data);

  soup_session_queue_message (session, message, zpj_skydrive_download_file_complete, &data);
  g_main_loop_run (data.loop);

  g_main_context_pop_thread_default (context);

  if (!g_io_stream_close (G_IO_STREAM (iostream), cancellable, error))
    goto out;

  file_dest = g_file_new_for_path (path);
  if (!g_file_move (file_tmp, file_dest, G_FILE_COPY_BACKUP | G_FILE_COPY_OVERWRITE, cancellable, NULL, NULL, error))
    goto out;

  ret_val = TRUE;

 out:
  /* Deletion of the temporary file is not cancellable */
  g_file_delete (file_tmp, NULL, NULL);

  g_clear_object (&file_dest);
  g_free (url);
  g_clear_object (&session);
  if (data.loop != NULL)
    g_main_loop_unref (data.loop);
  if (context != NULL)
    g_main_context_unref (context);
  g_clear_object (&iostream);
  g_clear_object (&file_tmp);

  return ret_val;
}


gboolean
zpj_skydrive_download_file_to_path (ZpjSkydrive *self,
                                    ZpjSkydriveFile *file,
                                    const gchar *path,
                                    GCancellable *cancellable,
                                    GError **error)
{
  const gchar *file_id;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), FALSE);
  g_return_val_if_fail (ZPJ_IS_SKYDRIVE_FILE (file), FALSE);
  g_return_val_if_fail (path != NULL && path[0] != '\0', FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  file_id = zpj_skydrive_entry_get_id (ZPJ_SKYDRIVE_ENTRY (file));
  g_return_val_if_fail (file_id != NULL && file_id[0] != '\0', FALSE);

  return zpj_skydrive_download_file_id_to_path (self, file_id, path, cancellable, error);
}


GList *
zpj_skydrive_list_folder (ZpjSkydrive *self, ZpjSkydriveFolder *folder, GCancellable *cancellable, GError **error)
{
  const gchar *folder_id;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), NULL);
  g_return_val_if_fail (ZPJ_IS_SKYDRIVE_FOLDER (folder), NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  folder_id = zpj_skydrive_entry_get_id (ZPJ_SKYDRIVE_ENTRY (folder));
  g_return_val_if_fail (folder_id != NULL && folder_id[0] != '\0', NULL);

  return zpj_skydrive_list_folder_id (self, folder_id, cancellable, error);
}


GList *
zpj_skydrive_list_folder_id (ZpjSkydrive *self, const gchar *folder_id, GCancellable *cancellable, GError **error)
{
  ZpjSkydrivePrivate *priv = self->priv;
  GList *list = NULL;
  JsonArray *array;
  JsonNode *root;
  JsonObject *object;
  JsonParser *parser = NULL;
  RestProxy *proxy = NULL;
  RestProxyCall *call = NULL;
  const gchar *payload;
  gchar *url = NULL;
  goffset length;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), NULL);
  g_return_val_if_fail (folder_id != NULL && folder_id[0] != '\0', NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (!zpj_authorizer_refresh_authorization (priv->authorizer, cancellable, error))
    goto out;

  url = g_strconcat (live_endpoint, folder_id, "/files", NULL);
  proxy = rest_proxy_new (url, FALSE);

  call = rest_proxy_new_call (proxy);
  rest_proxy_call_set_method (call, "GET");

  zpj_authorizer_process_call (priv->authorizer, NULL, call);

  if (!rest_proxy_call_sync (call, error))
    goto out;

  payload = rest_proxy_call_get_payload (call);
  length = rest_proxy_call_get_payload_length (call);
  parser = json_parser_new ();
  if (!json_parser_load_from_data (parser, payload, length, error))
    goto out;

  root = json_parser_get_root (parser);
  object = json_node_get_object (root);
  array = json_object_get_array_member (object, "data");

  json_array_foreach_element (array, zpj_skydrive_list_json_array_foreach_folder, &list);
  list = g_list_reverse (list);

 out:
  g_clear_object (&parser);
  g_clear_object (&call);
  g_clear_object (&proxy);
  g_free (url);
  return list;
}


ZpjAuthorizer *
zpj_skydrive_get_authorizer (ZpjSkydrive *self)
{
  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), NULL);
  return self->priv->authorizer;
}


ZpjSkydriveEntry *
zpj_skydrive_query_info_from_id (ZpjSkydrive *self, const gchar *id, GCancellable *cancellable, GError **error)
{
  ZpjSkydrivePrivate *priv = self->priv;
  JsonNode *root;
  JsonObject *object;
  JsonParser *parser = NULL;
  RestProxy *proxy = NULL;
  RestProxyCall *call = NULL;
  ZpjSkydriveEntry *entry = NULL;
  const gchar *payload;
  gchar *url = NULL;
  goffset length;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), NULL);
  g_return_val_if_fail (id != NULL && id[0] != '\0', NULL);
  g_return_val_if_fail (error == NULL || *error == NULL, NULL);

  if (!zpj_authorizer_refresh_authorization (priv->authorizer, cancellable, error))
    goto out;

  url = g_strconcat (live_endpoint, id, NULL);
  proxy = rest_proxy_new (url, FALSE);

  call = rest_proxy_new_call (proxy);
  rest_proxy_call_set_method (call, "GET");

  zpj_authorizer_process_call (priv->authorizer, NULL, call);

  if (!rest_proxy_call_sync (call, error))
    goto out;

  payload = rest_proxy_call_get_payload (call);
  length = rest_proxy_call_get_payload_length (call);
  parser = json_parser_new ();
  if (!json_parser_load_from_data (parser, payload, length, error))
    goto out;

  root = json_parser_get_root (parser);
  entry = zpj_skydrive_create_entry_from_json_node (root);

 out:
  g_clear_object (&parser);
  g_clear_object (&call);
  g_clear_object (&proxy);
  g_free (url);

  return entry;
}


void
zpj_skydrive_set_authorizer (ZpjSkydrive *self, ZpjAuthorizer *authorizer)
{
  ZpjSkydrivePrivate *priv = self->priv;

  g_return_if_fail (ZPJ_IS_SKYDRIVE (self));
  g_return_if_fail (authorizer == NULL || ZPJ_IS_AUTHORIZER (authorizer));

  g_clear_object (&priv->authorizer);

  if (authorizer != NULL)
    {
      g_object_ref (authorizer);
      priv->authorizer = authorizer;
    }

  g_object_notify (G_OBJECT (self), "authorizer");
}


gboolean
zpj_skydrive_upload_path_to_folder (ZpjSkydrive *self,
                                    const gchar *path,
                                    ZpjSkydriveFolder *folder,
                                    GCancellable *cancellable,
                                    GError **error)
{
  const gchar *folder_id;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), FALSE);
  g_return_val_if_fail (ZPJ_IS_SKYDRIVE_FOLDER (folder), FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  folder_id = zpj_skydrive_entry_get_id (ZPJ_SKYDRIVE_ENTRY (folder));
  g_return_val_if_fail (folder_id != NULL && folder_id[0] != '\0', FALSE);

  return zpj_skydrive_upload_path_to_folder_id (self, path, folder_id, cancellable, error);
}


gboolean
zpj_skydrive_upload_path_to_folder_id (ZpjSkydrive *self,
                                       const gchar *path,
                                       const gchar *folder_id,
                                       GCancellable *cancellable,
                                       GError **error)
{
  ZpjSkydrivePrivate *priv = self->priv;
  GMappedFile *file = NULL;
  SoupBuffer *buffer = NULL;
  SoupMessage *message = NULL;
  SoupMultipart *multipart = NULL;
  SoupSession *session = NULL;
  gboolean ret_val = FALSE;
  gchar *basename = NULL;
  gchar *contents;
  gchar *url = NULL;
  gsize length;
  guint status;

  g_return_val_if_fail (ZPJ_IS_SKYDRIVE (self), FALSE);
  g_return_val_if_fail (folder_id != NULL && folder_id[0] != '\0', FALSE);
  g_return_val_if_fail (error == NULL || *error == NULL, FALSE);

  if (!zpj_authorizer_refresh_authorization (priv->authorizer, cancellable, error))
    goto out;

  file = g_mapped_file_new (path, FALSE, error);
  if (file == NULL)
    goto out;

  session = soup_session_sync_new ();

  url = g_strconcat (live_endpoint, folder_id, "/files", NULL);
  message = soup_message_new ("POST", url);
  zpj_authorizer_process_message (priv->authorizer, NULL, message);

  basename = g_path_get_basename (path);

  contents = g_mapped_file_get_contents (file);
  length = g_mapped_file_get_length (file);
  buffer = soup_buffer_new (SOUP_MEMORY_STATIC, contents, length);

  multipart = soup_multipart_new ("multipart/form-data");
  soup_multipart_append_form_file (multipart, "file", basename, "application/octet-stream", buffer);
  soup_multipart_to_message (multipart, message->request_headers, message->request_body);

  status = soup_session_send_message (session, message);
  if (status != 201)
    {
      /* TODO: set error */
      goto out;
    }

  ret_val = TRUE;

 out:
  if (multipart != NULL)
    soup_multipart_free (multipart);
  if (buffer != NULL)
    soup_buffer_free (buffer);
  g_free (basename);
  g_clear_object (&message);
  g_free (url);
  g_clear_object (&session);
  if (file != NULL)
    g_mapped_file_unref (file);

  return ret_val;
}

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
#include <libsoup/soup.h>
#include <rest/rest-proxy-call.h>

#include "zpj-authorizer.h"
#include "zpj-goa-authorizer.h"


/**
 * SECTION:zpj-goa-authorizer
 * @title: ZpjGoaAuthorizer
 * @short_description: A GNOME Online Accounts authorizer object for
 *   Windows Live Connect.
 * @include: zpj/zpj.h
 *
 * #ZpjGoaAuthorizer provides an implementation of the #ZpjAuthorizer
 * interface using GNOME Online Accounts.
 */


struct _ZpjGoaAuthorizerPrivate
{
  GMutex mutex;
  GoaObject *goa_object;
  gchar *access_token;
};

enum
{
  PROP_0,
  PROP_GOA_OBJECT
};

static void zpj_authorizer_interface_init (ZpjAuthorizerInterface *iface);


G_DEFINE_TYPE_WITH_CODE (ZpjGoaAuthorizer, zpj_goa_authorizer, G_TYPE_OBJECT,
                         G_IMPLEMENT_INTERFACE (ZPJ_TYPE_AUTHORIZER,
                                                zpj_authorizer_interface_init));


static void
zpj_goa_authorizer_process_call (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain, RestProxyCall *call)
{
  ZpjGoaAuthorizer *self = ZPJ_GOA_AUTHORIZER (iface);
  ZpjGoaAuthorizerPrivate *priv = self->priv;

  g_mutex_lock (&priv->mutex);

  if (priv->access_token != NULL)
    rest_proxy_call_add_param (call, "access_token", priv->access_token);

  g_mutex_unlock (&priv->mutex);
}


static void
zpj_goa_authorizer_process_message (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain, SoupMessage *message)
{
  ZpjGoaAuthorizer *self = ZPJ_GOA_AUTHORIZER (iface);
  ZpjGoaAuthorizerPrivate *priv = self->priv;
  gchar *auth_value = NULL;

  g_mutex_lock (&priv->mutex);

  if (priv->access_token == NULL)
    goto out;

  if (g_strcmp0 (message->method, "DELETE") == 0 || g_strcmp0 (message->method, "GET") == 0)
    {
      SoupURI *uri;

      uri = soup_message_get_uri (message);
      auth_value = g_strconcat ("access_token=", priv->access_token, NULL);
      soup_uri_set_query (uri, auth_value);
    }
  else if (g_strcmp0 (message->method, "POST") == 0)
    {
      auth_value = g_strconcat ("Bearer ", priv->access_token, NULL);
      soup_message_headers_append (message->request_headers, "Authorization", auth_value);
    }
  else
    g_assert_not_reached ();

 out:
  g_free (auth_value);
  g_mutex_unlock (&priv->mutex);
}


static gboolean
zpj_goa_authorizer_refresh_authorization (ZpjAuthorizer *iface, GCancellable *cancellable, GError **error)
{
  ZpjGoaAuthorizer *self = ZPJ_GOA_AUTHORIZER (iface);
  ZpjGoaAuthorizerPrivate *priv = self->priv;
  GoaAccount *account;
  GoaOAuth2Based *oauth2_based;
  gboolean ret_val = FALSE;

  g_mutex_lock (&priv->mutex);

  g_free (priv->access_token);
  priv->access_token = NULL;

  account = goa_object_peek_account (priv->goa_object);
  oauth2_based = goa_object_peek_oauth2_based (priv->goa_object);

  if (!goa_account_call_ensure_credentials_sync (account, NULL, cancellable, error))
    goto out;

  if (!goa_oauth2_based_call_get_access_token_sync (oauth2_based, &priv->access_token, NULL, cancellable, error))
    goto out;

  ret_val = TRUE;

 out:
  g_mutex_unlock (&priv->mutex);
  return ret_val;
}


static void
zpj_goa_authorizer_set_goa_object (ZpjGoaAuthorizer *self, GoaObject *goa_object)
{
  GoaAccount *account;
  GoaOAuth2Based *oauth2_based;

  g_return_if_fail (GOA_IS_OBJECT (goa_object));

  oauth2_based = goa_object_peek_oauth2_based (goa_object);
  g_return_if_fail (oauth2_based != NULL && GOA_IS_OAUTH2_BASED (oauth2_based));

  account = goa_object_peek_account (goa_object);
  g_return_if_fail (account != NULL && GOA_IS_ACCOUNT (account));

  g_object_ref (goa_object);
  self->priv->goa_object = goa_object;
  g_object_notify (G_OBJECT (self), "goa-object");
}


static void
zpj_goa_authorizer_dispose (GObject *object)
{
  ZpjGoaAuthorizer *self = ZPJ_GOA_AUTHORIZER (object);

  g_clear_object (&self->priv->goa_object);

  G_OBJECT_CLASS (zpj_goa_authorizer_parent_class)->dispose (object);
}


static void
zpj_goa_authorizer_finalize (GObject *object)
{
  ZpjGoaAuthorizer *self = ZPJ_GOA_AUTHORIZER (object);
  ZpjGoaAuthorizerPrivate *priv = self->priv;

  g_mutex_clear (&priv->mutex);
  g_free (priv->access_token);

  G_OBJECT_CLASS (zpj_goa_authorizer_parent_class)->finalize (object);
}


static void
zpj_goa_authorizer_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  ZpjGoaAuthorizer *self = ZPJ_GOA_AUTHORIZER (object);

  switch (prop_id)
    {
    case PROP_GOA_OBJECT:
      g_value_set_object (value, self->priv->goa_object);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_goa_authorizer_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  ZpjGoaAuthorizer *self = ZPJ_GOA_AUTHORIZER (object);

  switch (prop_id)
    {
    case PROP_GOA_OBJECT:
      zpj_goa_authorizer_set_goa_object (self, GOA_OBJECT (g_value_get_object (value)));
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_goa_authorizer_init (ZpjGoaAuthorizer *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ZPJ_TYPE_GOA_AUTHORIZER, ZpjGoaAuthorizerPrivate);
  g_mutex_init (&self->priv->mutex);
}


static void
zpj_goa_authorizer_class_init (ZpjGoaAuthorizerClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->dispose = zpj_goa_authorizer_dispose;
  object_class->finalize = zpj_goa_authorizer_finalize;
  object_class->get_property = zpj_goa_authorizer_get_property;
  object_class->set_property = zpj_goa_authorizer_set_property;

  g_object_class_install_property (object_class,
                                   PROP_GOA_OBJECT,
                                   g_param_spec_object ("goa-object",
                                                        "GoaObject",
                                                        "The GOA account to authenticate.",
                                                        GOA_TYPE_OBJECT,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_type_class_add_private (class, sizeof (ZpjGoaAuthorizerPrivate));
}


static void
zpj_authorizer_interface_init (ZpjAuthorizerInterface *iface)
{
  iface->process_call = zpj_goa_authorizer_process_call;
  iface->process_message = zpj_goa_authorizer_process_message;
  iface->refresh_authorization = zpj_goa_authorizer_refresh_authorization;
}


/**
 * zpj_goa_authorizer_new:
 * @goa_object: A #GoaObject representing a Windows Live account.
 *
 * Creates a new #ZpjGoaAuthorizer using @goa_object.
 *
 * Returns: (transfer full): A new #ZpjGoaAuthorizer. Free the returned
 * object with g_object_unref().
 */
ZpjGoaAuthorizer *
zpj_goa_authorizer_new (GoaObject *goa_object)
{
  return g_object_new (ZPJ_TYPE_GOA_AUTHORIZER, "goa-object", goa_object, NULL);
}


/**
 * zpj_goa_authorizer_get_goa_object:
 * @self: A #ZpjGoaAuthorizer.
 *
 * Gets the GOA account used by @self for authorization.
 *
 * Returns: (transfer none): A #GoaObject. The returned object is
 * owned by #ZpjGoaAuthorizer and should not be modified or freed.
 */
GoaObject *
zpj_goa_authorizer_get_goa_object (ZpjGoaAuthorizer *self)
{
  return self->priv->goa_object;
}

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

#include <glib.h>

#include "zpj-authorizer.h"


G_DEFINE_INTERFACE (ZpjAuthorizer, zpj_authorizer, G_TYPE_OBJECT);


static void
zpj_authorizer_refresh_authorization_thread_func (GSimpleAsyncResult *simple,
                                                  GObject *object,
                                                  GCancellable *cancellable)
{
  GError *error = NULL;

  zpj_authorizer_refresh_authorization (ZPJ_AUTHORIZER (object), cancellable, &error);
  if (error != NULL)
    {
      g_simple_async_result_set_from_error (simple, error);
      g_error_free (error);
    }
}


static void
zpj_authorizer_default_init (ZpjAuthorizerInterface *iface)
{
}


gboolean
zpj_authorizer_is_authorized_for_domain (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain)
{
  g_return_val_if_fail (ZPJ_IS_AUTHORIZER (iface), FALSE);
  return ZPJ_AUTHORIZER_GET_INTERFACE (iface)->is_authorized_for_domain (iface, domain);
}


void
zpj_authorizer_process_call (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain, RestProxyCall *call)
{
  g_return_if_fail (ZPJ_IS_AUTHORIZER (iface));
  ZPJ_AUTHORIZER_GET_INTERFACE (iface)->process_call (iface, domain, call);
}


void
zpj_authorizer_process_message (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain, SoupMessage *message)
{
  g_return_if_fail (ZPJ_IS_AUTHORIZER (iface));
  ZPJ_AUTHORIZER_GET_INTERFACE (iface)->process_message (iface, domain, message);
}


gboolean
zpj_authorizer_refresh_authorization (ZpjAuthorizer *iface, GCancellable *cancellable, GError **error)
{
  g_return_val_if_fail (ZPJ_IS_AUTHORIZER (iface), FALSE);
  return ZPJ_AUTHORIZER_GET_INTERFACE (iface)->refresh_authorization (iface, cancellable, error);
}


void
zpj_authorizer_refresh_authorization_async (ZpjAuthorizer *iface,
                                            GCancellable *cancellable,
                                            GAsyncReadyCallback callback,
                                            gpointer user_data)
{
  GSimpleAsyncResult *simple;

  g_return_if_fail (ZPJ_IS_AUTHORIZER (iface));

  simple = g_simple_async_result_new (G_OBJECT (iface),
                                      callback,
                                      user_data,
                                      zpj_authorizer_refresh_authorization_async);
  g_simple_async_result_run_in_thread (simple,
                                       zpj_authorizer_refresh_authorization_thread_func,
                                       G_PRIORITY_DEFAULT,
                                       cancellable);
  g_object_unref (simple);
}


gboolean
zpj_authorizer_refresh_authorization_finish (ZpjAuthorizer *iface, GAsyncResult *res, GError **error)
{
  g_return_val_if_fail (g_simple_async_result_is_valid (res,
                                                        G_OBJECT (iface),
                                                        zpj_authorizer_refresh_authorization_async),
                        FALSE);

  if (g_simple_async_result_propagate_error (G_SIMPLE_ASYNC_RESULT (res), error))
    return FALSE;

  return TRUE;
}

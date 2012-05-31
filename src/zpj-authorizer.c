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

#include "zpj-authorizer.h"


/**
 * SECTION:zpj-authorizer
 * @title: ZpjAuthorizer
 * @short_description: Windows Live Connect authorization interface.
 * @include: zpj/zpj.h
 *
 * The #ZpjAuthorizer interface provides a uniform way to implement
 * authentication and authorization processes for use by #ZpjSkydrive.
 * Client code will construct a new #ZpjAuthorizer instance of their
 * choosing, such as #ZpjGoaAuthorizer, and create a #ZpjSkydrive with
 * with it.
 *
 * #ZpjGoaAuthorizer is an implementation of #ZpjAuthorizer for using
 * authorization tokens provided by GNOME Online Accounts. It is quite
 * possible for clients to write their own implementations. For
 * example, on platforms that do not use GNOME Online Accounts a
 * client might want to implement the
 * <ulink url="http://msdn.microsoft.com/en-us/library/live/hh243647">
 * OAuth 2.0</ulink> authorization flow itself.
 *
 * It must be noted that all #ZpjAuthorizer implementations must be
 * thread safe, as methods such as
 * zpj_authorizer_refresh_authorization() may be called from any
 * thread (such as the thread performing an asynchronous #ZpjSkydrive
 * operation) at any time.
 */


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


/**
 * zpj_authorizer_is_authorized_for_domain:
 * @iface: A #ZpjAuthorizer.
 * @domain: A #ZpjAuthorizationDomain.
 *
 * Whether the authorization tokens held by @iface are valid for
 * @domain.
 *
 * This method is thread safe.
 *
 * Returns: %TRUE if the tokens are valid.
 */
gboolean
zpj_authorizer_is_authorized_for_domain (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain)
{
  g_return_val_if_fail (ZPJ_IS_AUTHORIZER (iface), FALSE);
  return ZPJ_AUTHORIZER_GET_INTERFACE (iface)->is_authorized_for_domain (iface, domain);
}


/**
 * zpj_authorizer_process_call:
 * @iface: A #ZpjAuthorizer.
 * @domain: (allow-none): An optional #ZpjAuthorizationDomain object,
 *   or %NULL.
 * @call: A #RestProxyCall.
 *
 * Adds the necessary authorization to @call.
 *
 * This method modifies @call in place and is thread safe.
 */
void
zpj_authorizer_process_call (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain, RestProxyCall *call)
{
  g_return_if_fail (ZPJ_IS_AUTHORIZER (iface));
  ZPJ_AUTHORIZER_GET_INTERFACE (iface)->process_call (iface, domain, call);
}


/**
 * zpj_authorizer_process_message:
 * @iface: A #ZpjAuthorizer.
 * @domain: (allow-none): An optional #ZpjAuthorizationDomain object,
 *   or %NULL.
 * @message: A #SoupMessage.
 *
 * Adds the necessary authorization to @message. The type of @message
 * can be DELETE, GET and POST.
 *
 * This method modifies @message in place and is thread safe.
 */
void
zpj_authorizer_process_message (ZpjAuthorizer *iface, ZpjAuthorizationDomain *domain, SoupMessage *message)
{
  g_return_if_fail (ZPJ_IS_AUTHORIZER (iface));
  ZPJ_AUTHORIZER_GET_INTERFACE (iface)->process_message (iface, domain, message);
}


/**
 * zpj_authorizer_refresh_authorization:
 * @iface: A #ZpjAuthorizer.
 * @cancellable: (allow-none): An optional #GCancellable object, or
 *   %NULL.
 * @error: (allow-none): An optional #GError, or %NULL.
 *
 * Synchronously forces @iface to refresh any authorization tokens
 * held by it. See zpj_authorizer_refresh_authorization_async() for the
 * asynchronous version of this call.
 *
 * This method is thread safe.
 *
 * Returns: %TRUE if the authorizer now has a valid token.
 */
gboolean
zpj_authorizer_refresh_authorization (ZpjAuthorizer *iface, GCancellable *cancellable, GError **error)
{
  g_return_val_if_fail (ZPJ_IS_AUTHORIZER (iface), FALSE);
  return ZPJ_AUTHORIZER_GET_INTERFACE (iface)->refresh_authorization (iface, cancellable, error);
}


/**
 * zpj_authorizer_refresh_authorization_async:
 * @iface: A #ZpjAuthorizer.
 * @cancellable: (allow-none): An optional #GCancellable object, or
 *   %NULL.
 * @callback: (scope async): A #GAsyncReadyCallback to call when the
 *   request is satisfied.
 * @user_data: (closure): The data to pass to @callback.
 *
 * Asynchronously forces @iface to refresh any authorization tokens
 * held by it. See zpj_authorizer_refresh_authorization() for the
 * synchronous version of this call.
 *
 * When the operation is finished, @callback will be called. You can
 * then call zpj_authorizer_refresh_authorization_finish() to get the
 * result of the operation.
 *
 * This method is thread safe.
 */
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


/**
 * zpj_authorizer_refresh_authorization_finish:
 * @iface: A #ZpjAuthorizer.
 * @res: A #GAsyncResult.
 * @error: (allow-none): An optional #GError, or %NULL.
 *
 * Finishes an asynchronous operation started with
 * zpj_authorizer_refresh_authorization_async().
 *
 * Returns: %TRUE if the authorizer now has a valid token.
 */
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

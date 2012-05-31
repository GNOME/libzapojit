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

#ifndef ZPJ_AUTHORIZER_H
#define ZPJ_AUTHORIZER_H

#include <gio/gio.h>
#include <glib.h>
#include <libsoup/soup.h>
#include <rest/rest-proxy-call.h>

#include "zpj-authorization-domain.h"

G_BEGIN_DECLS

#define ZPJ_TYPE_AUTHORIZER (zpj_authorizer_get_type ())

#define ZPJ_AUTHORIZER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   ZPJ_TYPE_AUTHORIZER, ZpjAuthorizer))

#define ZPJ_IS_AUTHORIZER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   ZPJ_TYPE_AUTHORIZER))

#define ZPJ_AUTHORIZER_GET_INTERFACE(inst) \
  (G_TYPE_INSTANCE_GET_INTERFACE ((inst), \
   ZPJ_TYPE_AUTHORIZER, ZpjAuthorizerInterface))

typedef struct _ZpjAuthorizer          ZpjAuthorizer;
typedef struct _ZpjAuthorizerInterface ZpjAuthorizerInterface;

/**
 * ZpjAuthorizerInterface:
 * @parent_iface: The parent interface.
 * @is_authorized_for_domain: A method to check if the authorization
 *   tokens are valid for a #ZpjAuthorizationDomain.
 * @process_call: A method to append authorization headers to a
 *   #RestProxyCall.
 * @process_message: A method to append authorization headers to a
 *   #SoupMessage. Types of messages include DELETE, GET and POST.
 * @refresh_authorization: A synchronous method to force a refresh of
 *   any authorization tokens held by the authorizer. It should return
 *   %TRUE on success. An asynchronous version will be defined by
 *   invoking this in a thread.
 *
 * Interface structure for #ZpjAuthorizer. All methods should be
 * thread safe.
 */
struct _ZpjAuthorizerInterface
{
  GTypeInterface parent_iface;

  gboolean    (*is_authorized_for_domain)    (ZpjAuthorizer *iface,
                                              ZpjAuthorizationDomain *domain);
  void        (*process_call)                (ZpjAuthorizer *iface,
                                              ZpjAuthorizationDomain *domain,
                                              RestProxyCall *call);
  void        (*process_message)             (ZpjAuthorizer *iface,
                                              ZpjAuthorizationDomain *domain,
                                              SoupMessage *message);
  gboolean    (*refresh_authorization)       (ZpjAuthorizer *iface,
                                              GCancellable *cancellable,
                                              GError **error);
};

GType               zpj_authorizer_get_type                       (void) G_GNUC_CONST;

gboolean            zpj_authorizer_is_authorized_for_domain       (ZpjAuthorizer *iface,
                                                                   ZpjAuthorizationDomain *domain);

void                zpj_authorizer_process_call                   (ZpjAuthorizer *iface,
                                                                   ZpjAuthorizationDomain *domain,
                                                                   RestProxyCall *call);

void                zpj_authorizer_process_message                (ZpjAuthorizer *iface,
                                                                   ZpjAuthorizationDomain *domain,
                                                                   SoupMessage *message);

gboolean            zpj_authorizer_refresh_authorization          (ZpjAuthorizer *iface,
                                                                   GCancellable *cancellable,
                                                                   GError **error);

void                zpj_authorizer_refresh_authorization_async    (ZpjAuthorizer *iface,
                                                                   GCancellable *cancellable,
                                                                   GAsyncReadyCallback callback,
                                                                   gpointer user_data);

gboolean            zpj_authorizer_refresh_authorization_finish   (ZpjAuthorizer *iface,
                                                                   GAsyncResult *res,
                                                                   GError **error);

G_END_DECLS

#endif /* ZPJ_AUTHORIZER_H */

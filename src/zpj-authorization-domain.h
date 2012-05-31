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

#ifndef ZPJ_AUTHORIZATION_DOMAIN_H
#define ZPJ_AUTHORIZATION_DOMAIN_H

#include <glib-object.h>

G_BEGIN_DECLS

#define ZPJ_TYPE_AUTHORIZATION_DOMAIN (zpj_authorization_domain_get_type ())

#define ZPJ_AUTHORIZATION_DOMAIN(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   ZPJ_TYPE_AUTHORIZATION_DOMAIN, ZpjAuthorizationDomain))

#define ZPJ_AUTHORIZATION_DOMAIN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   ZPJ_TYPE_AUTHORIZATION_DOMAIN, ZpjAuthorizationDomainClass))

#define ZPJ_IS_AUTHORIZATION_DOMAIN(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   ZPJ_TYPE_AUTHORIZATION_DOMAIN))

#define ZPJ_IS_AUTHORIZATION_DOMAIN_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   ZPJ_TYPE_AUTHORIZATION_DOMAIN))

#define ZPJ_AUTHORIZATION_DOMAIN_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   ZPJ_TYPE_AUTHORIZATION_DOMAIN, ZpjAuthorizationDomainClass))

typedef struct _ZpjAuthorizationDomain        ZpjAuthorizationDomain;
typedef struct _ZpjAuthorizationDomainClass   ZpjAuthorizationDomainClass;
typedef struct _ZpjAuthorizationDomainPrivate ZpjAuthorizationDomainPrivate;

struct _ZpjAuthorizationDomain
{
  GObject parent_instance;
  ZpjAuthorizationDomainPrivate *priv;
};

struct _ZpjAuthorizationDomainClass
{
  GObjectClass parent_class;
};

GType               zpj_authorization_domain_get_type           (void) G_GNUC_CONST;

const gchar        *zpj_authorization_domain_get_scope          (ZpjAuthorizationDomain *self) G_GNUC_PURE;

const gchar        *zpj_authorization_domain_get_service_name   (ZpjAuthorizationDomain *self) G_GNUC_PURE;

G_END_DECLS

#endif /* ZPJ_AUTHORIZATION_DOMAIN_H */

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

#ifndef ZPJ_GOA_AUTHORIZER_H
#define ZPJ_GOA_AUTHORIZER_H

#include <glib-object.h>
#include <goa/goa.h>

G_BEGIN_DECLS

#define ZPJ_TYPE_GOA_AUTHORIZER (zpj_goa_authorizer_get_type ())

#define ZPJ_GOA_AUTHORIZER(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST ((obj), \
   ZPJ_TYPE_GOA_AUTHORIZER, ZpjGoaAuthorizer))

#define ZPJ_GOA_AUTHORIZER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST ((klass), \
   ZPJ_TYPE_GOA_AUTHORIZER, ZpjGoaAuthorizerClass))

#define ZPJ_IS_GOA_AUTHORIZER(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE ((obj), \
   ZPJ_TYPE_GOA_AUTHORIZER))

#define ZPJ_IS_GOA_AUTHORIZER_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE ((klass), \
   ZPJ_TYPE_GOA_AUTHORIZER))

#define ZPJ_GOA_AUTHORIZER_GET_CLASS(obj) \
  (G_TYPE_INSTANCE_GET_CLASS ((obj), \
   ZPJ_TYPE_GOA_AUTHORIZER, ZpjGoaAuthorizerClass))

typedef struct _ZpjGoaAuthorizer        ZpjGoaAuthorizer;
typedef struct _ZpjGoaAuthorizerClass   ZpjGoaAuthorizerClass;
typedef struct _ZpjGoaAuthorizerPrivate ZpjGoaAuthorizerPrivate;

/**
 * ZpjGoaAuthorizer:
 *
 * The #ZpjGoaAuthorizer structure contains only private data and
 * should only be accessed using the provided API.
 */
struct _ZpjGoaAuthorizer
{
  GObject parent_instance;
  ZpjGoaAuthorizerPrivate *priv;
};

/**
 * ZpjGoaAuthorizerClass:
 * @parent_class: The parent class.
 *
 * Class structure for #ZpjGoaAuthorizer.
 */
struct _ZpjGoaAuthorizerClass
{
  GObjectClass parent_class;
};

GType                zpj_goa_authorizer_get_type           (void) G_GNUC_CONST;

ZpjGoaAuthorizer    *zpj_goa_authorizer_new                (GoaObject *goa_object);

GoaObject           *zpj_goa_authorizer_get_goa_object     (ZpjGoaAuthorizer *self);

G_END_DECLS

#endif /* ZPJ_GOA_AUTHORIZER_H */

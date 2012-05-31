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

#include "zpj-authorization-domain.h"


struct _ZpjAuthorizationDomainPrivate
{
  gchar *scope;
  gchar *service_name;
};

enum
{
  PROP_0,
  PROP_SCOPE,
  PROP_SERVICE_NAME
};


G_DEFINE_TYPE (ZpjAuthorizationDomain, zpj_authorization_domain, G_TYPE_OBJECT);


static void
zpj_authorization_domain_finalize (GObject *object)
{
  ZpjAuthorizationDomain *self = ZPJ_AUTHORIZATION_DOMAIN (object);
  ZpjAuthorizationDomainPrivate *priv = self->priv;

  g_free (priv->scope);
  g_free (priv->service_name);

  G_OBJECT_CLASS (zpj_authorization_domain_parent_class)->finalize (object);
}


static void
zpj_authorization_domain_get_property (GObject *object, guint prop_id, GValue *value, GParamSpec *pspec)
{
  ZpjAuthorizationDomain *self = ZPJ_AUTHORIZATION_DOMAIN (object);
  ZpjAuthorizationDomainPrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_SCOPE:
      g_value_set_string (value, priv->scope);
      break;

    case PROP_SERVICE_NAME:
      g_value_set_string (value, priv->service_name);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_authorization_domain_set_property (GObject *object, guint prop_id, const GValue *value, GParamSpec *pspec)
{
  ZpjAuthorizationDomain *self = ZPJ_AUTHORIZATION_DOMAIN (object);
  ZpjAuthorizationDomainPrivate *priv = self->priv;

  switch (prop_id)
    {
    case PROP_SCOPE:
      priv->scope = g_value_dup_string (value);
      break;

    case PROP_SERVICE_NAME:
      priv->service_name = g_value_dup_string (value);
      break;

    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
    }
}


static void
zpj_authorization_domain_init (ZpjAuthorizationDomain *self)
{
  self->priv = G_TYPE_INSTANCE_GET_PRIVATE (self, ZPJ_TYPE_AUTHORIZATION_DOMAIN, ZpjAuthorizationDomainPrivate);
}


static void
zpj_authorization_domain_class_init (ZpjAuthorizationDomainClass *class)
{
  GObjectClass *object_class = G_OBJECT_CLASS (class);

  object_class->finalize = zpj_authorization_domain_finalize;
  object_class->get_property = zpj_authorization_domain_get_property;
  object_class->set_property = zpj_authorization_domain_set_property;

  g_object_class_install_property (object_class,
                                   PROP_SCOPE,
                                   g_param_spec_string ("scope",
                                                        "OAuth 2.0 scope",
                                                        "A URI detailing the scope of the authorization domain.",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_object_class_install_property (object_class,
                                   PROP_SERVICE_NAME,
                                   g_param_spec_string ("service-name",
                                                        "Service name",
                                                        "The name of the service which contains the authorization"
                                                        "domain.",
                                                        NULL,
                                                        G_PARAM_CONSTRUCT_ONLY | G_PARAM_READWRITE));

  g_type_class_add_private (class, sizeof (ZpjAuthorizationDomainPrivate));
}


const gchar *
zpj_authorization_domain_get_scope (ZpjAuthorizationDomain *self)
{
  return self->priv->scope;
}


const gchar *
zpj_authorization_domain_get_service_name (ZpjAuthorizationDomain *self)
{
  return self->priv->service_name;
}

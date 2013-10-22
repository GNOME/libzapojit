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

#ifndef ZPJ_ERROR_H
#define ZPJ_ERROR_H

#include <glib.h>

G_BEGIN_DECLS

/**
 * ZPJ_ERROR:
 *
 * Error domain for libzapojit. Errors in this domain will be from the
 * #ZpjError enumeration. See #GError for more information on error domains.
 */
#define ZPJ_ERROR (zpj_error_quark ())

/**
 * ZpjError:
 * @ZPJ_ERROR_REQUEST_URL_INVALID: The server received an invalid URL,
 *   possibly because of an invalid @file_id.
 * @ZPJ_ERROR_UNKNOWN: An unknown error occurred.
 *
 * This enumeration can be expanded at a later date.
 */
typedef enum
{
  ZPJ_ERROR_REQUEST_URL_INVALID,
  ZPJ_ERROR_UNKNOWN
} ZpjError;

GQuark    zpj_error_quark        (void);

G_END_DECLS

#endif /* ZPJ_ERROR_H */

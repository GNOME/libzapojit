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

#include "zpj-error.h"


/**
 * SECTION:zpj-error
 * @title: ZpjError
 * @short_description: Error codes.
 * @include: zpj/zpj.h
 *
 * Error codes used to represent errors thrown by the SkyDrive and
 * Hotmail REST APIs.
 */


GQuark
zpj_error_quark (void)
{
  return g_quark_from_static_string ("zpj-error-quark");
}

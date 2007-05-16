/*
 * Copyright (C) 2006, 2007 Free Software Foundation, Inc.
 * Written by Negreanu Marius <groleo@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program; see the file COPYING.  If not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA.
 */

/**
 *   \brief    Wraps around rm command.
 */
#include "config.h"
#include "dbg.h"
#include "sock.h"
#include "strop.h"
#include "sut.h"

#define TOOL_NAME start
#define TOOL_DESCRIPTION "Remove (unlink) the FILE(s).\n"
#define CMD_FUNC\
    ret = fop_rm( testType, argv[optind], cfg.hostname, cfg.rawport )
#include "template.c"

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
#include "config.h"
#include "dbg.h"
#include "sock.h"
#include "strop.h"
#include "sut.h"
#include "core.h"

#define TOOL_NAME stop
#define HAVE_CMD_ARG 1
#define TOOL_DESCRIPTION "Stop the product under test.\n"
#define CMD_FUNC\
    ret = sut_stop( testType, 5, getenv( SUT_SYSLOG ), cmd, cfg.hostname,\
            cfg.rawport );
#include "template.c"

/***********************************************************************************\
 * Copyright (C) 2004 Steven Deprez                                                *
 *                                                                                 *
 * This file is part of zasm                                                       *
 *                                                                                 *
 * zasm is free software; you can redistribute it and/or modify                    *
 * it under the terms of the GNU General Public License as published by            *
 * the Free Software Foundation; either version 2 of the License, or               *
 * (at your option) any later version.                                             *
 *                                                                                 *
 * This program is distributed in the hope that it will be useful,                 *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of                  *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                   *
 * GNU General Public License for more details.                                    *
 *                                                                                 *
 * You should have received a copy of the GNU General Public License               *
 * along with this program; if not, write to the Free Software                     *
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.     *
 *                                                                                 *
 * In addition, as a special exception, Peter-Martijn Kuipers gives permission     *
 * to link the code of this program with the appsign library, and distribute       *
 * linked combinations including the two. If you modify this file, you may extend  *
 * this exception to your version of the file, but you are not obligated to do so. *
 * If you do not wish to do so, delete this exception statement form your version. *
\***********************************************************************************/

#define ELSIFFALSE_C
#include <config.h>
#include "../preprocessor.h"
#include "cores.h"

static char *dir_if_l(char *args)
{
  dir_if_depth++;
  return "";
}

static char *dir_endif_l(char *args)
{
  dir_if_depth--;
  if (dir_if_depth < dir_if_false_depth)
    {
      if (core_deactivate()<0)
	ERROR("internal error\n");
    }
  return "";
}

struct directives_t core_dir_elsiffalse[] =
  {
    {"if", dir_if_l},
    {"ifdef", dir_if_l},
    {"ifndef", dir_if_l},
    {"endif", dir_endif_l},
    {"else", dir_ignore},
    {"elsif", dir_ignore},
    {"elseif", dir_ignore},
    {"elsifdef", dir_ignore},
    {"elseifdef", dir_ignore},
    {"elsifndef", dir_ignore},
    {"elseifndef", dir_ignore},

    {"define", dir_ignore},
    {"declare", dir_ignore},
    {"macro", dir_ignore},
    {"equ", dir_ignore},
    {"const", dir_ignore},
    {"undef", dir_ignore},

    {"include", dir_ignore},
    {"input", dir_ignore},

    {"dw", dir_ignore},
    {"db", dir_ignore},

    {"org", dir_ignore},
    {"file", dir_ignore},
    {"line", dir_ignore},
    {"page", dir_ignore},

    {"newpage", dir_ignore},
    {NULL, NULL}
  };

struct core_t core_elsiffalse = {core_pp_line_iffalse, core_dir_elsiffalse, NULL, NULL};

/* Declarations for termcap library.
   Copyright (C) 1991, 1992, 1995 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#ifndef _TERMCAP_H
#define _TERMCAP_H 1

#include <features.h>

__BEGIN_DECLS

#define PC (*__get_tc_pc())
extern char *__get_tc_pc(void);

#define ospeed (*__get_tc_ospeed())
extern short *__get_tc_ospeed(void);

#define UP (*__get_tc_up())
extern char **__get_tc_up(void);

#define BC (*__get_tc_bc())
extern char **__get_tc_bc(void);

extern int tgetent (char *buffer, const char *termtype);
extern int tgetnum (const char *name);
extern int tgetflag (const char *name);
extern char *tgetstr (const char *name, char **area);
extern void tputs (const char *str, int nlines, int (*outfun) (int));
extern char *tparam (const char *string, char *outstring, int leb, int arg0, int arg1, int arg2, int arg3);
extern char *tgoto (const char *cstring, int hpos, int vpos);

__END_DECLS

#endif /* not _TERMCAP_H */

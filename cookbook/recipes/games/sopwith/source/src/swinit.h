// Emacs style mode select -*- C++ -*-
//---------------------------------------------------------------------------
//
// $Id: swinit.h,v 1.3 2003/06/04 15:33:53 fraggle Exp $
//
// Copyright(C) 1984-2000 David L. Clark
// Copyright(C) 2001-2003 Simon Howard
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the GNU General Public License as published by the
// Free Software Foundation; either version 2 of the License, or (at your
// option) any later version. This program is distributed in the hope that
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied
// warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See
// the GNU General Public License for more details. You should have
// received a copy of the GNU General Public License along with this
// program; if not, write to the Free Software Foundation, Inc., 59 Temple
// Place - Suite 330, Boston, MA 02111-1307, USA.
//
//---------------------------------------------------------------------------

#ifndef __SWINIT_H__
#define __SWINIT_H__

#include "sw.h"

// global argc/argv

extern int g_argc;
extern char **g_argv;

extern void swinit(int argc, char *argv[]);
extern void swinitlevel();
extern void swrestart();
extern void initdisp(BOOL reset);
extern void dispguages(OBJECTS *ob); 
extern void initcomp(OBJECTS *obp);
extern void initplyr(OBJECTS *obp);
extern OBJECTS *initpln(OBJECTS *obp);
extern void initshot(OBJECTS *obop, OBJECTS *targ);
extern void initbomb(OBJECTS *obop);
extern void initmiss(OBJECTS *obop);
extern void initburst(OBJECTS *obop);
extern void initexpl(OBJECTS *obop, int small);
extern void initsmok(OBJECTS *obop);
extern void initbird(OBJECTS *obop, int i);

#endif


//---------------------------------------------------------------------------
//
// $Log: swinit.h,v $
// Revision 1.3  2003/06/04 15:33:53  fraggle
// Store global argc/argv for gtk_init
//
// Revision 1.2  2003/04/05 22:44:04  fraggle
// Remove some useless functions from headers, make them static if they
// are not used by other files
//
// Revision 1.1.1.1  2003/02/14 19:03:31  fraggle
// Initial Sourceforge CVS import
//
//
// sdh 14/2/2003: change license header to GPL
// sdh 26/10/2001: merge guages into a single function
// sdh 21/10/2001: added cvs tags
// sdh 19/10/2001: added header
//
//---------------------------------------------------------------------------


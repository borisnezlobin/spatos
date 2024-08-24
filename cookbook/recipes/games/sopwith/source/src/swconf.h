// Emacs style mode select -*- C++ -*-
//--------------------------------------------------------------------------
//
// $Id: swconf.h,v 1.1.1.1 2003/02/14 19:03:30 fraggle Exp $
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
//--------------------------------------------------------------------------
//
// Configuration code
//
// Save game settings to a configuration file
//
//-------------------------------------------------------------------------

#ifndef __SWCONF_H__
#define __SWCONF_H__

typedef struct
{
	char *name;
	enum {
		CONF_BOOL,
		CONF_INT
	} type;
	union {
		void *v;
		BOOL *b;
		int *i;
	} value;
	char *description;
} confoption_t;

extern confoption_t confoptions[];
extern int num_confoptions;

extern void swloadconf();
extern void swsaveconf();
extern void setconfig();          // config menu

#endif

//-------------------------------------------------------------------------
//
// $Log: swconf.h,v $
// Revision 1.1.1.1  2003/02/14 19:03:30  fraggle
// Initial Sourceforge CVS import
//
//
// sdh 14/2/2003: change license header to GPL
// sdh 10/11/2001: make confoptions available globally for gtk code
//
//-------------------------------------------------------------------------

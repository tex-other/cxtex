/*
Copyright (c) 1996-2002 Han The Thanh, <thanh@pdftex.org>

This file is part of pdfTeX.

pdfTeX is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

pdfTeX is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with pdfTeX; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

$Id: pdftexextra.h,v 1.2 2004/05/11 14:30:32 taco Exp $
*/

/* pdftexextra.h: banner etc. for pdfTeX.

   This is included by pdfTeX, from ../etexextra.c
   (generated from ../lib/texmfmp.c).
*/

#define BANNER "This is pdfTeX, Version 3.141592-1.11b"
#define COPYRIGHT_HOLDER "Han The Thanh"
#define AUTHOR NULL
#define PROGRAM_HELP PDFTEXHELP
#define DUMP_VAR TEXformatdefault
#define DUMP_LENGTH_VAR formatdefaultlength
#define DUMP_OPTION "fmt"
#define INPUT_FORMAT kpse_tex_format
#define INI_PROGRAM "pdfinitex"
#define VIR_PROGRAM "pdfvirtex"

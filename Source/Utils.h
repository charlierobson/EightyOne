/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2006 Michael D Wynne
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 * Utils.h
 *
 */

//---------------------------------------------------------------------------

#ifndef UtilsH
#define UtilsH
//---------------------------------------------------------------------------

#include "zx81config.h"

#ifdef __cplusplus
extern "C" {
#endif

int CRC32Block(BYTE *block, int len);
void ASCIIZX81(BYTE *in, BYTE *Output);
int ZX81Strlen(BYTE *str);

#ifdef __cplusplus
}
#endif

ZXString FileNameGetPath(ZXString Fname);
ZXString FileNameGetFname(ZXString Fname);
ZXString FileNameGetExt(ZXString Fname);
ZXString ConvertZX81ASCII(BYTE *str);
void ConvertASCIIZX81(ZXString Str, BYTE *Output);
ZXString RemoveExt(ZXString Fname);
ZXString GetExt(ZXString Fname);
ZXString RemovePath(ZXString Str);
int EnumeratePorts(TStrings *List, ZXString Type);

#endif


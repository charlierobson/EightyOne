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
 * ql.h
 *
 */

#ifndef ql_h
#define ql_h

extern BYTE memory[];
extern BYTE acecolour[];
extern void ql_initialise();
extern int ql_do_scanline(SCANLINE *CurScanLine);
extern "C" void ql_writebyte(int Address, int Data);
extern "C" BYTE ql_readbyte(int Address);
extern "C" BYTE ql_readoperandbyte(int Address);
extern BYTE ql_opcode_fetch(int Address);
extern void ql_writeport(int Address, int Data, int *tstates);
extern BYTE ql_readport(int Address, int *tstates);
extern int ql_contend(int Address, int states, int time);
extern int tstates, event_next_event;
extern int framepos, frametstates;
extern BOOL nmiGeneratorEnabled, syncOutputWhite;
extern int emulation_stop;
extern BYTE ZXKeyboard[];
extern int ql_do_accurate();

#endif

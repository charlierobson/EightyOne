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
 * cr81.cpp
 *
 */
#include <vcl4.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>


#include "zx81.h"
#include "z80/z80.h"
#include "snap.h"
#include "WavCInterface.h"
#include "sound.h"
#include "dev8255.h"
#include "serialport.h"
#include "zxprinter_c.h"
#include "rompatch.h"
#include "tzxman.h"
#include "iecbus.h"
#include "ide.h"

void cr81_initialise(void)
{
}

void cr81_writebyte(int Address, int Data)
{
}

BYTE cr81_readbyte(int Address)
{
        return 0;
}

void cr81_setbyte(int Address, int Data)
{
}

BYTE cr81_getbyte(int Address)
{
        return 0;
}

BYTE cr81_getopcodebyte(int Address)
{
        return 0;
}
BYTE cr81_opcode_fetch(int Address)
{
        return 0;
}

void cr81_writeport(int Address, int Data, int *tstates)
{
}

BYTE cr81_readport(int Address, int *tstates)
{
        return(255);
}

int cr81_contend(int Address, int states, int time)
{
        return(time);
}


int cr81_do_scanline(SCANLINE *CurScanLine)
{
        return 0;
}



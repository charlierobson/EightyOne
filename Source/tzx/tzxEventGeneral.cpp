/* EightyOne  - A Windows ZX80/81/clone emulator.
 * Copyright (C) 2003-2007 Michael D Wynne
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
 *
 * tzxEventGeneral.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#include <stdio.h>
#pragma hdrstop

#include "tzxfile.h"
#include "zx81config.h"
#include "wavload_.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

#define TZX_BYTE_EMPTY -1

#define SetEarState(State)                                      \
        switch(State)                                           \
        {                                                       \
        case 0: PulseList++; break;                             \
        case 1: EarState = !EarState; PulseList++; break;       \
        case 2: EarState = 0; PulseList++; break;               \
        case 3: EarState = 1; PulseList++; break;               \
        }                                                       \
        if ((emulator.machine != MACHINESPECTRUM)               \
                && (WavLoad->IgnoreZX81==false) )               \
                        EarState=!EarState;                     \

int TZXByte=TZX_BYTE_EMPTY;

bool TTZXFile::EventGeneral(void)
{
        static unsigned char *data;
        static unsigned short *CurPR, *PulseList;
        static unsigned char SymbolSize, SymbolMask, SymbolShift, CurrentBitCount, PulseCount;
        static int usedbits, CurrentBit, CurrentByte, CurPRCount, TOTP, phase;
        static long DataLen;

        if (!BlockInProgress)
        {
                int Syms;
                phase=0;

                data=Tape[CurBlock].Data.Data -1;
                DataLen=Tape[CurBlock].Head.General.DataLen;
                TOTP=Tape[CurBlock].Head.General.TOTP;

                if (FlashLoad && emulator.machine!=MACHINESPECTRUM) data++;

                Syms=Tape[CurBlock].Head.General.ASD; if (Syms) Syms -= 1;
                SymbolSize=0;

                while(Syms)
                {
                        SymbolSize +=(unsigned char)1;
                        Syms >>= 1;
                }

                SymbolMask = (unsigned char)((1<<SymbolSize) -1);
                SymbolShift = (unsigned char)(8-SymbolSize);
                SymbolMask <<= SymbolShift;

                usedbits=8-(DataLen*8 - Tape[CurBlock].Head.General.TOTD*SymbolSize);

                BlockInProgress=true;

                if (TOTP==0) phase=2;

                if (FlashLoad && emulator.machine!=MACHINESPECTRUM) phase=5;

                TZXByte=TZX_BYTE_EMPTY;
                FlashLoadable=1;
        }

        if (phase==5)
        {
                EarState=1;
                TZXEventCounter=4;
                if (TZXByte==TZX_BYTE_EMPTY)
                {
                        DataLen--;
                        if (DataLen==-1)
                        {
                                EarState=0;
                                TZXByte=TZX_BYTE_EMPTY;
                                Pause= Tape[CurBlock].Pause / 100;
                                CurBlockProgress=0;
                                CurBlockLen=0;
                                EventNextBlock();
                                return(true);
                        }

                        TZXByte=data[0];
                        data++;
                }
        }

        if (phase==0)
        {
                CurPR=Tape[CurBlock].PRLE;
                CurPRCount=CurPR[1];
                PulseList=Tape[CurBlock].SymDefP + (CurPR[0] * Tape[CurBlock].Head.General.NPP);

                SetEarState(*PulseList);
                PulseCount=1;
                phase=1;
        }

        if (phase==1)
        {
                if (*PulseList && PulseCount<Tape[CurBlock].Head.General.NPP)
                {
                        TZXEventCounter += TZXSCALE(*PulseList);
                        EarState=!EarState;
                        PulseList++;
                        PulseCount++;
                        return(false);
                }

                if (--CurPRCount == 0)
                {
                        CurPR+=2;
                        CurPRCount=CurPR[1];
                        if (--TOTP==0)
                        {
                                phase=2;
                                return(false);
                        }
                }

                PulseList=Tape[CurBlock].SymDefP + (CurPR[0] * Tape[CurBlock].Head.General.NPP);
                SetEarState(*PulseList);
                PulseCount=1;
                return(false);
        }

        if (phase==2)
        {
                if (DataLen==0)
                {
                        TZXByte=TZX_BYTE_EMPTY;
                        Pause=Tape[CurBlock].Pause;
                        CurBlockLen=0;
                        CurBlockProgress=0;
                        EventNextBlock();
                        return(true);
                }

                CurBlockLen=DataLen;
                CurBlockProgress=0;

                CurrentBitCount=0;
                CurrentByte=(data[0]<<8) | data[1];
                CurrentBitCount=8;
                CurrentBit=(CurrentByte&SymbolMask) >> SymbolShift;
                PulseList = Tape[CurBlock].SymDefD + (CurrentBit*Tape[CurBlock].Head.General.NPD);

                SetEarState(*PulseList);
                PulseCount=1;
                phase=3;
        }

        if (phase==3)
        {
                if (*PulseList && PulseCount<Tape[CurBlock].Head.General.NPD)
                {
                        TZXEventCounter += TZXSCALE(*PulseList);
                        EarState=!EarState;
                        PulseList++;
                        PulseCount++;
                        return(false);
                }

                CurrentBitCount -= SymbolSize;
                PulseCount=1;

                if (CurrentBitCount<=0)
                {
                        data++;
                        CurrentByte=(data[0]<<8) | data[1];

                        CurBlockProgress++;

                        if (DataLen==2) CurrentBitCount += (unsigned char)usedbits;
                        else CurrentBitCount+=(unsigned char)8;

                        if (!--DataLen)
                        {
                                TZXByte=TZX_BYTE_EMPTY;
                                Pause=Tape[CurBlock].Pause;
                                CurBlockLen=0;
                                CurBlockProgress=0;
                                EventNextBlock();
                                return(true);
                        }
                }
                else    CurrentByte<<=SymbolSize;

                CurrentBit=(CurrentByte&SymbolMask) >> SymbolShift;
                PulseList = Tape[CurBlock].SymDefD + (CurrentBit*Tape[CurBlock].Head.General.NPD);
                SetEarState(*PulseList);
                return(false);
        }

        return(false);
}


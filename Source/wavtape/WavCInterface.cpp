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
 *
 * WavCInterface.cpp
 */

#include "Wavload_.h"
#include "WavCInterface.h"
#include "tzxman.h"
#include "tzxfile.h"
#include "zx81config.h"

void WavRecordByte(int Byte)
{
        if (emulator.TZXout) TZX->RecordByte((unsigned char)Byte);
}

int WavFlashLoad(void)
{
        if (emulator.TZXin) return(TZX->FlashLoadBtn->Down);
        else return false;
}

void WavRewind(void)
{
        if (emulator.TZXin) TZX->RewEndClick(NULL);
        else WavLoad->RewStartClick(NULL);
}

int IsFlashLoadable(void)
{
        if (emulator.TZXin) return TZXFile.FlashLoadable;
        return(0);
}

int IsFlashSaveable(void)
{
        if (emulator.TZXout) return 1;
        return(0);
}

int GetEarState()
{
        if (emulator.TZXin) return(TZXFile.GetEarState());
        return(WavLoad->GetEarState());
}

void WavClockTick(int TStates, int MicState)
{
        TZX->ClockTick(TStates, true);
        WavLoad->ClockTick(TStates, true, MicState);
}

void WavStop()
{
        if ((TZX->PlayBtn->Down && TZX->AutoStartBtn->Down) || TZX->RecState!=REC_STOP)
                TZX->StopBtnClick(NULL);
        WavLoad->Stop(false);
}

void WavStart()
{
        if (emulator.TZXin)
        {
                if (TZX->AutoStartBtn->Down)
                        TZX->PlayBtnClick(NULL);
        }
        else    WavLoad->Start();
}

void WavStartRec()
{
        if (!emulator.TZXout) WavLoad->StartRec();
}

int WavPlaying()
{
        if (emulator.TZXin) return(TZXFile.Playing);
        return(WavLoad->Playing);
}

int WavInGroup()
{
        if (emulator.TZXin) return(TZXFile.Tape[TZXFile.CurBlock].Group);
        else return(0);
}


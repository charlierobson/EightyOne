/* EightyOne - a Sinclair emulators.
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
 * midi.cpp - Midi support.  Basicly, collect data one bit at a time from the
  * emulated machine, reassemble the data then send it to the host midi device
 */

#include <windows.h>
#include "zx81config.h"
#include "midi.h"

#define TIMEOUT 300000;

CMidi Midi;

CMidi::CMidi(void)
{
        Device=-1;
        MidiSerialCount=TIMEOUT;
        MidiBufferLen=0;
        outHandle=NULL;
}

void CMidi::WriteBit(int Bit)
{
        // Only the +2A/+3 have midi out, so if we're emulating
        // anything else ignore the midi data
        // Midi is transmitted at 31.25 kbs, 1 start bit, 8 data bits, MSB first,
        //  then 1 stop bit.

        if (emulator.machine==MACHINESPECTRUM &&
                ( spectrum.model==SPECCYPLUS2A || spectrum.model==SPECCYPLUS3))
        {
                // Midi data arrives in bit 2 of the port, so to build a midi byte
                // we do the following:
                // 1. Shift the data received right twice.
                // 2. Ignore everything except the (now) least sugnificant bit
                // 3. Shift it left 8 bita
                // 4. Or it with what we've already received.
                // Once the start bit reaches bot 0, we've receieved a full byte,
                // So output it.

                MidiByte = (MidiByte>>1) | (((~Bit>>2)&1)<<8);

                MidiSerialCount=TIMEOUT;
                if (MidiByte&1)
                {
                        Write((~MidiByte>>1)&255);
                        MidiByte=0;
                }
        }
}

void CMidi::ClockTick(int ts)
{
        if (MidiSerialCount>0) MidiSerialCount -= ts;
        else MidiByte=0;
}

void CMidi::Write(int Byte)
{
        if ((Byte&128) && (MidiBufferLen!=0)) MidiBufferLen=0;
        else if (!(Byte&128) && (MidiBufferLen==0))
                {
                        if (MidiBuffer[0]&128) MidiBufferLen=1;
                        else return;
                }

        MidiBuffer[MidiBufferLen++]=(unsigned char)Byte;

        if ((((MidiBuffer[0]&0xfd) == 0xf1) || ((MidiBuffer[0]&0xe0) == 0xc0))
                && (MidiBufferLen<2)) return;

        if ((MidiBuffer[0]!=0xf0) && MidiBufferLen<3) return;

        if ((MidiBuffer[0]==0xf0) && (Byte!=0xf7)) return;
        if ((MidiBuffer[0]==0xf0) && (Byte==0xf7))
        {
                MidiBufferLen=0;
                return;
        }

        if (outHandle)
                midiOutShortMsg((HMIDIOUT)outHandle, *((int *)MidiBuffer));
        MidiBuffer[0]=MidiBuffer[1]=MidiBuffer[2]=MidiBuffer[3]=0;
}

void CMidi::Start(void)
{
        Stop();
        if (Device==-2) return;

        if (midiOutOpen((LPHMIDIOUT)&outHandle, Device, 0, 0, CALLBACK_NULL))
                outHandle=NULL;
}

void CMidi::Stop(void)
{
        if (outHandle) midiOutClose((HMIDIOUT)outHandle);
        outHandle=NULL;
}


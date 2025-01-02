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
 */

#include "Spectra.h"

/*
SPECTRA is a multi-function interface for the Spectrum, with the primary purpose
of providing a clear, sharp picture on most modern TV sets via RGB SCART. The
interface plugs into the Spectrum expansion bus and requires no non-standard
modifications to the Spectrum.

The interface provides the following facilities:
- Provides 31 new colour attribute modes
- Supports palette of 64 colours
- Supports colour attribute resolution down to 2x1 pixels
- Kempston joystick socket
- ZX Interface 1 compatible RS232 socket
- ZX Interface 2 compatible ROM cartridge socket
- Onboard 16K EPROM to override the Spectrum ROM
- ZX Interface 1 paging mechanism emulation 
- BEEP sounds through the TV speaker(s)
- Works with Spectrum 16K-48K, 128, +2 (+2B and +3 via an adapter)
- Dual TV mode possible on 128K Spectrums
- Reset button

Full details on the interface can be found at www.fruitcake.plus.com
*/

const BYTE spectraDisplayModeRow = 0x00;
const BYTE spectraDisplayModeQuad = 0x01;
const BYTE spectraDisplayModeDual = 0x02;
const BYTE spectraDisplayModeSingle = 0x03;
const BYTE spectraDisplayMode = 0x03;
const BYTE spectraExtraColours = 0x04;
const BYTE spectraDoubleByte = 0x08;
const BYTE spectraEnhancedBorder = 0x10;
const BYTE spectraDisplayBank = 0x20;
const BYTE spectraShadowBank = 0x40;
const BYTE spectraHalfCell = 0x80;

const BYTE colourWhite = 0x2A;
const BYTE colourBlack = 0x00;

int spectraDisplayBankOffset;
BYTE SpectraMem[32*1024];  // Spectra has two banks of 16K RAM

extern int SPECBankEnable;
extern BYTE SPECLast7ffd;

void DisableSpectra()
{
        spectrum.spectraMode = 0x00;
        spectraDisplayBankOffset = 0x0000;
}

void InitialiseSpectra()
{
        DisableSpectra();

        for(unsigned int i = 0; i < sizeof(SpectraMem); i++)
                SpectraMem[i] = 0x00;
}

BYTE SpectraRAMRead(int bankOffset)
{
        return SpectraMem[spectraDisplayBankOffset + bankOffset];
}

void SpectraRAMWrite(int Address, BYTE Data)
{
        bool write4000 = ((Address >= 0x4000) && (Address <= 0x7FFF));
        bool detectWriteAtC000 = SPECBankEnable;
        bool writeC000 = detectWriteAtC000 && ((Address >= 0xC000) && (Address <= 0xFFFF));
        int bank128 = (SPECLast7ffd & 0x07);
        bool writeBank5 = (write4000 || (writeC000 && (bank128 == 0x05)));
        bool writeBank7 = (writeC000 && (bank128 == 0x07));
        bool shadowScreen1 = (spectrum.spectraMode & spectraShadowBank);

        if (writeBank7)
        {
                int shadowBankOffset = shadowScreen1 ? 0x0000 : 0x4000;
                SpectraMem[shadowBankOffset | (Address & 0x3FFF)] = Data;
        }
        else if (writeBank5)
        {
                int shadowBankOffset = shadowScreen1 ? 0x4000 : 0x0000;
                SpectraMem[shadowBankOffset | (Address & 0x3FFF)] = Data;
        }
}

void FetchSpectraAttributeFileBytes(int y, int column, int* attr1, int* attr2)
{
        int area, row, line;
        int lineOffset, secondByteOffset, baseOffset;

        switch (spectrum.spectraMode & spectraDisplayMode)
        {
        case spectraDisplayModeRow:
                area = ((y >> 3) & 0x18);
                line = 0;
                row = ((y >> 3) & 0x07);
                secondByteOffset = (spectrum.spectraMode & spectraDoubleByte) ? 0x0400 : 0x0000;
                baseOffset = 0x1800;
                break;

        case spectraDisplayModeQuad:
                area = ((y >> 2) & 0x30);
                line = ((y << 1) & 0x08);
                row = ((y >> 3) & 0x07);
                secondByteOffset = (spectrum.spectraMode & spectraDoubleByte) ? 0x0800 : 0x0000;
                baseOffset = 0x2000;
                break;

        case spectraDisplayModeDual:
                area = ((y >> 1) & 0x60);
                line = ((y << 2) & 0x18);
                row = ((y >> 3) & 0x07);
                secondByteOffset = (spectrum.spectraMode & spectraDoubleByte) ? 0x1000 : 0x0000;
                baseOffset = 0x2000;
                break;

        case spectraDisplayModeSingle:
                if (spectrum.spectraMode & spectraDoubleByte)
                {
                        if (y < 128)
                        {
                                // Single line mode in top 2/3rd of screen
                                area = (y & 0x40);
                                line = ((y << 3) & 0x38);
                                row = ((y >> 3) & 0x07);
                                secondByteOffset = 0x1000;
                                baseOffset = 0x2000;
                        }
                        else
                        {
                                // Dual line mode in bottom 3rd of screen
                                area = 0;
                                line = ((y << 2) & 0x18);
                                row = ((y >> 3) & 0x07);
                                secondByteOffset = 0x0400;
                                baseOffset = 0x1800;
                        }
                }
                else
                {
                        area = (y & 0xC0);
                        line = ((y << 3) & 0x38);
                        row = ((y >> 3) & 0x07);
                        secondByteOffset = 0x0000;
                        baseOffset = 0x2000;
                }
                break;
        }

        lineOffset = ((area | line | row) << 5);
        int addrOffset = baseOffset + lineOffset + column;
                                                                   
        *attr1 = SpectraMem[spectraDisplayBankOffset + addrOffset];
        *attr2 = SpectraMem[spectraDisplayBankOffset + addrOffset + secondByteOffset];
}

void DetermineSpectraInkPaper(int attr1, int attr2, int flashSwap, int* inkLeft, int* inkRight, int* paperLeft, int* paperRight)
{
        const int sb8 = 0;
        const int db8 = 1;
        const int se8 = 2;
        const int de8 = 3;
        const int sb4 = 4;
        const int db4 = 5;
        const int se4 = 6;
        const int de4 = 7;

        const int brightColour = 0x40;
        const int b1 = 0x01;
        const int r1 = 0x02;
        const int g1 = 0x04;
        const int b0 = 0x08;
        const int r0 = 0x10;
        const int g0 = 0x20;
        const int colourMask = 0x3F;
        const int paperWhite = 0x40;

        int db = spectrum.spectraMode & spectraDoubleByte ? 1 : 0;
        int ec = spectrum.spectraMode & spectraExtraColours ? 2 : 0;
        int hc = spectrum.spectraMode & spectraHalfCell ? 4 : 0;
        int displayMode = hc | ec | db;

        switch (displayMode)
        {
                case sb8:
                {
                        *inkLeft = ((attr1 & g1) << 3) | ((attr1 & r1) << 2) | ((attr1 & b1) << 1);
                        *paperLeft = (attr1 & g0) | ((attr1 & r0) >> 1) | ((attr1 & b0) >> 2);
                        if (attr1 & brightColour)
                        {
                                *inkLeft |= (*inkLeft >> 1);
                                *paperLeft |= (*paperLeft >> 1);
                        }

                        *inkRight = *inkLeft;
                        *paperRight = *paperLeft;

                        break;
                }
                case db8:
                {
                        *inkLeft = ((attr1 & g1) << 3) | ((attr1 & r1) << 2) | ((attr1 & b1) << 1);
                        if (attr1 & brightColour) { *inkLeft += (*inkLeft >> 1); }

                        *paperLeft = ((attr2 & g1) << 3) | ((attr2 & r1) << 2) | ((attr2 & b1) << 1);
                        if (attr2 & brightColour) { *paperLeft += (*paperLeft >> 1); }

                        *inkRight = *inkLeft;
                        *paperRight = *paperLeft;
                        break;
                }
                case se8:
                {
                        *inkLeft = (attr1 & colourMask);
                        *paperLeft = (attr1 & paperWhite) ? colourWhite : colourBlack;

                        *inkRight = *inkLeft;
                        *paperRight = *paperLeft;
                        break;
                }
                case de8:
                {
                        *inkLeft = (attr1 & colourMask);
                        *paperLeft = (attr2 & colourMask);

                        *inkRight = *inkLeft;
                        *paperRight = *paperLeft;
                        break;
                }
                case sb4:
                {
                        *inkRight = ((attr1 & g1) << 3) | ((attr1 & r1) << 2) | ((attr1 & b1) << 1);
                        *inkLeft = (attr1 & g0) | ((attr1 & r0) >> 1) | ((attr1 & b0) >> 2);
                        if (attr1 & brightColour)
                        {
                                *inkLeft |= (*inkLeft >> 1);
                                *inkRight |= (*inkRight >> 1);
                        }
                        *paperLeft = colourBlack;
                        *paperRight = colourBlack;
                        break;
                }
                case db4:
                {
                        *inkRight = ((attr1 & g1) << 3) | ((attr1 & r1) << 2) | ((attr1 & b1) << 1);
                        *inkLeft = (attr1 & g0) | ((attr1 & r0) >> 1) | ((attr1 & b0) >> 2);
                        if (attr1 & brightColour)
                        {
                                *inkLeft |= (*inkLeft >> 1);
                                *inkRight |= (*inkRight >> 1);
                        }

                        *paperRight = ((attr2 & g1) << 3) | ((attr2 & r1) << 2) | ((attr2 & b1) << 1);
                        *paperLeft = (attr2 & g0) | ((attr2 & r0) >> 1) | ((attr2 & b0) >> 2);
                        if (attr2 & brightColour)
                        {
                                *paperLeft |= (*paperLeft >> 1);
                                *paperRight |= (*paperRight >> 1);
                        }
                        break;
                }
                case se4:
                {
                        *inkRight = (attr1 & colourMask);
                        *inkLeft = (attr1 & paperWhite) ? colourWhite : colourBlack;

                        *paperRight = colourBlack;
                        *paperLeft = colourBlack;
                        break;
                }
                case de4:
                {
                        *inkRight = (attr1 & colourMask);
                        *paperRight = colourBlack; // (attr1 & paperWhite) ? colourWhite : colourBlack;

                        *inkLeft = (attr2 & colourMask);
                        *paperLeft = colourBlack; // (attr2 & paperWhite) ? colourWhite : colourBlack;
                        break;
                }
        }

        if (flashSwap)
        {
                const int flashColour = 0x80;

                int tempInkLeft = *inkLeft;
                int tempInkRight = *inkRight;
                                     
                if (attr1 & flashColour)
                {
                        *inkLeft = *paperLeft;
                        *inkRight = *paperRight;
                }

                if (attr2 & flashColour)
                {
                        *paperLeft = tempInkLeft;
                        *paperRight = tempInkRight;
                }
        }
}

int DetermineSpectraBorderColour(int Data, int flashSwap)
{
        const int b1 = 0x01;
        const int r1 = 0x02;
        const int g1 = 0x04;

        int borderColour;

        if  (spectrum.spectraMode & spectraEnhancedBorder)
        {
                if (spectrum.spectraMode & spectraExtraColours)
                {
                        const int b0 = 0x20;
                        const int r0 = 0x40;
                        const int g0 = 0x80;

                        borderColour = ((Data & g1) << 3) | ((Data & g0) >> 3) |
                                       ((Data & r1) << 2) | ((Data & r0) >> 4) |
                                       ((Data & b1) << 1) | ((Data & b0) >> 5);
                }
                else
                {
                        const int brightColour = 0x40;
                        const int flashColour = 0x80;
                        const int flashWhite = 0x20;

                        if ((Data & flashColour) && flashSwap)
                        {
                                borderColour = (Data & flashWhite) ? colourWhite : colourBlack;
                        }
                        else
                        {
                                borderColour = ((Data & g1) << 3) | ((Data & r1) << 2) | ((Data & b1) << 1);
                        }

                        if (Data & brightColour)
                        {
                                borderColour += (borderColour >> 1);
                        }
                }
        }
        else
        {
                borderColour = ((Data & g1) << 3) | ((Data & r1) << 2) | ((Data & b1) << 1);
        }

        return borderColour;
}

void DetermineSpectraDisplayBank()
{
        bool spectrum128Screen1 = (SPECLast7ffd & 0x08);
        bool displayBank1 = (spectrum.spectraMode & spectraDisplayBank);

        if (!spectrum128Screen1)
        {
                spectraDisplayBankOffset = displayBank1 ? 0x4000 : 0x0000;
        }
        else
        {
                spectraDisplayBankOffset = displayBank1 ? 0x0000 : 0x4000;
        }
}


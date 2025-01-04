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

/* Information on the Interface 2 ROM Cartridge mechanism and the ZXC1, ZXC2,
   ZXC3 and ZXC4 ROM cartridges can be found at www.fruitcake.plus.com */

#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <vcl4.h>
#include "zx81config.h"

const int MaxRomCartridgeCapacity = 4 * 1024 * 1024;
BYTE RomCartridgeMemory[MaxRomCartridgeCapacity];
int RomCartridgeCapacity;

const int zxc2Bank = 0x0F;
const int zxc2PageOut = 0x10;
const int zxc2Locked = 0x20;

const int zxc3Bank = 0x07;
const int zxc3Write = 0x08;
const int zxc3PageOut = 0x10;
const int zxc3Locked = 0x20;

int zxcStartAddressRangeFull = 0x0000;
int zxcEndAddressRangeFullWrite = 0x4000;
int zxcEndAddressRangeFullRead = 0x4000;
int zxcBaseAddressRangeWindow = 0x10000;
int zxcStartAddressRangeWindow = 0x10000;
int zxcEndAddressRangeWindow = 0x10000;
int zxcLowerControlLimit = 0x2000;
int zxcUpperControlBase = 0x3FC0;
const int zxcUpperControlMask = 0x003F;
const int zxcLowerControlMask = 0x1FFF;

const int zxc4Bank = 0x0007;
const int zxc4Write = 0x0008;
const int zxc4PageOut = 0x0010;
const int zxc4Locked = 0x0020;
const int zxc4BankSet = 0x07C0;
const int zxc4CassettePaging = 0x800;
const int zxc4Interface1Paging = 0x1000;

const int zxc4BankSetShift = 3;

const BYTE idleDataBus = 0xFF;

static BYTE AccessRomCartridgeSinclair(int Address);
static bool AccessRomCartridgeTimex(int Address, BYTE* Data);
static bool AccessRomCartridgeZXC1(int Address, BYTE* Data);
static bool AccessRomCartridgeZXC2(int Address, BYTE* Data);
static bool AccessRomCartridgeZXC3(int Address, BYTE* Data, bool writeAccess);
static bool AccessRomCartridgeZXC4(int Address, BYTE* Data, bool writeAccess);
static BYTE AccessRomCartridgeBank(int bank, int Address);
static bool AccessRomCartridge(int Address, BYTE* Data, bool writeAccess);
void ConfigureZXC1();
void ResetRomCartridge();

extern bool directMemoryAccess;

void InitialiseRomCartridge()
{
        ConfigureZXC1();
        ResetRomCartridge();

        if (emulator.machine == MACHINEZX80)
        {
                zxcStartAddressRangeFull = 0xC000;
                zxcEndAddressRangeFullWrite = 0xFFFF;
                zxcEndAddressRangeFullRead = 0xFFBF;
                zxcBaseAddressRangeWindow = 0x4000;
                zxcStartAddressRangeWindow = 0x7F80;
                zxcEndAddressRangeWindow = 0x7FBF;
                zxcLowerControlLimit = 0xE000;
                zxcUpperControlBase = 0xFFC0;
        }
        else
        {
                zxcStartAddressRangeFull = 0x0000;
                zxcEndAddressRangeFullWrite = 0x3FFF;
                zxcEndAddressRangeFullRead = (romcartridge.type == ROMCARTRIDGETS1510) ? 0x7FFF + (RomCartridgeCapacity - 0x2000) : 0x3FFF;
                zxcBaseAddressRangeWindow = 0x10000;
                zxcStartAddressRangeWindow = 0x10000;   // Place outside the memory map
                zxcEndAddressRangeWindow = 0x10000;
                zxcLowerControlLimit = 0x2000;
                zxcUpperControlBase = 0x3FC0;
        }
}

void ResetRomCartridge()
{
        romcartridge.zxcPaging = 0;
        romcartridge.zxcInterface1BankPagedIn = false;
        romcartridge.zxcCassetteBankPagedIn = false;
        romcartridge.zxcLowerControlAccessSelected = false;
}

void ConfigureZXC1()
{
        // Paging timings:
        // 16K+Pageout	2.46
        // 32K          1.52
        // 32K+Pageout  1.52, 3.55
        // 48K          1.52, 2.23
        // 48K+Pageout	1.52, 2.23, 4.26
        // 64K          1.62, 2.23, 3.08

        int framesPerSec = machine.NTSC ? 60 : 50;

        switch (romcartridge.zxc1Configuration)
        {
                case ZXC1_32K:                  // 27C256: A14 0->1 (22k/220k)
                        romcartridge.zxc1BankNumber[0] = 0;
                        romcartridge.zxc1BankNumber[1] = 1;
                        romcartridge.zxc1BankNumber[2] = -1;
                        romcartridge.zxc1BankNumber[3] = -1;

                        romcartridge.zxc1BankTimer[0] = (int)(1.52 * framesPerSec);
                        romcartridge.zxc1BankTimer[1] = 0;
                        romcartridge.zxc1BankTimer[2] = 0;
                        break;

                case ZXC1_48K:                  // 27C512: A14 0->1 (22k/220k), A15 0->1 (33k/330k)
                        romcartridge.zxc1BankNumber[0] = 0;
                        romcartridge.zxc1BankNumber[1] = 1;
                        romcartridge.zxc1BankNumber[2] = 3;
                        romcartridge.zxc1BankNumber[3] = -1;

                        romcartridge.zxc1BankTimer[0] = (int)(1.52 * framesPerSec);
                        romcartridge.zxc1BankTimer[1] = (int)(2.23 * framesPerSec);
                        romcartridge.zxc1BankTimer[2] = 0;
                        break;

                case ZXC1_64K:                  // 27C512 A14 1->0 (22k/220k), A15 0->1 (33k/330k), A14 0->1 (47k/470k)
                        romcartridge.zxc1BankNumber[0] = 1;
                        romcartridge.zxc1BankNumber[1] = 0;
                        romcartridge.zxc1BankNumber[2] = 2;
                        romcartridge.zxc1BankNumber[3] = 3;

                        romcartridge.zxc1BankTimer[0] = (int)(1.62 * framesPerSec);
                        romcartridge.zxc1BankTimer[1] = (int)(2.23 * framesPerSec);
                        romcartridge.zxc1BankTimer[2] = (int)(3.08 * framesPerSec);
                        break;

                case ZXC1_16KPAGEOUT:           // 27C128: ROMCS 1->0 (33k/330k)
                        romcartridge.zxc1BankNumber[0] = 0;
                        romcartridge.zxc1BankNumber[1] = -1;
                        romcartridge.zxc1BankNumber[2] = -1;
                        romcartridge.zxc1BankNumber[3] = -1;

                        romcartridge.zxc1BankTimer[0] = (int)(2.46 * framesPerSec);
                        romcartridge.zxc1BankTimer[1] = 0;
                        romcartridge.zxc1BankTimer[2] = 0;
                        break;

                case ZXC1_32KPAGEOUT:           // 27C256: A14 0->1 (22k/220k), ROMCS 1->0 (47k/470k)
                        romcartridge.zxc1BankNumber[0] = 0;
                        romcartridge.zxc1BankNumber[1] = 1;
                        romcartridge.zxc1BankNumber[2] = -1;
                        romcartridge.zxc1BankNumber[3] = -1;

                        romcartridge.zxc1BankTimer[0] = (int)(1.52 * framesPerSec);
                        romcartridge.zxc1BankTimer[1] = (int)(3.55 * framesPerSec);
                        romcartridge.zxc1BankTimer[2] = 0;
                        break;

                case ZXC1_48KPAGEOUT:           // 27C512: A14 0->1 (22k/220k), A15 0->1 (33k/330k), ROMCS 1->0 (56k/560k)
                        romcartridge.zxc1BankNumber[0] = 0;
                        romcartridge.zxc1BankNumber[1] = 1;
                        romcartridge.zxc1BankNumber[2] = 3;
                        romcartridge.zxc1BankNumber[3] = -1;

                        romcartridge.zxc1BankTimer[0] = (int)(1.52 * framesPerSec);
                        romcartridge.zxc1BankTimer[1] = (int)(2.23 * framesPerSec);
                        romcartridge.zxc1BankTimer[2] = (int)(4.26 * framesPerSec);
                        break;
        }

        romcartridge.zxc1PageOut = false;
        romcartridge.zxc1ActiveBank = 0;
}

bool RomCartridgePagedIn()
{
        bool pageIn = false;

        switch (romcartridge.type)
        {
        case ROMCARTRIDGESINCLAIR:
        case ROMCARTRIDGE16K:
        case ROMCARTRIDGETS1510:
                pageIn = true;
                break;

        case ROMCARTRIDGEZXC1:
                pageIn = !romcartridge.zxc1PageOut;
                break;

        case ROMCARTRIDGEZXC2:
                pageIn = ((romcartridge.zxcPaging & zxc2PageOut) == 0);
                break;

        case ROMCARTRIDGEZXC3:
                pageIn = ((romcartridge.zxcPaging & zxc3PageOut) == 0);
                break;

        case ROMCARTRIDGEZXC4:
                pageIn = ((romcartridge.zxcPaging & zxc4PageOut) == 0);
                break;
        }

        return pageIn;
}

bool LoadRomCartridgeFile(char *filename)
{
        memset(RomCartridgeMemory, 0xFF, sizeof(RomCartridgeMemory));
        RomCartridgeCapacity = 0;

        int fptr;

        fptr=open(filename, O_RDONLY | O_BINARY);
        if (fptr<1) return false;

        struct stat st;
        if (stat(filename, &st))
        {
                close(fptr);
                return false;
        }

        if ((RomCartridgeCapacity = read(fptr, RomCartridgeMemory, MaxRomCartridgeCapacity))==-1)
        {
                close(fptr);

                return false;
        }

        close(fptr);

        return true;
}

bool WriteRomCartridge(int Address, BYTE* Data)
{
        const bool writeAccess = true;
        return AccessRomCartridge(Address, Data, writeAccess);
}

bool ReadRomCartridge(int Address, BYTE* Data)
{
        const bool writeAccess = false;
        return AccessRomCartridge(Address, Data, writeAccess);
}

bool AccessRomCartridge(int Address, BYTE* Data, bool writeAccess)
{
        bool readStatus = false;

        bool romBankAccess = ((Address >= zxcStartAddressRangeFull) && (Address <= zxcEndAddressRangeFullWrite) && writeAccess) ||
                             ((Address >= zxcStartAddressRangeFull) && (Address <= zxcEndAddressRangeFullRead) && !writeAccess);
        bool windowRead = (Address >= zxcStartAddressRangeWindow) && (Address <= zxcEndAddressRangeWindow) && !writeAccess;

        if (windowRead)
        {
                Address += (zxcStartAddressRangeFull - zxcBaseAddressRangeWindow);
        }

        if (romBankAccess || windowRead)
        {
                switch (romcartridge.type)
                {
                case ROMCARTRIDGESINCLAIR:
                case ROMCARTRIDGE16K:
                        *Data = AccessRomCartridgeSinclair(Address);
                        readStatus = true;
                        break;

                case ROMCARTRIDGETS1510:
                        readStatus = AccessRomCartridgeTimex(Address, Data);
                        break;

                case ROMCARTRIDGEZXC1:
                        readStatus = AccessRomCartridgeZXC1(Address, Data);
                        break;

                case ROMCARTRIDGEZXC2:
                        readStatus = AccessRomCartridgeZXC2(Address, Data);
                        break;

                case ROMCARTRIDGEZXC3:
                        readStatus = AccessRomCartridgeZXC3(Address, Data, writeAccess);
                        break;

                case ROMCARTRIDGEZXC4:
                        readStatus = AccessRomCartridgeZXC4(Address, Data, writeAccess);
                        break;
                }
        }
        return readStatus;
}

static inline bool AccessRomCartridgeTimex(int Address, BYTE* Data)
{
        bool dataRead = false;

        if (Address < 0x2000 || (Address >= 0x4000 && Address < 0x8000))
        {
                return dataRead;
        }

        const int bank = 0;

        if (Address < 0x4000)
        {
                *Data = AccessRomCartridgeBank(bank, Address - 0x2000);
        }
        else
        {
                *Data = AccessRomCartridgeBank(bank, Address - 0x6000);
        }

        dataRead = true;

        return dataRead;
}

static inline BYTE AccessRomCartridgeSinclair(int Address)
{
        const int bank = 0;
        return AccessRomCartridgeBank(bank, Address);
}

void RomCartridgeZXC1TimerTick()
{
        if ((romcartridge.zxc1ActiveBank >= 0) && (romcartridge.zxc1ActiveBank < 3))
        {
                if (romcartridge.zxc1BankTimer[romcartridge.zxc1ActiveBank] > 0)
                {
                        romcartridge.zxc1BankTimer[romcartridge.zxc1ActiveBank]--;

                        if (romcartridge.zxc1BankTimer[romcartridge.zxc1ActiveBank] == 0)
                        {
                                romcartridge.zxc1ActiveBank++;

                                if (romcartridge.zxc1BankNumber[romcartridge.zxc1ActiveBank] == -1)
                                {
                                        romcartridge.zxc1PageOut = true;
                                }
                        }
                }
        }
}

static inline bool AccessRomCartridgeZXC1(int Address, BYTE* Data)
{
        bool dataRead = true;

        if (romcartridge.zxc1PageOut)
        {
                dataRead = false;
        }
        else
        {
                int bank = romcartridge.zxc1BankNumber[romcartridge.zxc1ActiveBank];
                *Data = AccessRomCartridgeBank(bank, Address);
        }

        return dataRead;
}

static inline bool AccessRomCartridgeZXC2(int Address, BYTE* Data)
{
        bool dataRead = true;

        if (!directMemoryAccess && (Address >= zxcUpperControlBase) && !(romcartridge.zxcPaging & zxc2Locked))
        {
                romcartridge.zxcPaging = (Address & zxcUpperControlMask);
                *Data = idleDataBus;
        }
        else if (romcartridge.zxcPaging & zxc2PageOut)
        {
                dataRead = false;
        }
        else
        {
                int bank = (romcartridge.zxcPaging & zxc2Bank);
                *Data = AccessRomCartridgeBank(bank, Address);
        }

        return dataRead;
}

static inline bool AccessRomCartridgeZXC3(int Address, BYTE* Data, bool writeAccess)
{
        bool dataRead = true;

        if (!directMemoryAccess && (Address >= zxcUpperControlBase) && !(romcartridge.zxcPaging & zxc3Locked) && ((emulator.machine != MACHINEZX80) || writeAccess))
        {
                romcartridge.zxcPaging = (Address & zxcUpperControlMask);
                *Data = idleDataBus;
        }
        else if (romcartridge.zxcPaging & zxc3PageOut)
        {
                dataRead = false;
        }
        else
        {
                if (romcartridge.zxcPaging & zxc3Write)
                {
                        // This will need to change when support for writing to the ROM is implemented
                        *Data = idleDataBus;
                }
                else
                {
                        int bank = (romcartridge.zxcPaging & zxc3Bank);
                        *Data = AccessRomCartridgeBank(bank, Address);
                }
        }
        
        return dataRead;
}

static inline bool AccessRomCartridgeZXC4(int Address, BYTE* Data, bool writeAccess)
{
        bool dataRead = true;
        bool controlAccessLocked = (romcartridge.zxcPaging & zxc4Locked);
        bool upperControlAccess = ((!directMemoryAccess && !controlAccessLocked) || romcartridge.zxcInterface1BankPagedIn || romcartridge.zxcCassetteBankPagedIn) &&
                                   !romcartridge.zxcLowerControlAccessSelected && (Address >= zxcUpperControlBase) && ((emulator.machine != MACHINEZX80) || writeAccess);
        bool lowerControlAccess = ((!directMemoryAccess && !controlAccessLocked) || romcartridge.zxcInterface1BankPagedIn || romcartridge.zxcCassetteBankPagedIn) &&
                                   romcartridge.zxcLowerControlAccessSelected && (Address < zxcLowerControlLimit) && ((emulator.machine != MACHINEZX80) || !(romcartridge.zxcPaging & zxc4PageOut));

        if (upperControlAccess)
        {
                const int selectLowerControlAccess = 0x0018;
                const int controlAccessSelectionMask = 0x0038;
                int paging = (Address & zxcUpperControlMask);
                if ((paging & controlAccessSelectionMask) == selectLowerControlAccess)
                {
                        romcartridge.zxcLowerControlAccessSelected = true;
                }
                else
                {
                        romcartridge.zxcPaging = (romcartridge.zxcPaging & ~zxcUpperControlMask) | paging;
                }
                *Data = idleDataBus;
        }
        else if (lowerControlAccess)
        {
                const int selectUpperControlAccess = 0x0028;
                const int controlAccessSelectionMask = 0x0038;
                int paging = (Address & zxcLowerControlMask);
                if ((paging & controlAccessSelectionMask) == selectUpperControlAccess)
                {
                        romcartridge.zxcLowerControlAccessSelected = false;
                }
                else
                {
                        romcartridge.zxcPaging = paging;
                }
                *Data = idleDataBus;
        }
        else
        {
                if (!(romcartridge.zxcPaging & zxc4PageOut) || romcartridge.zxcInterface1BankPagedIn || romcartridge.zxcCassetteBankPagedIn)
                {
                        if (romcartridge.zxcPaging & zxc4Write)
                        {
                                // This will need to change when support for writing to the ROM is implemented
                                *Data = idleDataBus;
                        }
                        else
                        {
                                int readBank = 0;
                                
                                if ((!romcartridge.zxcInterface1BankPagedIn && !romcartridge.zxcCassetteBankPagedIn) ||
                                    (!(romcartridge.zxcPaging & zxc4Interface1Paging) && romcartridge.zxcInterface1BankPagedIn) ||
                                    (!(romcartridge.zxcPaging & zxc4CassettePaging) && romcartridge.zxcCassetteBankPagedIn))
                                {
                                        readBank = (romcartridge.zxcPaging & zxc4Bank);
                                }
                                else
                                {
                                        // Force a read from bank 2 if the Interface 1 bank is active or bank 3 if the Cassette bank is active
                                        readBank = romcartridge.zxcCassetteBankPagedIn ? 3 : 2;
                                }

                                int bank = (((romcartridge.zxcPaging & zxc4BankSet) >> zxc4BankSetShift) | readBank);
                                *Data = AccessRomCartridgeBank(bank, Address);
                        }
                }
                else
                {
                        dataRead = false;
                }
        }

        // Now that the paging mode has been set, determine whether to page in or out the Interface 1 bank or the Cassette bank

        if (!directMemoryAccess)
        {
                if (!romcartridge.zxcInterface1BankPagedIn && !romcartridge.zxcCassetteBankPagedIn &&
                (romcartridge.zxcPaging & zxc4Interface1Paging) && ((Address == 0x0008) || (Address == 0x1708)))
                {
                        romcartridge.zxcInterface1BankPagedIn = true;
                }
                else if (romcartridge.zxcInterface1BankPagedIn && (romcartridge.zxcPaging & zxc4Interface1Paging) && (Address == 0x0700))
                {
                        romcartridge.zxcInterface1BankPagedIn = false;
                }

                if (!romcartridge.zxcInterface1BankPagedIn && !romcartridge.zxcCassetteBankPagedIn &&
                (romcartridge.zxcPaging & zxc4CassettePaging) && ((Address == 0x0556) || (Address == 0x04C2)))
                {
                        romcartridge.zxcCassetteBankPagedIn = true;
                }
                else if (romcartridge.zxcCassetteBankPagedIn && (romcartridge.zxcPaging & zxc4CassettePaging) &&
                        ((Address == 0x0556) || (Address == 0x04C2) || (Address == 0x0555)))
                {
                        romcartridge.zxcCassetteBankPagedIn = false;
                }
        }

        return dataRead;
}

static inline BYTE AccessRomCartridgeBank(int bank, int Address)
{
        return RomCartridgeMemory[(bank << 14) + (Address - zxcStartAddressRangeFull)];
}

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

/* Information on the Interface 2 ROM Cartridge mechanism and the ZXC2,
   ZXC3 and ZXC4 ROM cartridges can be found at www.fruitcake.plus.com */
    
#include <io.h>
#include <fcntl.h>
#include <vcl.h>
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

BYTE AccessRomCartridgeSinclair(int Address);
bool AccessRomCartridgeZXC2(int Address, BYTE* Data);
bool AccessRomCartridgeZXC3(int Address, BYTE* Data, bool writeAccess);
bool AccessRomCartridgeZXC4(int Address, BYTE* Data, bool writeAccess);
BYTE AccessRomCartridgeBank(int bank, int Address);
bool AccessRomCartridge(int Address, BYTE* Data, bool writeAccess);

extern bool directMemoryAccess;

void InitialiseRomCartridge()
{
        zx81.zxcPaging = 0;
        zx81.zxcInterface1BankPagedIn = false;
        zx81.zxcCassetteBankPagedIn = false;
        zx81.zxcLowerControlAccessSelected = false;

        if (zx81.machine == MACHINEZX80)
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
                zxcEndAddressRangeFullRead = 0x3FFF;
                zxcBaseAddressRangeWindow = 0x10000;
                zxcStartAddressRangeWindow = 0x10000;   // Place outside the memory map
                zxcEndAddressRangeWindow = 0x10000;
                zxcLowerControlLimit = 0x2000;
                zxcUpperControlBase = 0x3FC0;
        }
}

bool LoadRomCartridgeFile(char *filename)
{
        memset(RomCartridgeMemory, 0xFF, sizeof(RomCartridgeMemory));
        RomCartridgeCapacity = 0;

        int fptr;
        char file[256];
        int len;

        if (strchr(filename, '\\') || strchr(filename, '/'))
        {
                strcpy(file, filename);
        }
        else
        {
                strcpy(file, zx81.cwd);
                strcat(file,"ROM\\");
                strcat(file,filename);
        }

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<1) return false;

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
                switch (zx81.romCartridge)
                {
                case ROMCARTRIDGESINCLAIR:
                        *Data = AccessRomCartridgeSinclair(Address);
                        readStatus = true;
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

static inline BYTE AccessRomCartridgeSinclair(int Address)
{
        const int bank = 0;
        return AccessRomCartridgeBank(bank, Address);
}

static inline bool AccessRomCartridgeZXC2(int Address, BYTE* Data)
{
        bool dataRead = true;

        if (!directMemoryAccess && (Address >= zxcUpperControlBase) && !(zx81.zxcPaging & zxc2Locked))
        {
                zx81.zxcPaging = (Address & zxcUpperControlMask);
                *Data = idleDataBus;
        }
        else if (zx81.zxcPaging & zxc2PageOut)
        {
                dataRead = false;
        }
        else
        {
                int bank = (zx81.zxcPaging & zxc2Bank);
                *Data = AccessRomCartridgeBank(bank, Address);
        }

        return dataRead;
}

static inline bool AccessRomCartridgeZXC3(int Address, BYTE* Data, bool writeAccess)
{
        bool dataRead = true;

        if (!directMemoryAccess && (Address >= zxcUpperControlBase) && !(zx81.zxcPaging & zxc3Locked) && ((zx81.machine != MACHINEZX80) || writeAccess))
        {
                zx81.zxcPaging = (Address & zxcUpperControlMask);
                *Data = idleDataBus;
        }
        else if (zx81.zxcPaging & zxc3PageOut)
        {
                dataRead = false;
        }
        else
        {
                if (zx81.zxcPaging & zxc3Write)
                {
                        // This will need to change when support for writing to the ROM is implemented
                        *Data = idleDataBus;
                }
                else
                {
                        int bank = (zx81.zxcPaging & zxc3Bank);
                        *Data = AccessRomCartridgeBank(bank, Address);
                }
        }
        
        return dataRead;
}

static inline bool AccessRomCartridgeZXC4(int Address, BYTE* Data, bool writeAccess)
{
        bool dataRead = true;
        bool controlAccessLocked = (zx81.zxcPaging & zxc4Locked);
        bool upperControlAccess = ((!directMemoryAccess && !controlAccessLocked) || zx81.zxcInterface1BankPagedIn || zx81.zxcCassetteBankPagedIn) &&
                                   !zx81.zxcLowerControlAccessSelected && (Address >= zxcUpperControlBase) && ((zx81.machine != MACHINEZX80) || writeAccess);
        bool lowerControlAccess = ((!directMemoryAccess && !controlAccessLocked) || zx81.zxcInterface1BankPagedIn || zx81.zxcCassetteBankPagedIn) &&
                                   zx81.zxcLowerControlAccessSelected && (Address < zxcLowerControlLimit) && ((zx81.machine != MACHINEZX80) || !(zx81.zxcPaging & zxc4PageOut));

        if (upperControlAccess)
        {
                const int selectLowerControlAccess = 0x0018;
                const int controlAccessSelectionMask = 0x0038;
                int paging = (Address & zxcUpperControlMask);
                if ((paging & controlAccessSelectionMask) == selectLowerControlAccess)
                {
                        zx81.zxcLowerControlAccessSelected = true;
                }
                else
                {
                        zx81.zxcPaging = (zx81.zxcPaging & ~zxcUpperControlMask) | paging;
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
                        zx81.zxcLowerControlAccessSelected = false;
                }
                else
                {
                        zx81.zxcPaging = paging;
                }
                *Data = idleDataBus;
        }
        else
        {
                if (!(zx81.zxcPaging & zxc4PageOut) || zx81.zxcInterface1BankPagedIn || zx81.zxcCassetteBankPagedIn)
                {
                        if (zx81.zxcPaging & zxc4Write)
                        {
                                // This will need to change when support for writing to the ROM is implemented
                                *Data = idleDataBus;
                        }
                        else
                        {
                                int readBank = 0;
                                
                                if ((!zx81.zxcInterface1BankPagedIn && !zx81.zxcCassetteBankPagedIn) ||
                                    (!(zx81.zxcPaging & zxc4Interface1Paging) && zx81.zxcInterface1BankPagedIn) ||
                                    (!(zx81.zxcPaging & zxc4CassettePaging) && zx81.zxcCassetteBankPagedIn))
                                {
                                        readBank = (zx81.zxcPaging & zxc4Bank);
                                }
                                else
                                {
                                        // Force a read from bank 2 if the Interface 1 bank is active or bank 3 if the Cassette bank is active
                                        readBank = zx81.zxcCassetteBankPagedIn ? 3 : 2;
                                }

                                int bank = (((zx81.zxcPaging & zxc4BankSet) >> zxc4BankSetShift) | readBank);
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
                if (!zx81.zxcInterface1BankPagedIn && !zx81.zxcCassetteBankPagedIn &&
                (zx81.zxcPaging & zxc4Interface1Paging) && ((Address == 0x0008) || (Address == 0x1708)))
                {
                        zx81.zxcInterface1BankPagedIn = true;
                }
                else if (zx81.zxcInterface1BankPagedIn && (zx81.zxcPaging & zxc4Interface1Paging) && (Address == 0x0700))
                {
                        zx81.zxcInterface1BankPagedIn = false;
                }

                if (!zx81.zxcInterface1BankPagedIn && !zx81.zxcCassetteBankPagedIn &&
                (zx81.zxcPaging & zxc4CassettePaging) && ((Address == 0x0556) || (Address == 0x04C2)))
                {
                        zx81.zxcCassetteBankPagedIn = true;
                }
                else if (zx81.zxcCassetteBankPagedIn && (zx81.zxcPaging & zxc4CassettePaging) &&
                        ((Address == 0x0556) || (Address == 0x04C2) || (Address == 0x0555)))
                {
                        zx81.zxcCassetteBankPagedIn = false;
                }
        }

        return dataRead;
}

static inline BYTE AccessRomCartridgeBank(int bank, int Address)
{
        return RomCartridgeMemory[(bank << 14) + (Address - zxcStartAddressRangeFull)];
}
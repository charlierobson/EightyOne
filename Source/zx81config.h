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
 * zx81config.h
 *
 */


#ifndef ZX81CONFIG_H
#define ZX81CONFIG_H

#define CFGBYTE char
#define BYTE unsigned char

#define EMUID           0x85

#define SYNCTYPEH       1
#define SYNCTYPEV       2

#define BORDERNONE      0
#define BORDERSMALL     1
#define BORDERNORMAL    2
#define BORDERLARGE     3
#define BORDERFULL      4

#define MACHINEZX80     0
#define MACHINEZX81     1
#define MACHINEACE      2
#define MACHINETS1500   3
#define MACHINELAMBDA   4
#define MACHINEZX97LE   5
#define MACHINESPEC48   6
#define MACHINEQL       7
#define MACHINER470     8
#define MACHINETK85     9
#define MACHINETS1000   10

#define SPECCY16        0
#define SPECCY48        1
#define SPECCYPLUS      3
#define SPECCYTC2048    4
#define SPECCYTS2068    5
#define SPECCY128       6
#define SPECCYPLUS2     7
#define SPECCYPLUS2A    8
#define SPECCYPLUS3     9
#define SPECCYSE        10

#define SPECKBISS2      0
#define SPECKBISS3      1

#define HIRESDISABLED   0
#define HIRESWRX        1
#define HIRESG007       2
#define HIRESMEMOTECH   3

#define CHRGENSINCLAIR  0
#define CHRGENDK        1
#define CHRGENQS        2
#define CHRGENCHR128     3
#define CHRGENLAMBDA    4

#define COLOURDISABLED  0
#define COLOURSINCLAIR  0
#define COLOURLAMBDA    1
#define COLOURCHROMA    2
#define COLOURPRISM     3
#define COLOURDDC       4
#define COLOURHAVEN     5
#define COLOURACE       6
#define COLOURSPECTRA   7

#define ROMCARTRIDGENONE     0
#define ROMCARTRIDGESINCLAIR 1
#define ROMCARTRIDGEZXC4     2
#define ROMCARTRIDGEZXC3     3
#define ROMCARTRIDGEZXC2     4
#define ROMCARTRIDGEZXC1     5

#define CRCACE		0x0a09
#define CRCASZMIC	0xcac9
#define CRCASZMIC2	0x6fe5
#define CRCH4TH		0xa5cd
#define CRCSG81		0x72f4
#define CRCSP81		0x877d
#define CRCTK85		0x28a9
#define CRCTREE4TH	0x9dc7
#define CRCTS1500	0x63b7
#define CRCZX80		0x3a68
#define CRCZX97LE	0x68bb
#define CRCZX81_ED1	0x29E8
#define CRCZX81_ED2	0x2914
#define CRCLAMBDA	0x4d3c
#define CRC8300		0x0d4e
#define CRCR470         0x5413
#define CRCSP48         0xACE0

#define FLOPPYNONE      0
#define FLOPPYPLUS3     1
#define FLOPPYPLUSD     2
#define FLOPPYDISCIPLE  3
#define FLOPPYOPUSD     4
#define FLOPPYBETA      5
#define FLOPPYIF1       6
#define FLOPPYZX1541    7
#define FLOPPYLARKEN81  8

#define DRIVENONE       0
#define DRIVE3INCHSS    1
#define DRIVE3INCHDS    2
#define DRIVE35INCHDS   3

#define HDNONE          0
#define HDPLUS3E        1
#define HDDIVIDE        2
#define HDZXCF          3
#define HDACECF         4
#define HDPITERSCF      5
#define HDPITERS8B      6
#define HDPITERS16B     7

#define MFNONE          0
#define MF128           1
#define MFPLUS3         2

typedef enum
{
        ZXC1_32K,
        ZXC1_48K,
        ZXC1_64K,
        ZXC1_16KPAGEOUT,
        ZXC1_32KPAGEOUT,
        ZXC1_48KPAGEOUT,
        ZXC1_64KPAGEOUT
} ZXC1TYPE;

typedef struct
{
        CFGBYTE emuid, major,minor,testver;
        CFGBYTE dirtydisplay;
        CFGBYTE machine;
        CFGBYTE extfont;
        CFGBYTE shadowROM;
        CFGBYTE RAM816k;
        CFGBYTE protectROM;
        CFGBYTE truehires;
        CFGBYTE NTSC;
        CFGBYTE inverse;
        CFGBYTE aysound;
        CFGBYTE aytype;
        CFGBYTE single_step;
        CFGBYTE vsyncsound;
        CFGBYTE beepersound;
        CFGBYTE ts2050;
        CFGBYTE ace96k;
        CFGBYTE TZXin, TZXout;
        CFGBYTE audioout,audioin;
        CFGBYTE colour;
        CFGBYTE debug1, debug2;
        CFGBYTE autoload;
        CFGBYTE wobble;
        CFGBYTE chrgen, enableQSchrgen;
        CFGBYTE bordersize;
        CFGBYTE simpleghost;
        int maxireg;
        CFGBYTE zxprinter;
        CFGBYTE zxpand;
        CFGBYTE romCartridge;
        int zxcPaging;
        int zxc1PageOut;
        int zxc1ActiveBank;
        ZXC1TYPE zxc1Configuration;
        int zxc1BankNumber[4];
        int zxc1BankTimer[3];
        int zxcLowerControlAccessSelected;
        int zxcInterface1BankPagedIn;
        int zxcCassetteBankPagedIn;
        int RAMTOP;
        int ROMTOP;
        int m1not;
        //int romtype;
        int romcrc;
        int frameskip;
        int speedup;
        int UseRShift;
        CFGBYTE chromaMode;
        CFGBYTE spectraMode;
        CFGBYTE spectraColourSwitchOn;
        CFGBYTE chromaColourSwitchOn;
        CFGBYTE FloatingPointHardwareFix;
        CFGBYTE HideHardwareHSyncs;

        char ROM80[256];
        char ROM81[256];
        char ROMACE[256];
        char ROMTS1000[256];
        char ROMTS1500[256];
        char ROMLAMBDA[256];
        char ROMPC8300[256];
        char ROMTK85[256];
        char ROM97LE[256];
        char ROMR470[256];
        char ROMSP48[256];
        char ROMSP128[256];
        char ROMSPP2[256];
        char ROMSPP3[256];
        char ROMSPP3E[256];
        char ROMSPP3ECF[256];
        char ROMTC2048[256];
        char ROMTS2068[256];
        char ROMSPSE[256];
        char ROMDock[256];
        char ROMZXCF[256];
        char ROMZX8BIT[256];
        char ROMZX16BIT[256];
        char ROMQL[256];
        char ROMPLUSD[256];
        char ROMDISCIPLE[256];
        char ROMOPUSD[256];
        char ROMBETADISC[256];
        char ROMUSPEECH[256];
        char ROMCARTRIDGE[4 * 1024 * 1024];
        char cwd[256];
        char temppath[256];
        char inipath[256];
        char configpath[256];
        char mydocs[256];
        char machinename[256];
} ZX81;

typedef struct
{
        int machine;
        int RAMBanks;
        int ROMBanks;
        int uspeech;
        int kbissue;
        int kmouse;
        int floppytype, autoboot;
        int driveatype, drivebtype;
        char driveaimg[256], drivebimg[256];
        int drivebusy;
        int HDType;
        int WriteProtectJumper;
        int UploadJumper;
        int ZXCFRAMSize;
        int MFActive;
        int MFLockout;
        int MFVersion;
        int divIDEVersion;
} SPECTRUM;


typedef struct
{
        int bankswitch;
        int protect08;
        int protectab;
        int protectb0;
        int protectb115;
        int saveram;
        unsigned char bankmem[16*16384];
} ZX97;

typedef struct
{
        int AdvancedEffects;
        int DotCrawl;
        int Interlaced;
        int DisableAdvanced;
} TV;

typedef struct
{
        int sync_len, sync_valid;
        int scanline_len;
        BYTE scanline[4000];
} SCANLINE;


typedef struct
{
        void (*initialise)(void);
        int (*do_scanline)(SCANLINE *line);
        void (*writebyte)(int Address, int Data);
        void (*setbyte)(int Address, int Data);
        BYTE (*readbyte)(int Address);
        BYTE (*readoperandbyte)(int Address);
        BYTE (*getbyte)(int Address);
        BYTE (*opcode_fetch)(int Address);
        void (*writeport)(int Address, int Data, int *tstates);
        BYTE (*readport)(int Address, int *tstates);
        int (*contendmem)(int Address, int states, int time);
        int (*contendio)(int Address, int states, int time);
        void (*reset)(void);
        void (*nmi)(void);
        void (*exit)(void);

        int clockspeed;
        int tperscanline;
        int tperframe;
        int intposition;
        int scanlines;
        char CurRom[256];

        void* cset;

} MACHINE;

typedef struct
{
        int x,y, lastx, lasty;
        int buttons;
} MOUSE;

extern ZX81 zx81;
extern ZX97 zx97;
extern SPECTRUM spectrum;
extern TV tv;
extern MACHINE machine;
extern MOUSE mouse;
extern void load_config();

#define readbyte_internal(Addr) (machine.opcode_fetch(Addr))
#define readbyte(Addr) (machine.readbyte(Addr))
#define readoperandbyte(Addr) (machine.readoperandbyte(Addr))
#define getbyte(Addr) (machine.getbyte(Addr))
#define writebyte(Addr,Data) (machine.writebyte(Addr,Data))
#define setbyte(Addr,Data) (machine.setbyte(Addr,Data))
#define writeport(Addr,Data,tstates) (machine.writeport(Addr,Data,tstates))
#define readport(Addr, tstates) (machine.readport(Addr, tstates))
#define opcode_fetch(Addr) (machine.opcode_fetch(Addr))

#endif


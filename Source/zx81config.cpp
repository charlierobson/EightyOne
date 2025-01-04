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
 * zx81config.c
 *
 */

#include <string.h>
#include <dir.h>
#include <vcl4.h>

#include "zx81config.h"

ZX81 zx81;
ZX97 zx97;
SPECTRUM spectrum;
TV tv;
MACHINE machine;
MOUSE mouse;
ROMCARTRIDGE romcartridge;
EMULATOR emulator;

int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
int lastMemoryReadValueLo, lastMemoryWriteValueLo;
int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
int lastMemoryReadValueHi, lastMemoryWriteValueHi;
bool directMemoryAccess;

const _TCHAR* iniFolder                 = _TEXT("EightyOne\\");
const _TCHAR* temporaryFolder           = _TEXT("EightyOne\\");

const _TCHAR* nvMemoryFolder            = _TEXT("NV_Memory\\");
const _TCHAR* examplesDrivesFolder      = _TEXT("Examples\\Drives\\");
const _TCHAR* documentationFolder       = _TEXT("Documentation\\");
const _TCHAR* exampleZX81ProgramsFolder = _TEXT("Examples\\ZX81 Programs\\");

const _TCHAR* romsFolder                = _TEXT("ROMs\\");
const _TCHAR* fdcRomsFolder             = _TEXT("ROMs\\FDCs\\");
const _TCHAR* ideRomsFolder             = _TEXT("ROMs\\IDEs\\");
const _TCHAR* interfaceRomsFolder       = _TEXT("ROMs\\Interfaces\\");
const _TCHAR* graphicRomsFolder         = _TEXT("ROMs\\Graphics\\");
const _TCHAR* replacementRomsFolder     = _TEXT("ROMs\\Replacement ROMs\\");

const _TCHAR* romCartridgeFolder        = _TEXT("ROM Cartridges\\");
const _TCHAR* if2RomsFolder             = _TEXT("ROM Cartridges\\ZX Interface 2\\");
const _TCHAR* ts1510RomsFolder          = _TEXT("ROM Cartridges\\TS1510\\");
const _TCHAR* ts2068RomsFolder          = _TEXT("ROM Cartridges\\TS2068\\");
const _TCHAR* tc2068RomsFolder          = _TEXT("ROM Cartridges\\TC2068\\");

extern bool GetVersionNumber(int& versionNumberMajor, int& versionNumberMinor, int& versionNumberPart3, int& versionNumberPart4);

void LoadMachineRoms();

void load_config(void)
{
        int versionNumberMajor;
        int versionNumberMinor;
        int versionNumberPart3;
        int versionNumberPart4;
        GetVersionNumber(versionNumberMajor, versionNumberMinor, versionNumberPart3, versionNumberPart4);
        
        emulator.emuid=EMUID;
        emulator.major=(CFGBYTE)versionNumberMajor;
        emulator.minor=(CFGBYTE)versionNumberMinor;
        emulator.testver=(CFGBYTE)versionNumberPart3;

        emulator.machine=MACHINEZX81;
        
        zx81.shadowROM=0;
        zx81.RAM816k=0;
        zx81.RAM816kWriteProtected=0;
        machine.protectROM=1;
        zx81.RAMTOP=32767;
        zx81.ROMTOP=8191;
        zx81.truehires=HIRESWRX;
        emulator.inverse=0;
        zx81.extfont=0;
        emulator.frameskip=0;
        machine.aysound=0;
        machine.aytype=0;
        machine.speech=0;
        zx81.vsyncsound=0;
        emulator.beepersound=0;
        machine.ts2050=0;
        machine.plus3arabicPagedOut=0;
        emulator.TZXin=0;
        emulator.TZXout=0;
        emulator.audioout=0;
        emulator.audioin=0;
        emulator.romcrc=-1;
        emulator.autoload=0;
        zx81.chrgen=CHRGENSINCLAIR;
        zx81.enableQSchrgen=0;
        emulator.simpleghost=1;
        zx81.maxireg=0x1F;
        zx81.zxpand=0;
        emulator.speedup=0;
        emulator.UseRShift=0;
        zx81.chromaMode=0x0F;
        zx81.chromaColourSwitchOn = 0;
        zx81.FloatingPointHardwareFix = 0;
        machine.NTSC=0;
        machine.clockspeed=3250000;
        machine.tperscanline=207;
        machine.tperframe=312*207;
        machine.fps=50;
        spectrum.intposition=0;
        machine.zxprinter=1;

        emulator.ColouriseHorizontalSyncPulse = false;
        emulator.ColouriseVerticalSyncPulse = false;
        emulator.ColouriseRomDisplayDriver = false;
        emulator.ColouriseBackPorch = false;
        emulator.ColouriseNonMaskableInterruptResponse = false;
        emulator.ColouriseNonMaskableInterruptResponseWaitStates = false;
        emulator.ColouriseNonMaskableInterruptServiceRoutine = false;
        emulator.ColouriseNonMaskableInterruptServiceRoutineRecursion = false;
        emulator.ColouriseMaskableInterruptResponse = false;
        emulator.ColouriseMaskableInterruptServiceRoutine = false;
        emulator.ColouriseInstructionStraddlingNMI = false;
        emulator.ColouriseInstructionStraddlingNMIWaitStates = false;
        emulator.ColouriseZ80Halted = false;

        machine.colour=COLOURDISABLED;

        romcartridge.type = ROMCARTRIDGENONE;
        romcartridge.zxcPaging = 0;
        romcartridge.zxc1PageOut = false;
        romcartridge.zxc1ActiveBank = 0;
        romcartridge.zxc1Configuration = ZXC1_32K;
        romcartridge.zxcLowerControlAccessSelected = 0;
        romcartridge.zxcInterface1BankPagedIn = 0;
        romcartridge.zxcCassetteBankPagedIn = 0;

        emulator.bordersize=BORDERNORMAL;

        emulator.single_step=0;

        LoadMachineRoms();

        ZXString simplePlus3Rom = ZXString(emulator.cwd) + ideRomsFolder + ZXString("simple+3e8bit(sm8en3eE).rom");
        _tcscpy(emulator.ROMSPP3E, simplePlus3Rom.c_str());
        ZXString zxcflbaRom = ZXString(emulator.cwd) + ideRomsFolder + ZXString("zxcflba.rom");
        _tcscpy(emulator.ROMZXCF, zxcflbaRom.c_str());
        ZXString zx8blbsRom = ZXString(emulator.cwd) + ideRomsFolder + ZXString("zx8blbs.rom");
        _tcscpy(emulator.ROMZX8BIT, zx8blbsRom.c_str());
        ZXString zxidelbsRom = ZXString(emulator.cwd) + ideRomsFolder + ZXString("zxidelbs.rom");
        _tcscpy(emulator.ROMZX16BIT, zxidelbsRom.c_str());

        ZXString plusdRom = fdcRomsFolder + ZXString("plusd.rom");
        _tcscpy(emulator.ROMPLUSD, plusdRom.c_str());
        ZXString discipleRom = fdcRomsFolder + ZXString("disciple.rom");
        _tcscpy(emulator.ROMDISCIPLE, discipleRom.c_str());
        ZXString opusdiscoveryRom = fdcRomsFolder + ZXString("opusdiscovery.rom");
        _tcscpy(emulator.ROMOPUSD, opusdiscoveryRom.c_str());
        ZXString trdosRom = fdcRomsFolder + ZXString("trdos.rom");
        _tcscpy(emulator.ROMBETADISC, trdosRom.c_str());
        ZXString mwcfideRom = fdcRomsFolder + ZXString("mwcfide.rom");
        _tcscpy(emulator.ROMMWCFIDE, mwcfideRom.c_str());
        ZXString larken81Rom = fdcRomsFolder + ZXString("larken81.rom");
        _tcscpy(emulator.ROMLARKEN81, larken81Rom.c_str());
        ZXString interface1Ed1Rom = fdcRomsFolder + ZXString("interface1.edition1.rom");
        _tcscpy(emulator.ROMINTERFACE1ED1, interface1Ed1Rom.c_str());
        ZXString interface1Ed2Rom = fdcRomsFolder + ZXString("interface1.edition2.rom");
        _tcscpy(emulator.ROMINTERFACE1ED2, interface1Ed2Rom.c_str());

        ZXString uSpeechRom = interfaceRomsFolder + ZXString("uspeech.rom");
        _tcscpy(emulator.ROMUSPEECH, uSpeechRom.c_str());
        ZXString uSourceRom = interfaceRomsFolder + ZXString("usource.rom");
        _tcscpy(emulator.ROMUSOURCE, uSourceRom.c_str());
        ZXString multiface128Rom = interfaceRomsFolder + ZXString("multiface128.rom");
        _tcscpy(emulator.ROMMULTIFACE128, multiface128Rom.c_str());
        ZXString multiface3Rom = interfaceRomsFolder + ZXString("multiface3.rom");
        _tcscpy(emulator.ROMMULTIFACE3, multiface3Rom.c_str());

        ZXString dktronicsRom = graphicRomsFolder + ZXString("dktronicsgraphics.rom");
        _tcscpy(emulator.ROMDKTRONICS, dktronicsRom.c_str());
        ZXString kaydeRom = graphicRomsFolder + ZXString("kaydegraphics.rom");
        _tcscpy(emulator.ROMKAYDE, kaydeRom.c_str());
        ZXString g007Rom = graphicRomsFolder + ZXString("g007hrg.rom");
        _tcscpy(emulator.ROMG007, g007Rom.c_str());
        ZXString memotechRom = graphicRomsFolder + ZXString("memotechhrg.rom");
        _tcscpy(emulator.ROMMEMOTECH, memotechRom.c_str());
        ZXString QuicksilvaHiResRom = graphicRomsFolder + ZXString("quicksilvahires.rom");
        _tcscpy(emulator.ROMQUICKSILVAHIRES, QuicksilvaHiResRom.c_str());

        _tcscpy(emulator.machinename, _TEXT("EightyOne"));
        *(emulator.romcartridgefilepath) = '\0';

        zx97.bankswitch=0;
        zx97.protect08=1;
        zx97.protectab=0;
        zx97.protectb0=1;
        zx97.protectb115=0;
        zx97.saveram=0;

        tv.AdvancedEffects=0;
        tv.DotCrawl=0;
        tv.Interlaced=0;
        tv.DisableAdvanced=0;

        spectrum.usource=0;
        spectrum.kbissue=SPECKBISS3;
        spectrum.driveatype=DRIVE3INCHSS;
        spectrum.drivebtype=DRIVE3INCHSS;
        spectrum.driveaimg[0]='\0';
        spectrum.drivebimg[0]='\0';
        spectrum.drivebusy=-1;
        spectrum.kmouse=0;
        spectrum.HDType=HDNONE;
        spectrum.divIDEJumperEClosed=0;
        spectrum.UploadJumperZXCF=0;
        spectrum.MFVersion=MFNONE;
        spectrum.spectraMode=0x00;
        spectrum.spectraColourSwitchOn = 0;

        mouse.x=0;
        mouse.y=0;
        mouse.buttons=0;
        mouse.lastx=0;
        mouse.lasty=0;
}

void LoadMachineRoms()
{
        _tcscpy(emulator.ROM80, _TEXT("zx80.rom"));
        _tcscpy(emulator.ROM81, _TEXT("zx81.edition3.rom"));
        _tcscpy(emulator.ROMSP16, _TEXT("spectrum48.rom"));
        _tcscpy(emulator.ROMSP48, _TEXT("spectrum48.rom"));
        _tcscpy(emulator.ROMSPP, _TEXT("spectrum48.rom"));
        _tcscpy(emulator.ROMSP128, _TEXT("spectrum128.rom"));
        _tcscpy(emulator.ROMQL, _TEXT("ql.js.rom"));

        _tcscpy(emulator.ROMSPP2, _TEXT("spectrum+2.rom"));
        _tcscpy(emulator.ROMSPP2A, _TEXT("spectrum+3.version4-1.rom"));
        _tcscpy(emulator.ROMSPP3, _TEXT("spectrum+3.version4-1.rom"));

        _tcscpy(emulator.ROMTS1000, _TEXT("zx81.edition3.rom"));
        _tcscpy(emulator.ROMTS1500, _TEXT("ts1500.rom"));
        _tcscpy(emulator.ROMTC2048, _TEXT("tc2048.rom"));
        _tcscpy(emulator.ROMTC2068, _TEXT("ts2068.rom"));
        _tcscpy(emulator.ROMTS2068,_TEXT( "ts2068.rom"));

        _tcscpy(emulator.ROMLAMBDA, _TEXT("lambda8300colour.rom"));
        _tcscpy(emulator.ROMR470, _TEXT("ringo470.rom"));
        _tcscpy(emulator.ROMTK85, _TEXT("tk85.rom"));
        _tcscpy(emulator.ROMACE, _TEXT("jupiterace.rom"));

        _tcscpy(emulator.ROM97LE, _TEXT("zx97.rom"));
}

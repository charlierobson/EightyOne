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
 * snap.c
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <vcl.h>
#include <fcntl.h>
#include <io.h>
#include <mem.h>
#include <sys\stat.h>
#include <ctype.h>
#include <string.h>

#include "snap.h"
#include "zx81.h"
#include "zx81config.h"
#include "z80\z80.h"
#include "chroma\chroma.h"
#include "HW_.h"
#include "main_.h"
#include "Artifacts_.h"

extern int lineCounter;
extern int MemotechMode;
extern int QuicksilvaHiResMode;
extern BYTE font[1024];

int save_snap_zx81(_TCHAR* filename);
int save_snap_ace(_TCHAR* filename);
void load_snap_cpu(FILE *f);
void load_snap_mem(FILE *f);
void load_snap_zx81(FILE *f);
void load_snap_machine(FILE *f);
void load_snap_sound(FILE *f);
void load_snap_speech(FILE *f);
void load_snap_chrgen(FILE *f);
void load_snap_hires(FILE *f);
void load_snap_colour(FILE *f);
void load_snap_romcartridge(FILE *f);
void load_snap_interfaces(FILE *f);
void load_snap_advanced(FILE* f);
void load_snap_drives(FILE* f);
void ProcessTag(_TCHAR* tok, FILE* f);
void InitialiseHardware();

extern void HWSetMachine(int machine, int speccy);
extern void DebugUpdate();

ZXString ReplaceSpaces(ZXString text)
{
        return StringReplace(text.c_str(), " ", "_", TReplaceFlags() << rfReplaceAll);
}

_TCHAR *get_token(FILE *f)
{
        static _TCHAR buffer[256];
        int buflen;
        char c;

        c=fgetc(f);
        while(isspace(c) && !feof(f)) c=fgetc(f);

        buflen=0;
        buffer[buflen++]=c;

        c=fgetc(f);
        while(!isspace(c) && !feof(f) && buflen<255)
        {
                buffer[buflen++]=c;
                c=fgetc(f);
        }

        buffer[buflen]='\0';

        return(buffer);
}

int hex2dec(_TCHAR *str)
{
        int num;

        num=0;
        while(*str)
        {
                num=num*16;
                if (*str>='0' && *str<='9') num += *str - '0';
                else if (*str>='a' && *str<='f') num += *str +10 - 'a';
                else if (*str>='A' && *str<='F') num += *str +10 - 'A';
                else return(num);
                str++;
        }
        return(num);
}

void SetComboBox(TComboBox* comboBox, _TCHAR* text)
{
        ZXString newText = StringReplace(text, "_", " ", TReplaceFlags() << rfReplaceAll);

        for (int i = 0; i < comboBox->Items->Count; i++)
        {
                if (comboBox->Items->Strings[i] == newText)
                {
                        comboBox->ItemIndex = i;
                        break;
                }
        }
}

void load_snap_cpu(FILE *f)
{
        _TCHAR *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("PC"))) z80.pc.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("SP"))) z80.sp.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("HL"))) z80.hl.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("DE"))) z80.de.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("BC"))) z80.bc.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("AF"))) z80.af.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("HL_"))) z80.hl_.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("DE_"))) z80.de_.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("BC_"))) z80.bc_.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("AF_"))) z80.af_.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("IX"))) z80.ix.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("IY"))) z80.iy.w = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("IM"))) z80.im = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("IF1"))) z80.iff1 = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("IF2"))) z80.iff2 = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("HT"))) z80.halted = hex2dec(get_token(f));
                if (!_tcscmp(tok,_TEXT("IR")))
                {
                        int a;

                        a=hex2dec(get_token(f));

                        z80.i = (a>>8) & 255;
                        z80.r = a & 255;
                        z80.r7 = a & 128;
                }
        }
}

void load_snap_sound(FILE *f)
{
        while(!feof(f))
        {
                _TCHAR* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("TYPE")))
                {
                        tok = get_token(f);
                        SetComboBox(HW->SoundCardBox, tok);
                }
        }
}

void load_snap_speech(FILE *f)
{
        while(!feof(f))
        {
                _TCHAR* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("TYPE")))
                {
                        tok = get_token(f);
                        SetComboBox(HW->SpeechBox, tok);
                }
        }
}

void load_snap_chrgen(FILE *f)
{
        int Addr, Count, Chr;

        while(!feof(f))
        {
                _TCHAR* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("TYPE")))
                {
                        tok = get_token(f);
                        SetComboBox(HW->ChrGenBox, tok);

                        if (HW->ChrGenBox->Text == "Quicksilva")
                        {
                                Addr = 0x8400;
                                Form1->QSChrEnable->Visible = true;
                                Form1->QSChrEnable->Enabled = true;
                        }
                }
                else if ((HW->ChrGenBox->Text == "Quicksilva") && !_tcscmp(tok,_TEXT("ENABLED")))
                {
                        zx81.enableQSchrgen = hex2dec(get_token(f));
                        Form1->QSChrEnable->Checked = zx81.enableQSchrgen;
                }
                else if ((HW->ChrGenBox->Text == "Quicksilva") && (*tok=='*'))
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));

                        while(Count--)
                        {
                                font[Addr-0x8400] = Chr;

                                if (machine.colour == COLOURCHROMA)
                                {
                                        memory[Addr] = Chr;
                                        memory[Addr + 0x4000] = Chr;
                                        memory[Addr - 0x6000] = Chr;
                                }

                                Addr++;
                        }
                }
                else
                {
                        Chr=hex2dec(tok);
                        font[Addr-0x8400]=Chr;

                        if (machine.colour == COLOURCHROMA)
                        {
                                memory[Addr] = Chr;
                                memory[Addr + 0x4000] = Chr;
                                memory[Addr - 0x6000] = Chr;
                        }
                        Addr++;
                }
        }
}

void load_snap_hires(FILE *f)
{
        while(!feof(f))
        {
                _TCHAR* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("TYPE")))
                {
                        tok = get_token(f);
                        SetComboBox(HW->HiResBox, tok);
                        
                        if (!_tcscmp(tok,_TEXT("G007"))) HW->EnableLowRAM->Checked=true;
                        if (!_tcscmp(tok,_TEXT("Memotech"))) HW->ProtectROM->Checked=true;
                }
        }
}

void load_snap_romcartridge(FILE *f)
{
        while(!feof(f))
        {
                _TCHAR* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("TYPE")))
                {
                        tok = get_token(f);
                        SetComboBox(HW->RomCartridgeBox, tok);
                }
                else if (!_tcscmp(tok,_TEXT("PATH")))
                {
                        tok = get_token(f);
                        ZXString path = StringReplace(tok, "_", " ", TReplaceFlags() << rfReplaceAll);
                        HW->RomCartridgeFileBox->Text = path;
                }
                else if (!_tcscmp(tok,_TEXT("CONFIGURATION")))
                {
                        tok = get_token(f);
                        SetComboBox(HW->ZXC1ConfigurationBox, tok);
                }
        }
}

void load_snap_interfaces(FILE *f)
{
        while(!feof(f))
        {
                _TCHAR* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("ZXPAND")))
                {
                        HW->SetZXpandState(hex2dec(get_token(f)),true);
                }
                else if (!_tcscmp(tok,_TEXT("ZX_PRINTER")))
                {
                        HW->ZXPrinter->Checked = hex2dec(get_token(f));
                }
        }
}

void load_snap_colour(FILE *f)
{
        int Addr, Count, Chr;
        _TCHAR *tok;
        bool chroma = false;

        Addr=0xC000;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("TYPE")))
                {
                        tok = get_token(f);
                        SetComboBox(HW->ColourBox, tok);
                        chroma = (HW->ColourBox->Text == "Chroma");
                        bool lambda = (HW->ColourBox->Text == "Lambda");
                        if (chroma)
                        {
                                Form1->ChromaColourEnable->Visible = true;
                                Form1->ChromaColourEnable->Enabled = true;
                                machine.colour = COLOURCHROMA;
                        }
                        else if (lambda)
                        {
                                machine.colour = COLOURLAMBDA;
                        }
                }
                else if (chroma && !_tcscmp(tok,_TEXT("CHROMA_MODE")))
                {
                        zx81.chromaMode = hex2dec(get_token(f));
                }
                else if (chroma && !_tcscmp(tok,_TEXT("COLOUR_ENABLED")))
                {
                        zx81.chromaColourSwitchOn = hex2dec(get_token(f));
                        Form1->ChromaColourEnable->Checked = zx81.chromaColourSwitchOn;
                }
                else if (chroma && *tok=='*')
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));
                        while(Count--) memory[Addr++]=Chr;
                }
                else if (chroma)
                {
                        memory[Addr++]=hex2dec(tok);
                }
        }
}

ZXString GetMachine()
{
        ZXString machineName;

        switch(emulator.machine)
        {
                case MACHINEZX80:
                        machineName = "ZX80";
                        break;
                case MACHINEZX81:
                        machineName = "ZX81";
                        break;
                case MACHINEACE:
                        machineName = "ACE";
                        break;
                case MACHINETS1000:
                        machineName = "1000";
                        break;
                case MACHINETS1500:
                        machineName = "1500";
                        break;
                case MACHINELAMBDA:
                        machineName = "LAMDA";
                        break;
                case MACHINEZX97LE:
                        machineName = "ZX97LE";
                        break;
                case MACHINER470:
                        machineName = "R470";
                        break;
                case MACHINETK85:
                        machineName = "TK85";
                        break;
                default:
                        machineName = "ZX81";
                        break;
        }

        return machineName;
}

void SetMachine(ZXString machine)
{
        if (machine == "ZX80") HWSetMachine(MACHINEZX80, NULL);
        else if (machine == "ZX81") HWSetMachine(MACHINEZX81, NULL);
        else if (machine == "1500") HWSetMachine(MACHINETS1500, NULL);
        else if (machine == "LAMDA") HWSetMachine(MACHINELAMBDA, NULL);
        else if (machine == "ZX97LE") HWSetMachine(MACHINEZX97LE, NULL);
        else if (machine == "R470") HWSetMachine(MACHINER470, NULL);
        else if (machine == "TK85") HWSetMachine(MACHINETK85, NULL);
        else if (machine == "1000") HWSetMachine(MACHINETS1000, NULL);
}

void load_snap_machine(FILE *f)
{
        _TCHAR *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("MODEL"))) SetMachine(get_token(f));
        }
}

void load_snap_zx81(FILE *f)
{
        _TCHAR *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("NMI"))) nmiGeneratorEnabled = hex2dec(get_token(f));
                else if (!_tcscmp(tok,_TEXT("SYNC"))) syncOutputWhite = hex2dec(get_token(f));
                else if (!_tcscmp(tok,_TEXT("LINE"))) lineCounter = hex2dec(get_token(f));
                //Backwards compatibility
                else if (!_tcscmp(tok,_TEXT("HSYNC"))) syncOutputWhite = hex2dec(get_token(f));
                else if (!_tcscmp(tok,_TEXT("ROW"))) lineCounter = hex2dec(get_token(f));
        }
}

void load_snap_mem(FILE *f)
{
        int Addr, Count, Chr;
        _TCHAR *tok;

        Addr=16384;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("RAM_PACK")))
                {
                        tok = get_token(f);

                        SetComboBox(HW->RamPackBox, tok);
                }
                else if (!_tcscmp(tok,_TEXT("MEMRANGE")))
                {
                        Addr=hex2dec(get_token(f));
                        get_token(f);
                }
                else if (!_tcscmp(tok,_TEXT("8K_RAM_ENABLED")))
                {
                        HW->EnableLowRAM->Checked=hex2dec(get_token(f));
                }
                else if (!_tcscmp(tok,_TEXT("8K_RAM_PROTECTED")))
                {
                        Form1->WriteProtect8KRAM->Checked = hex2dec(get_token(f));
                }
                else if (*tok=='*')
                {
                        Count=hex2dec(tok+1);
                        Chr=hex2dec(get_token(f));
                        while(Count--) memory[Addr++]=Chr;
                }
                else
                {
                        memory[Addr++]=hex2dec(tok);
                }
        }
}

void load_snap_advanced(FILE* f)
{
        _TCHAR *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("ROM")))
                {
                        tok = get_token(f);
                        ZXString rom = StringReplace(tok, "_", " ", TReplaceFlags() << rfReplaceAll);
                        HW->RomBox->Text = rom;
                }
                else if (!_tcscmp(tok,_TEXT("PROTECT_ROM")))
                {
                        HW->ProtectROM->Checked = hex2dec(get_token(f));
                }
                else if (!_tcscmp(tok,_TEXT("M1NOT")))
                {
                        HW->M1Not->Checked = hex2dec(get_token(f));
                }
                else if (!_tcscmp(tok,_TEXT("IMPROVED_WAIT")))
                {
                        HW->ImprovedWait->Checked = hex2dec(get_token(f));
                }
                else if (!_tcscmp(tok,_TEXT("FLOATING_POINT_FIX")))
                {
                        HW->FloatingPointHardwareFix->Checked = hex2dec(get_token(f));
                }
                else if (!_tcscmp(tok,_TEXT("FRAME_RATE_60HZ")))
                {
                        HW->NTSC->Checked = hex2dec(get_token(f));
                }
        }
}

void load_snap_drives(FILE* f)
{
        _TCHAR *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!_tcscmp(tok,_TEXT("FDC")))
                {
                        SetComboBox(HW->FDC, get_token(f));
                }
                else if (!_tcscmp(tok,_TEXT("IDE")))
                {
                        SetComboBox(HW->IDEBox, get_token(f));
                }
        }
}

void ProcessTag(_TCHAR* tok, FILE* f)
{             
        if (!_tcscmp(tok, _TEXT("[CPU]"))) load_snap_cpu(f);
        else if (!_tcscmp(tok, _TEXT("[MACHINE]"))) load_snap_machine(f);
        else if (!_tcscmp(tok, _TEXT("[MEMORY]"))) load_snap_mem(f);
        else if (!_tcscmp(tok, _TEXT("[ZX81]"))) load_snap_zx81(f);
        else if (!_tcscmp(tok, _TEXT("[COLOUR]"))) load_snap_colour(f);
        else if (!_tcscmp(tok, _TEXT("[SOUND]"))) load_snap_sound(f);
        else if (!_tcscmp(tok, _TEXT("[SPEECH]"))) load_snap_speech(f);
        else if (!_tcscmp(tok, _TEXT("[CHR$_GENERATOR]"))) load_snap_chrgen(f);
        else if (!_tcscmp(tok, _TEXT("[HIGH_RESOLUTION]"))) load_snap_hires(f);
        else if (!_tcscmp(tok, _TEXT("[ROM_CARTRIDGE]"))) load_snap_romcartridge(f);
        else if (!_tcscmp(tok, _TEXT("[INTERFACES]"))) load_snap_interfaces(f);
        else if (!_tcscmp(tok, _TEXT("[ADVANCED]"))) load_snap_advanced(f);
        else if (!_tcscmp(tok, _TEXT("[DRIVES]"))) load_snap_drives(f);
}

void load_snap_ace(FILE *f)
{
        int memptr=0x2000;
        unsigned char c;
        int len, eof;

        eof=0;

        while(!eof)
        {
                c=fgetc(f);

                if (c!=0xED) memory[memptr++]=c;
                else
                {
                        len=fgetc(f);

                        if (!len) eof=1;
                        else
                        {
                                c=fgetc(f);
                                while(len--) memory[memptr++]=c;
                        }
                }

                if (feof(f)) eof=1;
        }

        int ramTop = memory[0x2081]*256;
        int ramSize = (ramTop - 0x3400) >> 10;
        int ramPackSize = ramSize - machine.baseRamSize;

        zx81.RAMTOP = ramTop-1;
        if (zx81.RAMTOP == -1) zx81.RAMTOP=65535;

        ZXString ramPackText = IntToStr(ramPackSize) + "K";
        SetComboBox(HW->RamPackBox, ramPackText.c_str());

        memptr=0x2100;

        z80.af.b.l = memory[memptr]; z80.af.b.h = memory[memptr+1]; memptr+=4;
        z80.bc.b.l = memory[memptr]; z80.bc.b.h = memory[memptr+1]; memptr+=4;
        z80.de.b.l = memory[memptr]; z80.de.b.h = memory[memptr+1]; memptr+=4;
        z80.hl.b.l = memory[memptr]; z80.hl.b.h = memory[memptr+1]; memptr+=4;
        z80.ix.b.l = memory[memptr]; z80.ix.b.h = memory[memptr+1]; memptr+=4;
        z80.iy.b.l = memory[memptr]; z80.iy.b.h = memory[memptr+1]; memptr+=4;
        z80.sp.b.l = memory[memptr]; z80.sp.b.h = memory[memptr+1]; memptr+=4;
        z80.pc.b.l = memory[memptr]; z80.pc.b.h = memory[memptr+1]; memptr+=4;
        z80.af_.b.l = memory[memptr]; z80.af_.b.h = memory[memptr+1]; memptr+=4;
        z80.bc_.b.l = memory[memptr]; z80.bc.b.h = memory[memptr+1]; memptr+=4;
        z80.de_.b.l = memory[memptr]; z80.de_.b.h = memory[memptr+1]; memptr+=4;
        z80.hl_.b.l = memory[memptr]; z80.hl_.b.h = memory[memptr+1]; memptr+=4;

        z80.im = memory[memptr]; memptr+=4;
        z80.iff1 = memory[memptr]; memptr+=4;
        z80.iff2 = memory[memptr]; memptr+=4;
        z80.i = memory[memptr]; memptr+=4;
        z80.r = memory[memptr];
}

int do_load_snap(_TCHAR *filename, bool resetHardware)
{
        _TCHAR *p;
        FILE *f;

        p=filename+_tcslen(filename)-4;

        if (_tcscmp(p,_TEXT(".Z81")) && _tcscmp(p,_TEXT(".z81"))
                && _tcscmp(p,_TEXT(".ace")) && _tcscmp(p,_TEXT(".ACE")) ) return(0);

        if (!_tcscmp(p,_TEXT(".ace")) || !_tcscmp(p,_TEXT(".ACE")))
        {
                f=_tfopen(filename,_TEXT("rb"));
                if (!f)
                {
                        ShowMessage("Snapshot load failed.");
                        return 0;
                }

                if (resetHardware)
                {
                        HWSetMachine(MACHINEACE, NULL);
                        machine.initialise();
                }
                load_snap_ace(f);
        }
        else
        {
                if (resetHardware) machine.initialise();

                f=_tfopen(filename,_TEXT("rt"));
                if (!f) return(0);

                while(!feof(f))
                {
                        _TCHAR* tok = get_token(f);
                        if (tok[0] == '[')
                        {
                                ProcessTag(tok, f);
                        }
                 }
        }

        fclose(f);
        DebugUpdate();
        return(1);
}

void InitialiseHardware()
{
        HWSetMachine(MACHINEZX81, NULL);

        InitialiseChroma();
        
        SetComboBox(HW->ColourBox, (_TCHAR *)_TEXT("None"));
        SetComboBox(HW->SoundCardBox, (_TCHAR *)_TEXT("None"));
        SetComboBox(HW->ChrGenBox, (_TCHAR *)_TEXT("Sinclair"));
        SetComboBox(HW->HiResBox, (_TCHAR *)_TEXT("None"));
        SetComboBox(HW->RomCartridgeBox, (_TCHAR *)_TEXT("None"));
        SetComboBox(HW->ZXC1ConfigurationBox, (_TCHAR *)_TEXT("32K"));
        HW->ZXC1ConfigurationBox->Visible = false;
        HW->RomCartridgeFileBox->Left = 86;
        HW->RomCartridgeFileBox->Width = 281;
        HW->RomCartridgeFileBox->Text = "";
        HW->SinclairRomCartridgeFileBox->Text = "";
        HW->TS1510RomCartridgeFileBox->Text = "";
        HW->TS2068RomCartridgeFileBox->Text = "";
        HW->TC2068RomCartridgeFileBox->Text = "";
        HW->SinclairRomCartridgeFileBox->Visible = false;
        HW->TS1510RomCartridgeFileBox->Visible = false;
        HW->TS2068RomCartridgeFileBox->Visible = false;
        HW->TC2068RomCartridgeFileBox->Visible = false;
        HW->RomCartridgeFileBox->Visible = true;
        HW->BrowseRomCartridge->Enabled = false;
        HW->ZXPrinter->Checked = false;
        HW->SetZXpandState(false,false);
        HW->ProtectROM->Checked=true;
        HW->EnableLowRAM->Checked=false;
        HW->M1Not->Checked=false;
        HW->ImprovedWait->Checked=false;
        HW->FloatingPointHardwareFix->Checked=false;
        HW->NTSC->Checked=false;

        Form1->ChromaColourEnable->Checked = false;
        Form1->ChromaColourEnable->Enabled = false;
        Form1->ChromaColourEnable->Visible = false;
        Form1->QSChrEnable->Checked = false;
        Form1->QSChrEnable->Enabled = false;
        Form1->QSChrEnable->Visible = false;
        Form1->ResetMemotechHRG->Visible = false;
        Form1->ResetMemotechHRG->Enabled = false;
        Form1->ResetQuicksilvaHiRes->Visible = false;
        Form1->ResetQuicksilvaHiRes->Enabled = false;

        MemotechMode = 0;
        QuicksilvaHiResMode = 0;
        zx81.enableQSchrgen = false;
        zx81.chromaColourSwitchOn = false;
}

int load_snap(_TCHAR *filename)
{
        // Read in the snapshot into memory and settings (this also resets the computer)
        int ret = do_load_snap(filename, true);
        if (!ret) return ret;

        // Re-apply the settings without a reset
        const bool reinitialiseStatus = true;
        bool disableResetStatus = true;
        HW->UpdateHardwareSettings(reinitialiseStatus, disableResetStatus);

        return true;
}

int save_snap(_TCHAR *filename)
{
        _TCHAR *p;
        int f;

        p=filename+_tcslen(filename)-4;

        if (_tcscmp(p,_TEXT(".Z81")) && _tcscmp(p,_TEXT(".z81"))
                && _tcscmp(p,_TEXT(".ace")) && _tcscmp(p,_TEXT(".ACE")) ) return(0);

        if (!_tcscmp(p,_TEXT(".ace")) || !_tcscmp(p,_TEXT(".ACE")))
        {
                f = save_snap_ace(filename);
        }
        else
        {
                f = save_snap_zx81(filename);
        }

        if (f)
        {
                ShowMessage("Save snapshot failed.");
        }

        return(0);
}

int save_snap_zx81(_TCHAR *filename)
{
        FILE *f;
        int Addr, Count, Chr;

	f=_tfopen(filename,_TEXT("wt"));
	if (!f)
	{
	        return -1;
	}

	fprintf(f,"[MACHINE]\n");
#ifdef _UNICODE
	fprintf(f,"MODEL %S\n", GetMachine().c_str());
#else
	fprintf(f,"MODEL %s\n", GetMachine().c_str());
#endif

	fprintf(f,"\n[CPU]\n");
	fprintf(f,"PC %04X    SP  %04X\n", z80.pc.w,z80.sp.w);
	fprintf(f,"HL %04X    HL_ %04X\n", z80.hl.w,z80.hl_.w);
	fprintf(f,"DE %04X    DE_ %04X\n", z80.de.w,z80.de_.w);
	fprintf(f,"BC %04X    BC_ %04X\n", z80.bc.w,z80.bc_.w);
	fprintf(f,"AF %04X    AF_ %04X\n", z80.af.w,z80.af_.w);
	fprintf(f,"IX %04X    IY  %04X\n", z80.ix.w,z80.iy.w);
	fprintf(f,"IR %04X\n", (z80.i<<8) | (z80.r7 & 128) | ((z80.r) & 127));

	fprintf(f,"IM %02X      IF1 %02X\n", z80.im, z80.iff1);
	fprintf(f,"HT %02X      IF2 %02X\n", z80.halted, z80.iff2);

	fprintf(f,"\n[ZX81]\n");
	fprintf(f,"NMI %02X     SYNC %02X\n", nmiGeneratorEnabled, syncOutputWhite);
	fprintf(f,"LINE %03X\n", lineCounter);

	fprintf(f,"\n[MEMORY]\n");
	fprintf(f,"RAM_PACK %S\n", HW->RamPackBox->Text.c_str());
	fprintf(f,"8K_RAM_ENABLED %02X\n", zx81.RAM816k);
	fprintf(f,"8K_RAM_PROTECTED %02X\n", zx81.RAM816kWriteProtected);

	Addr = zx81.RAM816k || zx81.zxpand ? 8192 : zx81.ROMTOP+1;
	int topOfRAM = zx81.RAMTOP;
	if (zx81.zxpand && (topOfRAM < 0xBFFF))
	{
	        topOfRAM = 0xBFFF;
	}
	fprintf(f,"MEMRANGE %04X %04X\n", Addr, topOfRAM);

	while(Addr <= topOfRAM)
	{
		Chr=memory[Addr];
		Count=1;

		while((memory[Addr+Count]==Chr) && ((Addr+Count)<=topOfRAM))
		        Count++;

                if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
		else fprintf(f,"%02X ",Chr);

		Addr += Count;
	}
	fprintf(f, "\n");

	fprintf(f,"\n[ADVANCED]\n");
	fprintf(f,"ROM %S\n", ReplaceSpaces(HW->RomBox->Text).c_str());
	fprintf(f,"PROTECT_ROM %02X\n", machine.protectROM);
	fprintf(f,"M1NOT %02X\n", (zx81.m1not == 0xC000));
	fprintf(f,"IMPROVED_WAIT %02X\n", zx81.improvedWait);
	fprintf(f,"FLOATING_POINT_FIX %02X\n", zx81.FloatingPointHardwareFix);
	fprintf(f,"FRAME_RATE_60HZ %02X\n", machine.NTSC);

	fprintf(f,"\n[SOUND]\n");
	fprintf(f,"TYPE %S\n", ReplaceSpaces(HW->SoundCardBox->Text).c_str());

	fprintf(f,"\n[SPEECH]\n");
	fprintf(f,"TYPE %S\n", ReplaceSpaces(HW->SpeechBox->Text).c_str());

	// Must output this before the Chr$ Generator section
	fprintf(f,"\n[COLOUR]\n");
	fprintf(f,"TYPE %S\n", HW->ColourBox->Text.c_str());

	if (machine.colour == COLOURCHROMA)
	{
                Addr = 0xC000;

		while (Addr <= 0xFFFF)
		{
		        Chr=memory[Addr];
			Count=1;

                        while((memory[Addr+Count]==Chr) && ((Addr+Count)<=0xFFFF))
			        Count++;

                        if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
			else fprintf(f,"%02X ",Chr);

                        Addr += Count;
                }

		fprintf(f,"\nCHROMA_MODE %02X\n", zx81.chromaMode);
		fprintf(f,"COLOUR_ENABLED %02X\n", zx81.chromaColourSwitchOn);
	}

	fprintf(f,"\n[CHR$_GENERATOR]\n");
	fprintf(f,"TYPE %S\n", HW->ChrGenBox->Text.c_str());

	if (zx81.chrgen == CHRGENQS)
	{
		fprintf(f,"ENABLED %02X\n", zx81.enableQSchrgen);

		Addr = 0x8400;

                while (Addr <= 0x8800)
		{
                        Chr=font[Addr-0x8400];
			Count=1;

			while((font[Addr-0x8400+Count]==Chr) && ((Addr+Count)<=0x8800))
			        Count++;

			if (Count>1) fprintf(f,"*%04X %02X ",Count, Chr);
			else fprintf(f,"%02X ",Chr);

		        Addr += Count;
		}
		fprintf(f, "\n");
	}

	fprintf(f,"\n[HIGH_RESOLUTION]\n");
	fprintf(f,"TYPE %S\n", HW->HiResBox->Text.c_str());

	fprintf(f,"\n[ROM_CARTRIDGE]\n");
	fprintf(f,"TYPE %S\n", StringReplace(HW->RomCartridgeBox->Text, " ", "_", TReplaceFlags() << rfReplaceAll).c_str());
	if (HW->RomCartridgeBox->Text != "None")
	{
		fprintf(f,"PATH %S\n", StringReplace(HW->RomCartridgeFileBox->Text, " ", "_", TReplaceFlags() << rfReplaceAll).c_str());
		if (HW->RomCartridgeBox->Text == "ZXC1")
		{
		        fprintf(f,"CONFIGURATION %S\n", HW->ZXC1ConfigurationBox->Text.c_str());
		}
	}

	fprintf(f,"\n[INTERFACES]\n");
	fprintf(f,"ZX_PRINTER %02X\n", machine.zxprinter);
	fprintf(f,"ZXPAND %02X\n", zx81.zxpand);

	fprintf(f,"\n[DRIVES]\n");
	fprintf(f,"FDC %S\n", HW->FDC->Text.c_str());
	fprintf(f,"IDE %S\n", HW->IDEBox->Text.c_str());

        fprintf(f,"\n[EOF]\n");
	fclose(f);

        return 0;
}

int save_snap_ace(_TCHAR *filename)
{
	FILE *f;
        int Addr, Count, Chr, memptr;

	f=_tfopen(filename,_TEXT("wb"));
	if (!f)
	{
	        ShowMessage("Save snapshot failed.");
		return -1;
	}

	memptr=0x2000;
	memory[memptr]=0x01; memory[memptr+1]=0x80;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x2080;
	memory[memptr]=0x00; memory[memptr+1]=(zx81.RAMTOP+1)/256;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0284;
	memory[memptr]=0x00; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0288;
	memory[memptr]=0x00; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x028c;
	memory[memptr]=0x03; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0290;
	memory[memptr]=0x03; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x0294;
	memory[memptr]=0xfd; memory[memptr+1]=0xfd;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr+=0x0298;
	memory[memptr]=0x01; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr+=0x029c;
	memory[memptr]=0x00; memory[memptr+1]=0x00;
	memory[memptr+2]=0x00; memory[memptr+3]=0x00;

	memptr=0x2100;

	memory[memptr] = z80.af.b.l; memory[memptr+1] = z80.af.b.h; memptr+=4;
	memory[memptr] = z80.bc.b.l; memory[memptr+1] = z80.bc.b.h; memptr+=4;
	memory[memptr] = z80.de.b.l; memory[memptr+1] = z80.de.b.h; memptr+=4;
	memory[memptr] = z80.hl.b.l; memory[memptr+1] = z80.hl.b.h; memptr+=4;
	memory[memptr] = z80.ix.b.l; memory[memptr+1] = z80.ix.b.h; memptr+=4;
	memory[memptr] = z80.iy.b.l; memory[memptr+1] = z80.iy.b.h; memptr+=4;
	memory[memptr] = z80.sp.b.l; memory[memptr+1] = z80.sp.b.h; memptr+=4;
	memory[memptr] = z80.pc.b.l; memory[memptr+1] = z80.pc.b.h; memptr+=4;
	memory[memptr] = z80.af_.b.l; memory[memptr+1] = z80.af_.b.h; memptr+=4;
	memory[memptr] = z80.bc_.b.l; memory[memptr+1] = z80.bc.b.h ; memptr+=4;
	memory[memptr] = z80.de_.b.l; memory[memptr+1] = z80.de_.b.h; memptr+=4;
	memory[memptr] = z80.hl_.b.l; memory[memptr+1] = z80.hl_.b.h; memptr+=4;

	memory[memptr] = z80.im ; memptr+=4;
	memory[memptr] = z80.iff1; memptr+=4;
	memory[memptr] = z80.iff2; memptr+=4;
	memory[memptr] = z80.i; memptr+=4;
	memory[memptr] = z80.r; memptr+=4;
	memory[memptr] = 0x80; memory[memptr+1] = 0x00; memory[memptr+2] = 0x00; memory[memptr+3] = 0x00;

	Addr=0x2000;

	while(Addr <= zx81.RAMTOP)
	{
	        Chr=memory[Addr];
		Count=1;

		while((memory[Addr+Count] == Chr) && ((Addr+Count) <= zx81.RAMTOP))
			Count++;

		if (Count>240) Count=240;

		if (Count>3 || Chr==0xed)
		{
			fputc(0xed,f);
		        fputc(Count,f);
		}
		else    Count=1;

		fputc(Chr,f);
		Addr+=Count;
	}

	fputc(0xed,f);
	fputc(0x00,f);

        fclose(f);
        
	return 0;
}

int memoryLoadToAddress(_TCHAR *filename, void* destAddress, int length)
{
        int fptr;
        _TCHAR file[256];
        int len;

        if (_tcschr(filename, '\\') || _tcschr(filename, '/'))
        {
                _tcscpy(file, filename);
        }
        else
        {
                _tcscpy(file, emulator.cwd);
                _tcscat(file, romsFolder);
                _tcscat(file, filename);
        }

        fptr=_topen(file, O_RDONLY | O_BINARY);
        if (fptr<1)
        {
                int err=errno;
                ZXString errMsg = "ROM load to address failed:\n" + ZXString(filename);
                ShowMessage(errMsg);
                return(err);
        }

        if ((len=read(fptr, destAddress, length))==-1)
        {
                int err=errno;
                close(fptr);
                ZXString errMsg = "ROM load to address failed:\n" + ZXString(filename);
                ShowMessage(errMsg);
                return(err);
        }

        close(fptr);

        return(len);
}

int do_memory_load(_TCHAR *file, int address, int length, int secondBank)
{
        int fptr;
        int len;

        fptr=_topen(file, O_RDONLY | O_BINARY);
        if (fptr<1)
        {
                int err=errno;
                ZXString errMsg = "ROM load failed:\n" + ZXString(file);
                ShowMessage(errMsg);
                return(err);
        }

        if ((len=read(fptr, memory+address, length))==-1)
        {
                int err=errno;
                close(fptr);
                ZXString errMsg = "ROM load failed:\n" + ZXString(file);
                ShowMessage(errMsg);
                return(err);
        }

        if (secondBank)
        {
                if ((len=read(fptr, memory+address, length))==-1)
                {
                        int err=errno;
                        close(fptr);
                        ZXString errMsg = "ROM load failed:\n" + ZXString(file);
                        ShowMessage(errMsg);
                        return(err);
                }
        }

        close(fptr);

        return(len);
}

int memory_device_rom_load(_TCHAR *filename, int address, int length)
{
        _TCHAR file[256];

        _tcscpy(file, emulator.cwd);
        _tcscat(file, filename);

        return do_memory_load(file, address, length, 0);
}

int memory_load(_TCHAR *filename, int address, int length, int secondBank)
{
        int fptr;
        _TCHAR file[256];
        int len;

        if (_tcschr(filename, '\\') || _tcschr(filename, '/'))
        {
                _tcscpy(file, filename);
        }
        else
        {
                _tcscpy(file, emulator.cwd);
                _tcscat(file, romsFolder);
                _tcscat(file, filename);
        }

        return do_memory_load(file, address, length, secondBank);
}

int font_load(const _TCHAR *filename, char *address, int length)
{
        int fptr;
        _TCHAR file[256];
        int len;

        _tcscpy(file, emulator.cwd);
        _tcscat(file, romsFolder);
        _tcscat(file, filename);

        fptr=_topen(file, O_RDONLY | O_BINARY);
        if (fptr<1) return(errno);

        if ((len=read(fptr, address, length))==-1)
        {
                int err=errno;
                close(fptr);
                ZXString errMsg = "Font load failed:\n" + ZXString(filename);
                ShowMessage(errMsg);
                return(err);
        }

        close(fptr);

        return(len);
}


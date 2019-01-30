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

extern int rowcounter;
extern int MemotechMode;
extern BYTE font[1024];

void load_snap_cpu(FILE *f);
void load_snap_mem(FILE *f);
void load_snap_zx81(FILE *f);
void load_snap_sound(FILE *f);
void load_snap_chrgen(FILE *f);
void load_snap_hires(FILE *f);
void load_snap_colour(FILE *f);
void load_snap_romcartridge(FILE *f);
void load_snap_interfaces(FILE *f);
void load_snap_advanced(FILE* f);
void ProcessTag(char* tok, FILE* f);
void InitialiseHardware();

extern DebugUpdate();

char *get_token(FILE *f)
{
        static char buffer[256];
        int buflen;
        char c;

        c=fgetc(f);
        while(isspace(c) && !feof(f)) c=fgetc(f);

        //if (feof(f)) return(NULL);

        buflen=0;
        buffer[buflen++]=c;

        c=fgetc(f);
        while(!isspace(c) && !feof(f) && buflen<255)
        {
                buffer[buflen++]=c;
                c=fgetc(f);
        }

        buffer[buflen]='\0';

        //if (!buflen) return(NULL);
        return(buffer);
}

int hex2dec(char *str)
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

void SetComboBox(TComboBox* comboBox, char* text)
{
        for (int i = 0; i < comboBox->Items->Count; i++)
        {
                if (comboBox->Items->Strings[i] == text)
                {
                        comboBox->ItemIndex = i;
                        break;
                }
        }
}

void load_snap_cpu(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"PC")) z80.pc.w = hex2dec(get_token(f));
                if (!strcmp(tok,"SP")) z80.sp.w = hex2dec(get_token(f));
                if (!strcmp(tok,"HL")) z80.hl.w = hex2dec(get_token(f));
                if (!strcmp(tok,"DE")) z80.de.w = hex2dec(get_token(f));
                if (!strcmp(tok,"BC")) z80.bc.w = hex2dec(get_token(f));
                if (!strcmp(tok,"AF")) z80.af.w = hex2dec(get_token(f));
                if (!strcmp(tok,"HL_")) z80.hl_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"DE_")) z80.de_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"BC_")) z80.bc_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"AF_")) z80.af_.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IX")) z80.ix.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IY")) z80.iy.w = hex2dec(get_token(f));
                if (!strcmp(tok,"IM")) z80.im = hex2dec(get_token(f));
                if (!strcmp(tok,"IF1")) z80.iff1 = hex2dec(get_token(f));
                if (!strcmp(tok,"IF2")) z80.iff2 = hex2dec(get_token(f));
                if (!strcmp(tok,"HT")) z80.halted = hex2dec(get_token(f));
                if (!strcmp(tok,"IR"))
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
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->SoundCardBox, tok);
                }
        }
}

void load_snap_chrgen(FILE *f)
{
        int Addr, Count, Chr;

        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
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
                else if ((HW->ChrGenBox->Text == "Quicksilva") && !strcmp(tok,"ENABLED"))
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

                                if (zx81.colour == COLOURCHROMA)
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
                        
                        if (zx81.colour == COLOURCHROMA)
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
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->HiResBox, tok);
                        
                        if (tok=="G007") HW->EnableLowRAM->Checked=true;
                        if (tok=="Memotech") HW->ProtectROM->Checked=true;
                }
        }
}

void load_snap_romcartridge(FILE *f)
{
        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->RomCartridgeBox, tok);
                }
                else if (!strcmp(tok,"PATH"))
                {
                        tok = get_token(f);
                        HW->RomCartridgeFileBox->Text = tok;
                }
        }

        bool romCartridgeSelected = (HW->RomCartridgeBox->Text != "None");
        HW->RomCartridgeFileBox->Enabled = romCartridgeSelected;
        HW->BrowseRomCartridge->Enabled = romCartridgeSelected;
}

void load_snap_interfaces(FILE *f)
{
        while(!feof(f))
        {
                char* tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"ZXPAND"))
                {
                        HW->ZXpand->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"ZX_PRINTER"))
                {
                        HW->ZXPrinter->Checked = hex2dec(get_token(f));
                }
        }
}

void load_snap_colour(FILE *f)
{
        int Addr, Count, Chr;
        char *tok;
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

                if (!strcmp(tok,"TYPE"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->ColourBox, tok);
                        chroma = (HW->ColourBox->Text == "Chroma");
                        if (chroma)
                        {
                                Form1->ChromaColourEnable->Visible = true;
                                Form1->ChromaColourEnable->Enabled = true;
                        }
                }
                else if (chroma && !strcmp(tok,"CHROMA_MODE"))
                {
                        zx81.chromaMode = hex2dec(get_token(f));
                }
                else if (chroma && !strcmp(tok,"COLOUR_ENABLED"))
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

void load_snap_zx81(FILE *f)
{
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"NMI")) NMI_generator = hex2dec(get_token(f));
                else if (!strcmp(tok,"HSYNC")) HSYNC_generator = hex2dec(get_token(f));
                else if (!strcmp(tok,"ROW")) rowcounter = hex2dec(get_token(f));
        }
}

void load_snap_mem(FILE *f)
{
        int Addr, Count, Chr;
        char *tok;

        Addr=16384;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"RAM_PACK"))
                {
                        tok = get_token(f);
                        SetComboBox(HW->RamPackBox, tok);
                }
                else if (!strcmp(tok,"MEMRANGE"))
                {
                        Addr=hex2dec(get_token(f));
                        get_token(f);
                }
                else if (!strcmp(tok,"8K_RAM_ENABLED"))
                {
                        HW->EnableLowRAM->Checked=hex2dec(get_token(f));
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
        char *tok;

        while(!feof(f))
        {
                tok=get_token(f);
                if (tok[0] == '[')
                {
                        ProcessTag(tok, f);
                        return;
                }

                if (!strcmp(tok,"ROM"))
                {
                        HW->RomBox->Text = get_token(f);
                }
                else if (!strcmp(tok,"PROTECT_ROM"))
                {
                        HW->ProtectROM->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"M1NOT"))
                {
                        HW->M1Not->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"FLOATING_POINT_FIX"))
                {
                        HW->FloatingPointHardwareFix->Checked = hex2dec(get_token(f));
                }
                else if (!strcmp(tok,"FRAME_RATE_60HZ"))
                {
                        HW->NTSC->Checked = hex2dec(get_token(f));
                }
        }
}

void ProcessTag(char* tok, FILE* f)
{             
        if (!strcmp(tok, "[CPU]")) load_snap_cpu(f);
        else if (!strcmp(tok, "[MEMORY]")) load_snap_mem(f);
        else if (!strcmp(tok, "[ZX81]")) load_snap_zx81(f);
        else if (!strcmp(tok, "[COLOUR]")) load_snap_colour(f);
        else if (!strcmp(tok, "[SOUND]")) load_snap_sound(f);
        else if (!strcmp(tok, "[CHR$_GENERATOR]")) load_snap_chrgen(f);
        else if (!strcmp(tok, "[HIGH_RESOLUTION]")) load_snap_hires(f);
        else if (!strcmp(tok, "[ROM_CARTRIDGE]")) load_snap_romcartridge(f);
        else if (!strcmp(tok, "[INTERFACES]")) load_snap_interfaces(f);
        else if (!strcmp(tok, "[ADVANCED]")) load_snap_advanced(f);
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

        zx81.RAMTOP = (memory[0x2081]*256)-1;
        if (zx81.RAMTOP == -1) zx81.RAMTOP=65535;

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

int do_load_snap(char *filename)
{
        char *p;
        FILE *f;

        p=filename+strlen(filename)-4;

        if (strcmp(p,".Z81") && strcmp(p,".z81")
                && strcmp(p,".ace") && strcmp(p,".ACE") ) return(0);


        if (!strcmp(p,".ace") || !strcmp(p,".ACE"))
        {
                f=fopen(filename,"rb");
                if (!f)
                {
                        ShowMessage("Snapshot load failed.");
                        return 0;
                }
                load_snap_ace(f);
        }
        else
        {
                InitialiseHardware();

                f=fopen(filename,"rt");
                if (!f) return(0);

                while(!feof(f))
                {
                        char* tok = get_token(f);
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
        InitialiseChroma();
        
        SetComboBox(HW->ColourBox, "None");
        SetComboBox(HW->SoundCardBox, "None");
        SetComboBox(HW->ChrGenBox, "Sinclair");
        SetComboBox(HW->HiResBox, "None");
        SetComboBox(HW->RomCartridgeBox, "None");
        HW->RomCartridgeFileBox->Text = "";
        HW->BrowseRomCartridge->Enabled = false;
        HW->ZXPrinter->Checked = false;
        HW->ZXpand->Checked = false;
        HW->ProtectROM->Checked=true;
        HW->EnableLowRAM->Checked=false;
        HW->M1Not->Checked=false;
        HW->FloatingPointHardwareFix->Checked=false;
        HW->NTSC->Checked=false;
        
        Form1->ChromaColourEnable->Checked = false;
        Form1->ChromaColourEnable->Enabled = false;
        Form1->ChromaColourEnable->Visible = false;
        Form1->QSChrEnable->Checked = false;
        Form1->QSChrEnable->Enabled = false;
        Form1->QSChrEnable->Visible = false;
        Form1->MemotechReset->Visible = false;
        Form1->MemotechReset->Enabled = false;

        MemotechMode = 0;
        zx81.enableQSchrgen = false;
        zx81.chromaColourSwitchOn = false;
}

int load_snap(char *filename)
{
        int ret = do_load_snap(filename);
        if (!ret) return ret;

        HW->OKClick(NULL);

        ret = do_load_snap(filename);
        if (!ret) return ret;

        Artifacts->SelectRGBOutput(zx81.colour == COLOURCHROMA);

        return true;
}

int save_snap(char *filename)
{
        FILE *f;
        char *p;
        int Addr, Count, Chr, memptr;

        p=filename+strlen(filename)-4;

        if (strcmp(p,".Z81") && strcmp(p,".z81")
                && strcmp(p,".ace") && strcmp(p,".ACE") ) return(0);


        if (!strcmp(p,".ace") || !strcmp(p,".ACE"))
        {
                f=fopen(filename,"wb");
                if (!f)
                {
                        ShowMessage("Save failed.");
                        return(0);
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
                memory[memptr] = z80.r;

                Addr=0x2000;

                while(Addr<32768)
                {
                        Chr=memory[Addr];
                        Count=1;

                        while((memory[Addr+Count]==Chr) && ((Addr+Count)<=32768))
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
        }
        else
        {
                f=fopen(filename,"wt");
                if (!f)
                {
                        ShowMessage("Save failed.");
                        return(0);
                }

                fprintf(f,"[CPU]\n");
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
                fprintf(f,"NMI %02X     HSYNC %02X\n", NMI_generator, HSYNC_generator);
                fprintf(f,"ROW %03X\n", rowcounter);

                fprintf(f,"\n[MEMORY]\n");
                fprintf(f,"RAM_PACK %s\n", HW->RamPackBox->Text.c_str());
                fprintf(f,"8K_RAM_ENABLED %02X\n", zx81.RAM816k);

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
                fprintf(f,"ROM %s\n", HW->RomBox->Text.c_str());
                fprintf(f,"PROTECT_ROM %02X\n", zx81.protectROM);
                fprintf(f,"M1NOT %02X\n", (zx81.m1not == 0xC000));
                fprintf(f,"FLOATING_POINT_FIX %02X\n", zx81.FloatingPointHardwareFix);
                fprintf(f,"FRAME_RATE_60HZ %02X\n", zx81.NTSC);

                fprintf(f,"\n[SOUND]\n");
                fprintf(f,"TYPE %s\n", HW->SoundCardBox->Text.c_str());

                // Must output this before the Chr$ Generator section
                fprintf(f,"\n[COLOUR]\n");
                fprintf(f,"TYPE %s\n", HW->ColourBox->Text.c_str());

                if (zx81.colour == COLOURCHROMA)
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
                fprintf(f,"TYPE %s\n", HW->ChrGenBox->Text.c_str());

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
                fprintf(f,"TYPE %s\n", HW->HiResBox->Text.c_str());

                fprintf(f,"\n[ROM_CARTRIDGE]\n");
                fprintf(f,"TYPE %s\n", HW->RomCartridgeBox->Text.c_str());
                if (HW->RomCartridgeBox->Text != "None")
                {
                        fprintf(f,"PATH %s\n", HW->RomCartridgeFileBox->Text.c_str());
                }

                fprintf(f,"\n[INTERFACES]\n");
                fprintf(f,"ZX_PRINTER %02X\n", zx81.zxprinter);
                fprintf(f,"ZXPAND %02X\n", zx81.zxpand);

                fprintf(f,"\n[EOF]\n");
        }
        fclose(f);
        return(0);
}

int memoryLoadToAddress(char *filename, void* destAddress, int length)
{
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
        if (fptr<1)
        {
                ShowMessage("ROM load failed.");
                return(errno);
        }

        if ((len=read(fptr, destAddress, length))==-1)
        {
                int err=errno;
                close(fptr);
                ShowMessage("ROM load failed.");
                return(err);
        }

        close(fptr);

        return(len);
}

int memory_load(char *filename, int address, int length)
{
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
        if (fptr<1)
        {
                ShowMessage("ROM load failed.");
                return(errno);
        }

        if ((len=read(fptr, memory+address, length))==-1)
        {
                int err=errno;
                close(fptr);
                ShowMessage("ROM load failed.");
                return(err);
        }

        close(fptr);

        return(len);
}

int font_load(char *filename, char *address, int length)
{
        int fptr;
        char file[256];
        int len;

        strcpy(file, zx81.cwd);
        strcat(file,"ROM\\");
        strcat(file,filename);

        fptr=open(file, O_RDONLY | O_BINARY);
        if (fptr<1) return(errno);

        if ((len=read(fptr, address, length))==-1)
        {
                int err=errno;
                close(fptr);
                ShowMessage("Font load failed.");
                return(err);
        }

        close(fptr);

        return(len);
}


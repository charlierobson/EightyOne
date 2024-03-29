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
 * spec48snap.c
 *
 */
#include <vcl.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <io.h>
#include <mem.h>
#include <string.h>

#include "zx81.h"
#include "z80\z80.h"
#include "snap.h"
#include "zx81config.h"
#include "WavCInterface.h"
#include "sound.h"
#include "dev8255.h"
#include "serialport.h"
#include "zxprinter_c.h"
#include "rompatch.h"
#include "tzxman.h"
#include "interface1.h"
#include "spec48.h"
#include "spectra/spectra.h"
#include "HW_.h"
#include "main_.h"
#include "Artifacts_.h"

extern void HWSetMachine(int machine, int speccy);

extern BYTE SpecMem[(128+64)*1024];  //enough memory for 64k ROM + 128k RAM
extern BYTE TimexMem[(64+64)*1024];  // Timex has two more blocks of 64k each
extern BYTE TimexWritable[16];

extern int TIMEXByte, TIMEXMode, TIMEXColour;
extern int TIMEXPage, TIMEXBank;

extern int SPECMICState, SPECTopBorder, SPECLeftBorder, SPECBorder, FloatingBus;
extern int SPECBlk[4], SPECVideoBank, SPECBankEnable, ContendCounter;
extern int SPECKb, SPECNextBorder;
extern int SPECVSync;
extern int fts;

extern BYTE SPECLast1ffd;

int LoadDock(_TCHAR *Filename)
{
        FILE *f;
        int i,bank, chunks[8];
		BYTE *ptr = NULL;

        if (spectrum.model!=SPECCYTC2048 && spectrum.model!=SPECCYTS2068 && spectrum.model!=SPECCYTC2068)
                return(0);

		if (!_tcslen(Filename))
        {
                for(i=0;i<((64+64)*1024);i++) TimexMem[i]=255;
                emulator.ROMDock[0]='\0';
                return(1);
        }

		f=_tfopen(Filename, _TEXT("rb"));
        if (!f) return(0);

        bank=fgetc(f);
        if (feof(f)) return 0;

        if (bank > 0 && bank < 254) return 0;

        for(i=0;i<8;i++)
        {
                int b = fgetc(f);
                if (feof(f)) return 0;
                chunks[i]=b;
                if ((bank==0) && (chunks[i]&1))
                        TimexWritable[i]=1;
                if ((bank==254) && (chunks[i]&1))
                        TimexWritable[i+8]=1;
        }

        for(i=0;i<8;i++)
        {
				if (bank==0) ptr=TimexMem;  // Dock chunk
				else if (bank==254) ptr=TimexMem+65536;  //ExROM chunk
                else if (bank==255) ptr=SpecMem;  // Home chunk

                if (ptr == NULL) return 0;
                
                ptr += i*8192;
                if (chunks[i]&2) fread(ptr,1,8192,f);
        }
        fclose(f);
        _tcscpy(emulator.ROMDock, Filename);
		return(1);
}

unsigned char *z80expandSpectra(unsigned char *in, int OutAddr, int Count)
{
        while(Count)
        {
                if ((in[0]==0xed) && (in[1]==0xed))
                {
                        int byte, repeat;
                        repeat=in[2];
                        byte=in[3];
                        while(repeat)
                        {
                                SpectraMem[OutAddr] = byte;
                                OutAddr++;
                                repeat--;
                                Count--;
                        }
                        in+=4;
                }
                else
                {
                        SpectraMem[OutAddr] = *in;
                        in++;
                        OutAddr++;
                        Count--;
                }
        }

        return(in);
}

unsigned char *z80expand(unsigned char *in, int OutAddr, int Count)
{
        while(Count)
        {
                if ((in[0]==0xed) && (in[1]==0xed))
                {
                        int byte, repeat;
                        repeat=in[2];
                        byte=in[3];
                        while(repeat)
                        {
                                spec48_setbyte(OutAddr, byte);
                                OutAddr++;
                                repeat--;
                                Count--;
                        }
                        in+=4;
                }
                else
                {
                        spec48_setbyte(OutAddr, *in);
                        in++;
                        OutAddr++;
                        Count--;
                }
        }

        return(in);
}
  
void spec_load_z80(char *fname)
{
        FILE *f;
        int i, FileLen, compressed;
        unsigned char *buf;
        int z80version;
        int speccy;

        InitialiseSpectra();
        HW->ColourBox->ItemIndex = COLOURDISABLED;
        Form1->SpectraColourEnable->Checked = false;
        Form1->SpectraColourEnable->Enabled = false;
        Form1->SpectraColourEnable->Visible = false;

        f=fopen(fname, "rb");
        if (!f) return;

        buf=(BYTE*)malloc(311335);
        FileLen=fread(buf,1,311335, f);
        fclose(f);

        z80version=1;
        if (buf[12]==255) buf[12]=1;
        if ((buf[6]==0) && (buf[7]==0))
        {
                if (buf[30]==23) z80version=2;
                else z80version=3;
        }

        speccy=SPECCY48;

        switch(z80version)
        {
        case 1: speccy=SPECCY48; break;

        case 2:
                switch(buf[34])
                {
                case 0: speccy=SPECCY48; break;
                case 1: speccy=SPECCY48; break;
                case 3: speccy=SPECCY128; break;
                case 4: speccy=SPECCY128; break;
                case 7: speccy=SPECCYPLUS2A; break;
                case 8: speccy=SPECCYPLUS2A; break;
                case 9: speccy=SPECCY128; break;
                case 10: speccy=SPECCY128; break;
                case 11: speccy=SPECCY128; break;
                case 12: speccy=SPECCYPLUS2; break;
                case 13: speccy=SPECCYPLUS2A; break;
                case 14: speccy=SPECCYTC2048; break;
                case 15: speccy=SPECCYTC2068; break;
                case 128: speccy=SPECCYTS2068; break;
                default: speccy=SPECCY128; break;
                }
                break;

        case 3:
                switch(buf[34])
                {
                case 0: speccy=SPECCY48; break;
                case 1: speccy=SPECCY48; break;
                case 3: speccy=SPECCY48; break;
                case 4: speccy=SPECCY128; break;
                case 5: speccy=SPECCY128; break;
                case 6: speccy=SPECCY128; break;
                case 7: speccy=SPECCYPLUS2A; break;
                case 8: speccy=SPECCYPLUS2A; break;
                case 9: speccy=SPECCY128; break;
                case 10: speccy=SPECCY128; break;
                case 11: speccy=SPECCY128; break;
                case 12: speccy=SPECCYPLUS2; break;
                case 13: speccy=SPECCYPLUS2A; break;
                case 14: speccy=SPECCYTC2048; break;
                case 15: speccy=SPECCYTC2068; break;
                case 128: speccy=SPECCYTS2068; break;
                default: speccy=SPECCY128; break;
                }
                break;
        }
        if ((z80version>1) && (buf[37]&128))
        {
                switch(speccy)
                {
                case SPECCY48: speccy=SPECCY16; break;
                case SPECCY128: speccy=SPECCYPLUS2; break;
                case SPECCYPLUS3: speccy=SPECCYPLUS2A; break;
                }
        }
        HWSetMachine(MACHINESPECTRUM, speccy);
        machine.initialise();

        z80.af.b.h = buf[0]; z80.af.b.l = buf[1];
        z80.bc.w = buf[2] + 256*buf[3];
        z80.hl.w = buf[4] + 256*buf[5];
        z80.pc.w = buf[6] + 256*buf[7];
        z80.sp.w = buf[8] + 256*buf[9];
        z80.i = buf[10];
        z80.r = buf[11];
        z80.de.w = buf[13] + 256*buf[14];
        z80.bc_.w = buf[15] + 256*buf[16];
        z80.de_.w = buf[17] + 256*buf[18];
        z80.hl_.w = buf[19] + 256*buf[20];
        z80.af_.b.h = buf[21];
        z80.af_.b.l = buf[22];
        z80.iy.w = buf[23] + 256*buf[24];
        z80.ix.w = buf[25] + 256*buf[26];
        z80.iff1 = buf[27];
        z80.iff2 = buf[28];
        z80.im = buf[29]&3;
        z80.r7=(buf[12]&1)<<7;
        SPECBorder=(buf[12]>>1)&7;
        SPECNextBorder=SPECBorder;
        compressed=buf[12]&32;

        if (z80version==1)
        {
                if (compressed) z80expand(buf+30, 16384, 49152);
                else for(i=0;i<49152;i++) spec48_setbyte(i+16384, buf[i+30]);

                free(buf);
                return;
        }
        else
        {
                unsigned char *ptr;
                int page, len;
              
                ptr = buf+32;
                int h2len = buf[30] + (buf[31] << 8);
                ptr += h2len;

                while(ptr < (buf+FileLen))
                {
                        len=ptr[0] + 256*ptr[1];
                        page=ptr[2];

                        ptr += 3;

                        if (page == 13)
                        {
                                ptr=z80expandSpectra(ptr, 0, 16384);
                        }
                        else if (page == 14)
                        {
                                ptr=z80expandSpectra(ptr, 0x4000, 16384);
                        }
                        else
                        {
                                if (speccy==SPECCY16 || speccy==SPECCY48 || speccy==SPECCYTC2048 || speccy==SPECCYTS2068 || speccy==SPECCYTC2068)
                                {
                                        switch(page)
                                        {
                                        case 4: page=2; break;
                                        case 5: page=0; break;
                                        case 8: page=5; break;
                                        default: page=6; break;
                                        }
                                }
                                else    page -= 3;

                                if (len==0xffff)
                                {
                                        for(i=0;i<16384;i++)
                                                RAMWrite((page+4), i, ptr[i]);
                                        ptr+=16384;
                                }
                                else
                                {
                                        SPECBlk[0]=page+4;
                                        ptr=z80expand(ptr, 0, 16384);
                                }
                                SPECBlk[0]=0;
                        }
                }
        }

        int page = (spectrum.model >= SPECCY128) ? 11 : 9;
        
        for(i=0; i<16384; i++)
        {
                SpectraMem[i] = SpecMem[(9<<14) + i]; // Page 5
                SpectraMem[0x4000 + i] = SpecMem[(page<<14) + i]; // Page 5 or 7
        }

        if (z80version == 3)
        {
                SPECLast1ffd = buf[86];

                bool spectraPresent = (buf[37] & 0x08);
                if (spectraPresent)
                {
                        HW->ColourBox->ItemIndex = 1;
                        machine.colour = COLOURSPECTRA;

                        spectrum.spectraColourSwitchOn = buf[87] & 0x01;
                        spectrum.spectraMode = buf[88];
                        SPECBorder = buf[89];
                        SPECNextBorder = SPECBorder;
                        SPECKb = SPECBorder;
                        
                        DetermineSpectraDisplayBank();

                        Form1->SpectraColourEnable->Visible = true;
                        Form1->SpectraColourEnable->Enabled = true;
                        Form1->SpectraColourEnable->Checked = spectrum.spectraColourSwitchOn;
                }
                else
                {
                        HW->ColourBox->ItemIndex = 0;
                        machine.colour = COLOURSINCLAIR;

                        spectrum.spectraColourSwitchOn = false;
                        spectrum.spectraMode = 0x00;
                }
        }

        Artifacts->SelectRGBOutput(machine.colour == COLOURSPECTRA);
        Artifacts->Vibrant->Checked = !spectrum.spectraColourSwitchOn;
        if (machine.colour == COLOURSPECTRA)
        {
                Form1->DisplayArt->Checked = false;
                Artifacts->Visible = false;
        }
        Form1->DisplayArt->Enabled = (machine.colour != COLOURSPECTRA);

        z80.pc.w=buf[32]+ 256*buf[33];
        if (speccy==SPECCYTC2048 || speccy==SPECCYTS2068 || speccy==SPECCYTC2068)
                spec48_writeport(0xff,buf[36], &i);
        spec48_writeport(0x7ffd,buf[35], &i);

        fts=17471 - (buf[55] | (buf[56]<<8));
        fts = (((buf[57]+1)&3)<<16) | fts;
        fts=0;

        //fts=buf[55] | (buf[56]<<8) | (buf[57]<<16);

        free(buf);
}         

void spec_load_sna(char *fname)
{
        FILE *f;
        int i,j,k, len;
        unsigned char *buf;
        int banks[8];

        for(i=0;i<8;i++) banks[i]=0;
        banks[5]=1;
        banks[2]=1;

        f=fopen(fname, "rb");
        if (!f) return;

        buf=(BYTE*)malloc(147487);
        len=fread(buf,1,147487, f);
        fclose(f);

        if (len>49179) HWSetMachine(MACHINESPECTRUM, SPECCY128);
        else HWSetMachine(MACHINESPECTRUM, SPECCY48);
        machine.initialise();

        z80.i = buf[0];
        z80.hl_.w=buf[1] + 256*buf[2];
        z80.de_.w=buf[3] + 256*buf[4];
        z80.bc_.w=buf[5] + 256*buf[6];
        z80.af_.w=buf[7] + 256*buf[8];

        z80.hl.w=buf[9] + 256*buf[10];
        z80.de.w=buf[11] + 256*buf[12];
        z80.bc.w=buf[13] + 256*buf[14];
        z80.iy.w=buf[15] + 256*buf[16];
        z80.ix.w=buf[17] + 256*buf[18];

        z80.iff2=buf[19]&4;
        z80.iff1=z80.iff2;

        z80.r=buf[20]; z80.r7=z80.r&128;
        z80.af.w=buf[21] + 256*buf[22];
        z80.sp.w=buf[23] + 256*buf[24];
        z80.im=buf[25];
        SPECBorder=buf[26];
        SPECNextBorder=SPECBorder;

        if (len>49179)
        {
                i=buf[49181];
                spec48_writeport(0x7ffd,i,&j);
                banks[i&7]=1;
        }

        for(i=0;i<49152;i++) spec48_setbyte(i+16384, buf[i+27]);

        if (len==49179)
        {
                z80.pc.w=spec48_getbyte(z80.sp.w++);
                z80.pc.w += 256*spec48_getbyte(z80.sp.w++);
                free(buf);
                return;
        }

        z80.pc.w=buf[49179] + 256*buf[49180];

        k=49183;
        for(i=0;i<8;i++)
                if (!banks[i])
                        for(j=0;j<16384;j++) RAMWrite((i+4), j, buf[k++]);

        int page = (spectrum.model >= SPECCY128) ? 11 : 9;
        
        for(i=0; i<16384; i++)
        {
                SpectraMem[i] = SpecMem[(9<<14) + i];
                SpectraMem[0x4000 + i] = SpecMem[(page<<14) + i]; // Page 5 or 7
        }

        free(buf);
}

void spec_save_sna(char *fname)
{
        FILE *f;
        int i,j;
        int banks[8];

        f=fopen(fname,"wb");
        if (!f) return;

        if (!(spectrum.model==SPECCY128
                || spectrum.model==SPECCYPLUS2
                || spectrum.model==SPECCYPLUS2A
                || spectrum.model==SPECCYPLUS3))
        {
                z80.sp.w -= 2;
                spec48_setbyte(z80.sp.w, z80.pc.b.l);
                spec48_setbyte(z80.sp.w+1, z80.pc.b.h);
        }

        fputc(z80.i, f);
        fputc(z80.hl_.b.l,f); fputc(z80.hl_.b.h,f);
        fputc(z80.de_.b.l,f); fputc(z80.de_.b.h,f);
        fputc(z80.bc_.b.l,f); fputc(z80.bc_.b.h,f);
        fputc(z80.af_.b.l,f); fputc(z80.af_.b.h,f);

        fputc(z80.hl.b.l,f); fputc(z80.hl.b.h,f);
        fputc(z80.de.b.l,f); fputc(z80.de.b.h,f);
        fputc(z80.bc.b.l,f); fputc(z80.bc.b.h,f);
        fputc(z80.iy.b.l,f); fputc(z80.iy.b.h,f);
        fputc(z80.ix.b.l,f); fputc(z80.ix.b.h,f);

        i=0;
        if (z80.iff2) i |= 4;
        fputc(i,f);

        i=z80.r;
        if (z80.r7) i|=128;
        fputc(i,f);

        fputc(z80.af.b.l,f); fputc(z80.af.b.h,f);
        fputc(z80.sp.b.l,f); fputc(z80.sp.b.h,f);

        fputc(z80.im,f);
        fputc((SPECBorder & 0x07),f);

        for(i=0;i<49152;i++) fputc(spec48_getbyte(16384+i),f);

        if (spectrum.model==SPECCY128
                || spectrum.model==SPECCYPLUS2
                || spectrum.model==SPECCYPLUS2A
                || spectrum.model==SPECCYPLUS3)
        {
                fputc(z80.pc.b.l,f); fputc(z80.pc.b.h,f);
                fputc(SPECLast7ffd,f);
                fputc(0,f);

                for(i=0;i<8;i++) banks[i]=0;
                banks[5]=1;
                banks[2]=1;
                banks[SPECBlk[3]-4]=1;

                for(i=0;i<8;i++)
                        if (!banks[i])
                                for(j=0;j<16384;j++) fputc(RAMRead((i+4),j),f);
        }
        else    z80.sp.w += 2;
        fclose(f);
}

void z80_save_block(FILE *f, int bank, int z80block)
{
        int len, run, byte;
        int lastrun, lastbyte;
        unsigned char buf[65536];
        int i;

        i=0;
        len=0;

        lastbyte=lastrun=-1;

        while(i<16384)
        {
                byte=RAMRead(bank,i);
                run=1;
                while((RAMRead(bank,i+run)==byte)
                        && ((i+run)<16384)) run++;

                if ((lastbyte==0xed) && (lastrun==1)) run=1;
                if ((run>4) || (byte==0xed && run>1))
                {
                        if (run>255) run=255;
                        buf[len++]=0xed;
                        buf[len++]=0xed;
                        buf[len++]=run;
                        buf[len++]=byte;
                }
                else
                {
                        buf[len++]=byte;
                        run=1;
                }

                i+=run;
                lastbyte=byte;
                lastrun=run;
        }

        fputc(len&255,f);
        fputc((len>>8)&255,f);
        fputc(z80block,f);
        fwrite(buf, 1, len, f);
}

int SpectraRAMRead(int bank, int addr)
{
        return SpectraMem[bank*16384 + addr];
}

void z80_save_spectra_block(FILE *f, int bank, int z80block)
{
        int len, run, byte;
        int lastrun, lastbyte;
        unsigned char buf[65536];
        int i;

        i=0;
        len=0;

        lastbyte=lastrun=-1;

        while(i<16384)
        {
                byte=SpectraRAMRead(bank,i);
                run=1;
                while((SpectraRAMRead(bank,i+run)==byte)
                        && ((i+run)<16384)) run++;

                if ((lastbyte==0xed) && (lastrun==1)) run=1;
                if ((run>4) || (byte==0xed && run>1))
                {
                        if (run>255) run=255;
                        buf[len++]=0xed;
                        buf[len++]=0xed;
                        buf[len++]=run;
                        buf[len++]=byte;
                }
                else
                {
                        buf[len++]=byte;
                        run=1;
                }

                i+=run;
                lastbyte=byte;
                lastrun=run;
        }

        fputc(len&255,f);
        fputc((len>>8)&255,f);
        fputc(z80block,f);
        fwrite(buf, 1, len, f);
}

void spec_save_z80(char *fname)
{
        FILE *f;
        int i;
        int mode, flags;

        f=fopen(fname,"wb");
        if (!f) return;

        fputc(z80.af.b.h,f);
        fputc(z80.af.b.l,f);
        fputc(z80.bc.b.l,f); fputc(z80.bc.b.h,f);
        fputc(z80.hl.b.l,f); fputc(z80.hl.b.h,f);
        fputc(0,f); fputc(0,f);
        fputc(z80.sp.b.l,f); fputc(z80.sp.b.h,f);
        fputc(z80.i,f);
        fputc(z80.r,f);

        i=0;
        if (z80.r7) i |= 1;
        i |= ((SPECBorder & 0x07)<<1);
        fputc(i,f);
        fputc(z80.de.b.l,f); fputc(z80.de.b.h,f);
        fputc(z80.bc_.b.l,f); fputc(z80.bc_.b.h,f);
        fputc(z80.de_.b.l,f); fputc(z80.de_.b.h,f);
        fputc(z80.hl_.b.l,f); fputc(z80.hl_.b.h,f);
        fputc(z80.af_.b.h,f);
        fputc(z80.af_.b.l,f);
        fputc(z80.iy.b.l,f); fputc(z80.iy.b.h,f);
        fputc(z80.ix.b.l,f); fputc(z80.ix.b.h,f);
        fputc(z80.iff1,f);
        fputc(z80.iff2,f);

        i=z80.im;
        if (spectrum.kbissue==SPECKBISS2) i|=4;
        fputc(i,f);

        // We're going to do a version 2 or 3 .z80 file

        int h2len = (machine.colour == COLOURSPECTRA) ? 58 : 23;
        fputc(h2len,f); fputc(0,f);
        fputc(z80.pc.b.l,f); fputc(z80.pc.b.h,f);

        switch(spectrum.model)
        {
        case SPECCY16: mode=0; flags=128; break;
        case SPECCY48: mode=0; flags=0; break;
        case SPECCYTC2048: mode=14; flags=0; break;
        case SPECCYTC2068: mode=15; flags=0; break;
        case SPECCYTS2068: mode=128; flags=0; break;
        case SPECCY128: mode=3; flags=0; break;
        case SPECCYPLUS2: mode=12; flags=0; break;
        case SPECCYPLUS2A: mode=13; flags=0; break;
        case SPECCYPLUS3: mode=7; flags=0; break;
        }

        if (spectrum.floppytype==FLOPPYIF1) mode++;

        fputc(mode,f);
        if (spectrum.model==SPECCY128
                || spectrum.model==SPECCYPLUS2
                || spectrum.model==SPECCYPLUS2A
                || spectrum.model==SPECCYPLUS3)
                        fputc(SPECLast7ffd,f);
        else    fputc(0,f);

        if (spectrum.model==SPECCYTC2048 || spectrum.model==SPECCYTS2068 || spectrum.model==SPECCYTC2068)
                fputc(TIMEXByte,f);
        else    fputc(0,f);

        /*
        Bit 0: 1 if R register emulation on
	Bit 1: 1 if LDIR emulation on
	Bit 2: AY sound in use, even on 48K machines
	Bit 3: SPECTRA interface present
	Bit 6: (if bit 2 set) Fuller Audio Box emulation
	Bit 7: Modify hardware
        */
        flags |= 1 | 2;
        if (machine.colour == COLOURSPECTRA) flags |= 8;
        fputc(flags,f);

        fputc(0,f);
        for(i=0;i<16;i++) fputc(0,f);

        // If Spectra is enabled then output a version 3 file
        if (machine.colour == COLOURSPECTRA)
        {
                for (i=55; i<86; i++)
                {
                        fputc(0,f);
                }
                
                fputc(SPECLast1ffd, f);

                /*
                Spectra extension devised for zxsp emulator.                     
                Bit 0: new colour modes enabled
		Bit 1: RS232 enabled
		Bit 2: Joystick enabled
		Bit 3: IF1 rom hooks enabled
		Bit 4: rom paged in
		Bit 5: port 239: Comms out bit
		Bit 6: port 239: CTS out bit
		Bit 7: port 247: Data out bit
                */

                // Only colour mode supported at present
                BYTE spectraBits = spectrum.spectraColourSwitchOn ? 1 : 0;
                fputc(spectraBits, f);

                /*
                Spectra extension devised for zxsp emulator.
                Last out to port 7FDF
                */
                fputc(spectrum.spectraMode, f);

                fputc(SPECBorder, f);
        }

        if (spectrum.model==SPECCY16)
                z80_save_block(f,SPECBlk[1], 8);
        else if (spectrum.model==SPECCY48 || spectrum.model==SPECCYPLUS
                || spectrum.model==SPECCYTC2048 || spectrum.model==SPECCYTS2068 || spectrum.model==SPECCYTC2068)
        {
                z80_save_block(f,SPECBlk[1], 8);
                z80_save_block(f,SPECBlk[2], 4);
                z80_save_block(f,SPECBlk[3], 5);
        }
        else
        {
                for(i=0;i<8;i++)
                        z80_save_block(f,i+4, i+3);
        }

        if (machine.colour == COLOURSPECTRA)
        {
                /*
                Spectra extension devised for zxsp emulator.
                
        	pg no.	memory bank
	        ------	-----------
        	12		SPECTRA rom
	        13		SPECTRA ram[0]
        	14		SPECTRA ram[1]
                */

                // Only RAM supported at present
                z80_save_spectra_block(f, 0, 13);
                z80_save_spectra_block(f, 1, 14);
        }

        fclose(f);
}


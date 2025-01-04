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
 * tzxload.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#include <stdio.h>
#pragma hdrstop

#include "tzxfile.h"
#include "utils.h"
//---------------------------------------------------------------------------

#pragma package(smart_init)

char TZX_ID[]="ZXTape!\032";

unsigned char TTZXFile::ReadByte(FILE *f)
{
        unsigned char a=0;
        fread(&a, 1,1, f);
        return(a);
}

unsigned short TTZXFile::ReadWord(FILE *f)
{
        unsigned short a,b;
        a=ReadByte(f);
        b=ReadByte(f);
        return (unsigned short)(a + b*256);
}

unsigned int TTZXFile::ReadDWord(FILE *f)
{
        unsigned int a,b;
        a=ReadWord(f);
        b=ReadWord(f);
        return(a + b*65536);
}

unsigned int TTZXFile::Read3Bytes(FILE *f)
{
        unsigned int a,b,c;
        a=ReadByte(f);
        b=ReadByte(f);
        c=ReadByte(f);
        return(a + b*256 + c*65536);
}

void TTZXFile::ReadBytes(FILE *f, int len, void *buf)
{
        fread(buf, 1, len, f);
}

bool TTZXFile::LoadOldGeneralBlock(FILE *f)
{
        int bl, flags, pl, pp, ns, np, as, pause;
        int datalen;
        unsigned short *sp, *at;
        char *data;
        int i;

        unsigned short SymDef[]=
        { 3, 530, 520, 530, 520, 530, 520, 530, 4689,
                0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
          3, 530, 520, 530, 520, 530, 520, 530, 520, 530,
          520, 530, 520, 530, 520, 530, 520, 530, 4689 }, *SymDefD;


        SymDefD=(unsigned short *)malloc(2*19*sizeof(short));
        memcpy(SymDefD, SymDef, 2*19*sizeof(short));

        bl=ReadDWord(f);
        flags=ReadByte(f);
        pl=ReadWord(f);
        pp=ReadWord(f);
        ns=ReadByte(f);

        if ((flags!=0) && (flags!=1)) { free(SymDefD); return(true); }
        if (pl!=0) { free(SymDefD); return(true); }
        if (pp!=0) { free(SymDefD); return(true); }
        if (ns!=0) { free(SymDefD); return(true); }

        if (ns)
        {
                sp=(unsigned short *) malloc(ns * sizeof(short));
                for(i=0;i<pp;i++) sp[i]=ReadWord(f);
        } else  sp=NULL;

        np=ReadByte(f);
        as=ReadByte(f);

        if (as!=2) { free(SymDefD); if (sp) free(sp); return(true); }

        at=(unsigned short *) malloc(np * as * sizeof(short));
        for(i=0; i< (np*as); i++) at[i]=ReadWord(f);

        //usedbits=ReadByte(f);
        ReadByte(f);

        pause=ReadWord(f);

        datalen=bl-(11+2*(ns+np*as));
        data=(char *) malloc(datalen);
        ReadBytes(f, datalen, data);

        Tape[CurBlock].BlockID=TZX_BLOCK_GENERAL;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.General.TOTP=0;
        Tape[CurBlock].Head.General.NPP=0;
        Tape[CurBlock].Head.General.ASP=0;
        Tape[CurBlock].Head.General.TOTD=datalen*8;
        Tape[CurBlock].Head.General.NPD=19;
        Tape[CurBlock].Head.General.ASD=2;
        Tape[CurBlock].Head.General.DataLen=datalen;

        Tape[CurBlock].SymDefD=SymDefD;
        Tape[CurBlock].Data.Data=(unsigned char*)data;
        Tape[CurBlock].SymDefP=NULL;
        Tape[CurBlock].PRLE=NULL;

        free(sp);
        free(at);

        return(false);
}

bool TTZXFile::LoadGeneralBlock(FILE *f)
{
        unsigned short *SymDefP, *SymDefD, *PRLE;
        unsigned char *Data;
        long Pause;
        int TOTP, NPP, ASP, TOTD, NPD,ASD;
        int bits, bytes;
        int i,j,k;

        long pos;

        pos=ftell(f);

        if (!LoadOldGeneralBlock(f)) return(false);

        fseek(f,pos,SEEK_SET);

        //DataLen=ReadDWord(f);
        ReadDWord(f);
        
        Pause=ReadWord(f);
        TOTP=ReadDWord(f);
        NPP=ReadByte(f)+1;
        ASP=ReadByte(f); if (ASP==0 && TOTP>0) ASP=256;

        if (TOTP==0) { NPP=0; ASP=0; }

        TOTD=ReadDWord(f);
        NPD=ReadByte(f)+1;
        ASD=ReadByte(f); if (ASD==0 && TOTP>0) ASD=256;

        if (TOTD==0) { NPD=0; ASD=0; }

        if (TOTP>0)
        {
                SymDefP=(unsigned short *)malloc(ASP*(NPP*sizeof(short)));
                PRLE=(unsigned short *)malloc(2*TOTP*sizeof(short));

                for(i=0;i<(ASP);i++)
                {
                        SymDefP[i*NPP]=ReadByte(f);
                        for(j=0;j<(NPP-1);j++)
                        {
                                k=ReadWord(f);
                                SymDefP[i*NPP+j+1]=(unsigned short)k;
                                //if (k==0) j=NPP;
                        }
                }
                for(i=0;i<TOTP;i++)
                {
                        PRLE[i*2]=ReadByte(f);
                        PRLE[i*2+1]=ReadWord(f);
                }
        }
        else
        {
                SymDefP=NULL;
                PRLE=NULL;
        }

        if (TOTD>0)
        {
                i=1;
                bits=0;
                while(i<ASD)
                {
                        i<<=1;
                        bits++;
                }

                bits = bits*TOTD;
                bytes = bits/8;
                if ((bytes*8)<bits) bytes++;

                SymDefD=(unsigned short *)malloc(ASD*(NPD*sizeof(short)));

                for(i=0;i<(ASD);i++)
                {
                        SymDefD[i*NPD]=ReadByte(f);
                        for(j=0;j<(NPD-1);j++)
                        {
                                k=ReadWord(f);
                                SymDefD[i*NPD+j+1]=(unsigned short)k;
                                //if (k==0) j=NPP;
                        }
                }


                Data=(unsigned char *)malloc(bytes);
                ReadBytes(f,bytes, Data);
        }
        else
        {
            Data=NULL;
        }

        Tape[CurBlock].BlockID=TZX_BLOCK_GENERAL;
        Tape[CurBlock].Pause=Pause;
        Tape[CurBlock].Head.General.TOTP=TOTP;
        Tape[CurBlock].Head.General.NPP=NPP;
        Tape[CurBlock].Head.General.ASP=ASP;
        Tape[CurBlock].Head.General.TOTD=TOTD;
        Tape[CurBlock].Head.General.NPD=NPD;
        Tape[CurBlock].Head.General.ASD=ASD;
        Tape[CurBlock].Head.General.DataLen=bytes;

        Tape[CurBlock].Data.Data=Data;
        Tape[CurBlock].SymDefP=SymDefP;
        Tape[CurBlock].SymDefD=SymDefD;
        Tape[CurBlock].PRLE=PRLE;

        return(false);
}


bool TTZXFile::LoadROMBlock(FILE *f)
{
        int length;
        int pause;
        unsigned char *data;

        pause=ReadWord(f);
        length=ReadWord(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_ROM;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.ROM.DataLen=(unsigned short)length;
        Tape[CurBlock].Pause=pause;

        return(false);
}

bool TTZXFile::LoadTurboBlock(FILE *f)
{
        int datalen, lp,ls1,ls2,l0,l1,lpt, usedbits,pause;
        unsigned char *data;

        lp=ReadWord(f);
        ls1=ReadWord(f);
        ls2=ReadWord(f);
        l0=ReadWord(f);
        l1=ReadWord(f);
        lpt=ReadWord(f);
        usedbits=ReadByte(f);
        pause=ReadWord(f);
        datalen=Read3Bytes(f);

        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_TURBO;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Turbo.PilotLen=(unsigned short)lp;
        Tape[CurBlock].Head.Turbo.Sync1Len=(unsigned short)ls1;
        Tape[CurBlock].Head.Turbo.Sync2Len=(unsigned short)ls2;
        Tape[CurBlock].Head.Turbo.Bit0Len=(unsigned short)l0;
        Tape[CurBlock].Head.Turbo.Bit1Len=(unsigned short)l1;
        Tape[CurBlock].Head.Turbo.PilotPulses=(unsigned short)lpt;
        Tape[CurBlock].Head.Turbo.FinalBits=(unsigned short)usedbits;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.Turbo.DataLen=datalen;

        return(false);
}
bool TTZXFile::LoadToneBlock(FILE *f)
{
        int pulselen, pulses;

        pulselen=ReadWord(f);
        pulses=ReadWord(f);

        Tape[CurBlock].BlockID=TZX_BLOCK_TONE;
        Tape[CurBlock].Head.Tone.PulseLen=(unsigned short)pulselen;
        Tape[CurBlock].Head.Tone.NoPulses=(unsigned short)pulses;

        return(false);
}
bool TTZXFile::LoadPulseBlock(FILE *f)
{
        int nopulses;
        unsigned short *pulses;

        nopulses=ReadByte(f);
        pulses=(unsigned short *) malloc(nopulses*2);
        ReadBytes(f,nopulses*2,pulses);

        Tape[CurBlock].BlockID=TZX_BLOCK_PULSE;
        Tape[CurBlock].Data.Pulses=pulses;
        Tape[CurBlock].Head.Pulse.NoPulses=(unsigned short)nopulses;

        return(false);
}
bool TTZXFile::LoadDataBlock(FILE *f)
{
        int datalen, len0, len1, usedbits, pause;
        unsigned char *data;

        len0=ReadWord(f);
        len1=ReadWord(f);
        usedbits=ReadByte(f);
        pause=ReadWord(f);
        datalen=Read3Bytes(f);
        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_DATA;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Data.Len0=(unsigned short)len0;
        Tape[CurBlock].Head.Data.Len1=(unsigned short)len1;
        Tape[CurBlock].Head.Data.FinalBits=(unsigned short)usedbits;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.Data.DataLen=datalen;

        return(false);
}
bool TTZXFile::LoadDRecBlock(FILE *f)
{
        int samplelen, pause, usedbits, datalen;
        unsigned char *data;

        samplelen=ReadWord(f);
        pause=ReadWord(f);
        usedbits=ReadByte(f);
        datalen=Read3Bytes(f);

        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_DREC;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.DRec.SampleLen=samplelen;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.DRec.FinalBits=(unsigned short)usedbits;
        Tape[CurBlock].Head.DRec.Samples=datalen;

        return(false);
}
bool TTZXFile::LoadCSWBlock(FILE *f)
{
        int datalen, pause, samplerate, compression, flags, nopulses;
        unsigned char *data;

        datalen=ReadDWord(f)-11;
        pause=ReadWord(f);
        samplerate=Read3Bytes(f);
        compression=ReadByte(f);
        flags=ReadByte(f);
        nopulses=ReadDWord(f);

        data=(unsigned char *) malloc(datalen);
        ReadBytes(f,datalen,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_CSW;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.CSW.BlockLen=datalen;
        Tape[CurBlock].Pause=pause;
        Tape[CurBlock].Head.CSW.SampleRate=samplerate;
        Tape[CurBlock].Head.CSW.Compression=compression;
        Tape[CurBlock].Head.CSW.Flags=flags;
        Tape[CurBlock].Head.CSW.NoPulses=nopulses;

        return(false);
}

bool TTZXFile::LoadPauseBlock(FILE *f)
{
        int pause;

        pause=ReadWord(f);
        Tape[CurBlock].BlockID=TZX_BLOCK_PAUSE;
        Tape[CurBlock].Pause=pause;

        return(false);
}
bool TTZXFile::LoadGStartBlock(FILE *f)
{
        int length;
        unsigned char *data;

        length=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_GSTART;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.GStart.NameLen=(unsigned short)length;

        return(false);
}
bool TTZXFile::LoadGEndBlock(FILE *f)
{
        Tape[CurBlock].BlockID=TZX_BLOCK_GEND;
        return(false);
}
bool TTZXFile::LoadJumpBlock(FILE *f)
{
        int jump;

        jump=ReadWord(f);
        Tape[CurBlock].BlockID=TZX_BLOCK_JUMP;
        Tape[CurBlock].Head.Jump.JumpRel=(unsigned short)jump;

        return(false);
}
bool TTZXFile::LoadLStartBlock(FILE *f)
{
        int repeats;

        repeats=ReadWord(f);

        Tape[CurBlock].BlockID=TZX_BLOCK_LSTART;
        Tape[CurBlock].Head.LStart.Repeats=(unsigned short)repeats;

        return(false);
}
bool TTZXFile::LoadLEndBlock(FILE *f)
{
        Tape[CurBlock].BlockID=TZX_BLOCK_LEND;
        return(false);
}
bool TTZXFile::LoadSBlock(FILE *f)
{
        int length, selections;
        unsigned char *data;

        length=ReadWord(f)-1;
        selections=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_SBLOCK;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.SBlock.BlockLen=(unsigned short)length;
        Tape[CurBlock].Head.SBlock.NoSelections=(unsigned short)selections;

        return(false);
}
bool TTZXFile::LoadStop48KBlock(FILE *f)
{
        ReadDWord(f);
        Tape[CurBlock].BlockID=TZX_BLOCK_STOP48K;

        return(false);
}
bool TTZXFile::LoadSetLevelBlock(FILE *f)
{
        int level;

        ReadDWord(f);
        level=ReadByte(f);

        Tape[CurBlock].BlockID=TZX_BLOCK_SETLEVEL;
        Tape[CurBlock].Head.SetLevel.Level=(unsigned short)level;

        return(false);
}
bool TTZXFile::LoadTextBlock(FILE *f)
{
        int length;
        unsigned char *data;

        length=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_TEXT;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Text.TextLen=(unsigned short)length;

        return(false);
}
bool TTZXFile::LoadMessageBlock(FILE *f)
{
        int length, time;
        unsigned char *data;

        time=ReadByte(f);
        length=ReadByte(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_MESSAGE;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Message.TextLen=(unsigned short)length;
        Tape[CurBlock].Head.Message.Time=(unsigned short)time;

        return(false);
}
bool TTZXFile::LoadArchiveBlock(FILE *f)
{
        int length,strings;
        unsigned char *data;

        length=ReadWord(f)-1;
        data=(unsigned char *) malloc(length);
        strings=ReadByte(f);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=TZX_BLOCK_ARCHIVE;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Archive.BlockLen=(unsigned short)length;
        Tape[CurBlock].Head.Archive.NoStrings=(unsigned short)strings;

        return(false);
}
bool TTZXFile::LoadHWTypeBlock(FILE *f)
{
        int blocks,i;
        struct TZXHWInfo *data, *p;

        blocks=ReadByte(f);
        data=(struct TZXHWInfo *) malloc(blocks * sizeof(struct TZXHWInfo));
        p=data;

        for(i=0;i<blocks;i++)
        {
                p->Type = ReadByte(f);
                p->ID = ReadByte(f);
                p->Information = ReadByte(f);
                p++;
        }

        Tape[CurBlock].BlockID=TZX_BLOCK_HWTYPE;
        Tape[CurBlock].Data.HWTypes=data;
        Tape[CurBlock].Head.HWType.NoTypes=(unsigned short)blocks;

        return(false);
}
bool TTZXFile::LoadCustomBlock(FILE *f)
{
        char *data, id[17];
        int len;

        ReadBytes(f, 16, id);
        id[16]='\0';

        len=ReadDWord(f);
        data=(char *)malloc(len);
        ReadBytes(f, len, data);

        Tape[CurBlock].BlockID=TZX_BLOCK_CUSTOM;
        memcpy(Tape[CurBlock].Head.Custom.IDString, data, 11);
        Tape[CurBlock].Data.Data=(unsigned char*)data;
        Tape[CurBlock].Head.Custom.Length=len;

        return(false);
}
bool TTZXFile::LoadGlueBlock(FILE *f)
{
        Tape[CurBlock].BlockID=TZX_BLOCK_GLUE;
        ReadDWord(f);
        ReadDWord(f);
        return(false);
}
bool TTZXFile::LoadUnknownBlock(FILE *f, int BlockID)
{
        int length;
        unsigned char *data;

        length=ReadDWord(f);
        data=(unsigned char *) malloc(length);
        ReadBytes(f,length,data);

        Tape[CurBlock].BlockID=0;
        Tape[CurBlock].Data.Data=data;
        Tape[CurBlock].Head.Unknown.type=(unsigned short)BlockID;
        Tape[CurBlock].Head.Unknown.length=length;

        return(false);
}

bool TTZXFile::LoadTAPFile(ZXString FileName, bool Insert)
{
        FILE *f;
        int error, i;
        int HeaderLen;
        int len;
        bool FirstBlock, AddSync, AddChecksum;
        unsigned char data[65536];

        f=_tfopen(FileName.c_str(), _TEXT("rb"));
        if (!f) return(false);
        this->FileName=FileName;

        FirstBlock=true;
        AddSync=0; AddChecksum=0;

        if (!Insert) EraseAll();
        error=false;

        while(!feof(f) && !error)
        {
                len=ReadWord(f);

                if (FirstBlock)
                {
                        if (len==26) AddSync=1;
                        if (len==25) { AddSync=1; AddChecksum=1; }
                        HeaderLen=len;
                }
                FirstBlock=false;

                if (len<1 || len>65536) error=true;
                else
                {
                        ReadBytes(f, len, data+AddSync);
                        if (AddSync)
                        {
                                if (len==HeaderLen) data[0]=0;
                                else data[0]=255;
                        }

                        if (AddChecksum)
                        {
                                unsigned char check=0;

                                for(i=0;i<(len);i++)
                                        check = check ^ data[i+AddSync];
                                data[len+AddSync]=check;
                        }

                        len+= AddSync+AddChecksum;

                        MoveBlock(AddROMBlock((char*)data, len), CurBlock);
                        if (AddSync)
                        {
                                if (len==27) Tape[CurBlock].Pause=100;
                                else Tape[CurBlock].Pause=5000;
                        }
                        CurBlock++;
                }
        }

        fclose(f);
        GroupCount();
        return(true);
}

bool TTZXFile::LoadPFile(ZXString FileName, bool Insert)
{
        FILE *f;
        int len, fnamelen;
        char tempdata[65536+256];

        f=_tfopen(FileName.c_str(), _TEXT("rb"));
        if (!f) return(false);
        this->FileName=FileName;

        if (!Insert) NewTZX();

        if (FileNameGetExt(FileName)==".P"
                || FileNameGetExt(FileName)==".81")
        {
                ConvertASCIIZX81(RemoveExt(RemovePath(FileName)), (unsigned char*)tempdata);
                fnamelen=ZX81Strlen((unsigned char*)tempdata);
        }
        else    fnamelen=0;

        len=fread(tempdata+fnamelen, 1, 65536, f);

        ValidateFile(FileName, tempdata, len);

        MoveBlock(AddGeneralBlock(tempdata, len+fnamelen), CurBlock);
        Tape[CurBlock].Pause=3000;

        fclose(f);
        GroupCount();
        return(true);
}

void TTZXFile::LoadFileData(ZXString FileName, unsigned char* programData, int length, bool Insert)
{
        ZXString Extension = FileNameGetExt(FileName);

        if (Extension == ".B80" || ((Extension == ".TXT" || Extension == ".BAS") && emulator.machine == MACHINEZX80))
        {
                LoadOFileData(programData, length, Insert);
        }
        else if (Extension == ".B81" || ((Extension == ".TXT" || Extension == ".BAS") && (emulator.machine == MACHINEZX81 || emulator.machine == MACHINETS1000 || emulator.machine == MACHINETS1500 || emulator.machine == MACHINETK85)))
        {
                LoadPFileData(FileName.c_str(), programData, length, Insert);
        }
        else if (Extension == ".B82" || ((Extension == ".TXT" || Extension == ".BAS") && emulator.machine == MACHINESPECTRUM))
        {
                LoadTapFileData(FileName.c_str(), programData, length, Insert);
        }
}

void TTZXFile::LoadOFileData(unsigned char* programData, int length, bool Insert)
{
        if (!Insert) NewTZX();

        MoveBlock(AddGeneralBlock((char*)programData, length), CurBlock);
        Tape[CurBlock].Pause=3000;

        GroupCount();
}

void TTZXFile::LoadPFileData(ZXString FileName, unsigned char* programData, int length, bool Insert)
{
        if (!Insert) NewTZX();

        int fnamelen;
        char tempdata[65536+256];
        
        ConvertASCIIZX81(RemoveExt(RemovePath(FileName)), (unsigned char*)tempdata);
        fnamelen = ZX81Strlen((unsigned char*)tempdata);

        memcpy(tempdata+fnamelen, (char*)programData, length);

        MoveBlock(AddGeneralBlock(tempdata, length+fnamelen), CurBlock);
        Tape[CurBlock].Pause=3000;

        GroupCount();
}

void TTZXFile::LoadTapFileData(ZXString FileName, unsigned char* programData, int length, bool Insert)
{
        if (!Insert) EraseAll();

        int headerOffset = 0;
        int headerLength = programData[headerOffset] + (programData[headerOffset+1] << 8);
        headerOffset += 2;
        unsigned char* headerStart = programData + headerOffset;
        MoveBlock(AddROMBlock((char*)headerStart, headerLength), CurBlock);
        Tape[CurBlock].Pause=100;
        CurBlock++;

        int dataOffset = headerOffset + headerLength;
        int dataLength = programData[dataOffset] + (programData[dataOffset+1] << 8);
        dataOffset += 2;
        unsigned char* dataStart = programData + dataOffset;
        MoveBlock(AddROMBlock((char*)dataStart, dataLength), CurBlock);
        Tape[CurBlock].Pause=5000;
        CurBlock++;

        GroupCount();
}

void TTZXFile::ValidateFile(ZXString FileName, char* tempdata, int len)
{
        ZXString Extension = FileNameGetExt(FileName);

        if (Extension == ".A83") return;        // TO DO - Add support for this format

        char* program = tempdata;
        int startSystemVariables;
        int elineOffset;
#if __CODEGEARC__ >= 0x0620
        UnicodeString msg;
#else
        AnsiString msg;
#endif

        int length = len;

        if (Extension == ".P" || Extension == ".P81" || Extension == ".81")
        {
                while ((length > 0) && ((unsigned char)(program[0]) & 128) == 0)
                {
                        program++;
                        length--;
                }

                startSystemVariables = 0x4009;
                elineOffset = 0x4014 - startSystemVariables;

                if (length < (elineOffset + 2))
                {
                        msg = "The start of the program data could not be found.";
#if __CODEGEARC__ >= 0x0620
                        Application->MessageBox(msg.c_str(), L"File integrity error", MB_OK | MB_ICONERROR);
#else
                        Application->MessageBox(msg.c_str(), "File integrity error", MB_OK | MB_ICONERROR);
#endif
                        return;
                }

                program++;
                length--;
        }
        else if (Extension == ".O" || Extension == ".80")
        {
                startSystemVariables = 0x4000;
                elineOffset = 0x400A - startSystemVariables;
        }

        unsigned char lowByte = program[elineOffset];
        unsigned char highByte = program[elineOffset + 1];

        int eline = (highByte << 8) + lowByte;
        int expectedLen = eline - startSystemVariables;

        if (expectedLen < length)
        {
                int surplusBytes = length - expectedLen;
                bool includesSurplusBytes = surplusBytes > 1;
                msg = "The file contains " + IntToStr(surplusBytes) + " byte" + (includesSurplusBytes ? "s" : "") + " more than specifed by ELINE. Th" + (includesSurplusBytes ? "ese" : "is") + " will be ignored.";

#if __CODEGEARC__ >= 0x0620
                Application->MessageBox(msg.c_str(), L"File size warning", MB_OK | MB_ICONWARNING);
#else
                Application->MessageBox(msg.c_str(), "File size warning", MB_OK | MB_ICONWARNING);
#endif
        }
}

bool TTZXFile::LoadT81File(ZXString FileName, bool Insert)
{
        // T81 Format:
        // 4 bytes ASCII  - "EO81"
        //
        // Program block:
        // 32 bytes ASCII - Filename padded with '\0'
        // 16 bytes ASCII - Length of bytes that follow as decimal string padded with '\0'
        // ZX81 Filename  - EighyOne assumes absent if first byte is 0, 1 or 255, i.e. must be start of the program itself
        // ZX80/1 Program - Equivalent to .O/.P file data
        //
        // Pause block:
        // 32 bytes ASCII - "<Silence>" padded with '\0'
        // 16 bytes ASCII - Length of silence as decimal string padded with '\0'
        //
        // Custom block:
        // 32 bytes ASCII - "<...>" padded with '\0'
        // 16 bytes ASCII - Length of bytes that follow as a decimal string padded with '\0'
        // n bytes        - Application specific data

        char header[5];
        char fname[33], flen[17];
        unsigned char buffer1[65536+256], buffer2[65535+256];

        FILE *fptr;
        int length, zxnamelen,i;

        fptr=_tfopen(FileName.c_str(), _TEXT("rb"));
        if (!fptr) return(false);
        this->FileName=FileName;

        fread(header, 4, 1, fptr);
        if (strncmp(header, T81_HEADER_ID,4))
        {
                fclose(fptr);
                return(false);
        }

        if (!Insert) NewTZX();

        do
        {
                memset(fname, 0, 33);
                memset(flen, 0, 17);
                fread(fname, 32, 1, fptr);
                fread (flen, 16, 1, fptr);

                length = atoi(flen);

                int fnameLen = strlen(fname);
                if ( (fnameLen>32) || (length < 1) || (length > 65535) )
                        break;

                if ((fname[0] == '<') && fname[fnameLen - 1] == '>')
                {
                        if (!strcmp(fname,"<Silence>"))
                        {
                                MoveBlock(AddPauseBlock(length), CurBlock++);
                        }
                        else
                        {
                                // Skip over the unhandled block
                                fread(buffer1, length, 1, fptr);
                        }
                }
                else
                {
                        fread(buffer1, length, 1, fptr);
                        if ( (*buffer1==0x00) || (*buffer1==255) || (*buffer1==1) ) // If buffer doesn't include the filename, add one
                        {
                                if (*fname != '\0')
                                {
                                        ConvertASCIIZX81(fname, buffer2);
                                        zxnamelen = ZX81Strlen(buffer2);
                                }
                                else
                                {
                                        zxnamelen = 0;
                                }
                        }
                        else    zxnamelen = 0;

                        memcpy(buffer2+zxnamelen, buffer1, length);
                        length += zxnamelen;

                        while(length>0 && buffer2[length-1]!=0x80) length--;

                        MoveBlock(AddGeneralBlock((char*)buffer2, length), CurBlock++);
                }
        } while(!feof(fptr));

        fclose(fptr);
        MergeBlocks();

        for(i=1;i<Blocks;i++)
                if (Tape[i].BlockID==TZX_BLOCK_GENERAL && Tape[i].Pause==0) Tape[i].Pause=5000;

        GroupCount();
        return(true);
}


bool TTZXFile::LoadFile(ZXString FileName, bool Insert)
{
        FILE *f;
        int BlockID, error;
        ZXString Extension;

        struct TZXHeader head;

        Extension=FileNameGetExt(FileName);

        if (Extension == ".TAP") return(LoadTAPFile(FileName, Insert));
        if (Extension == ".P"
                || Extension == ".O"
                || Extension == ".81"
                || Extension == ".P81"
                || Extension == ".80"
                || Extension == ".A83") return(LoadPFile(FileName, Insert));
        if (Extension == ".T81") return(LoadT81File(FileName, Insert));

        f=_tfopen(FileName.c_str(), _TEXT("rb"));
        if (!f) return(false);
        this->FileName=FileName;

        fread(&head, sizeof(head),1, f);
        if (strncmp(head.id, TZX_ID, 8))
        {
                fclose(f);
                return(false);
        }

        if (!Insert) EraseAll();
        error=false;

        while(!feof(f) && !error)
        {
                BlockID=ReadByte(f);

                if (Insert) { InsertBlock(CurBlock); CurBlock--; }
                EraseBlock(CurBlock);

                switch(BlockID)
                {
                case TZX_BLOCK_ROM:      error=LoadROMBlock(f); break;
                case TZX_BLOCK_TURBO:    error=LoadTurboBlock(f); break;
                case TZX_BLOCK_TONE:     error=LoadToneBlock(f); break;
                case TZX_BLOCK_PULSE:    error=LoadPulseBlock(f); break;
                case TZX_BLOCK_DATA:     error=LoadDataBlock(f); break;
                case TZX_BLOCK_DREC:     error=LoadDRecBlock(f); break;
                case TZX_BLOCK_CSW:      error=LoadCSWBlock(f); break;
                case TZX_BLOCK_GENERAL:  error=LoadGeneralBlock(f); break;
                case TZX_BLOCK_PAUSE:    error=LoadPauseBlock(f); break;
                case TZX_BLOCK_GSTART:   error=LoadGStartBlock(f); break;
                case TZX_BLOCK_GEND:     error=LoadGEndBlock(f); break;
                case TZX_BLOCK_JUMP:     error=LoadJumpBlock(f); break;
                case TZX_BLOCK_LSTART:   error=LoadLStartBlock(f); break;
                case TZX_BLOCK_LEND:     error=LoadLEndBlock(f); break;
                case TZX_BLOCK_SBLOCK:   error=LoadSBlock(f); break;
                case TZX_BLOCK_STOP48K:  error=LoadStop48KBlock(f); break;
                case TZX_BLOCK_SETLEVEL: error=LoadSetLevelBlock(f); break;
                case TZX_BLOCK_TEXT:     error=LoadTextBlock(f); break;
                case TZX_BLOCK_MESSAGE:  error=LoadMessageBlock(f); break;
                case TZX_BLOCK_ARCHIVE:  error=LoadArchiveBlock(f); break;
                case TZX_BLOCK_HWTYPE:   error=LoadHWTypeBlock(f); break;
                case TZX_BLOCK_CUSTOM:   error=LoadCustomBlock(f); break;
                case TZX_BLOCK_GLUE:     error=LoadGlueBlock(f); break;
                case 0xf0:
                case 0:                  error=true; break;
                default:                 error=LoadUnknownBlock(f,BlockID); break;
                }

                if (error)
                {
                        if (Insert) DeleteBlock(CurBlock);
                }
                else
                {
                        CurBlock++;
                        if (!Insert) Blocks++;
                }
        }

        fclose(f);
        GroupCount();
        return(true);
}

void TTZXFile::GroupCount(void)
{
        int i;

        int GroupCount=0;

        for(i=0;i<Blocks;i++)
        {
                if (Tape[i].BlockID==TZX_BLOCK_GEND
                        || Tape[i].BlockID==TZX_BLOCK_LEND)
                                GroupCount--;

                Tape[i].Group=GroupCount;

                if (Tape[i].BlockID==TZX_BLOCK_GSTART
                        || Tape[i].BlockID==TZX_BLOCK_LSTART)
                                GroupCount++;
        }
}

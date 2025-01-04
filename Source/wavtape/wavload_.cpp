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
 * wavload_.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "wavload_.h"
#include "main_.h"
#include "utils.h"
#include "zx81config.h"
#include "TZXMan.h"
#include "sound.h"
#include "ZipFile_.h"
#include "BasicLister\BasicLister_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OffBtn"
#pragma resource "*.dfm"
TWavLoad *WavLoad;
TWavFile Wav;

int GLOBALBYTE;
extern int AutoLoadCount;

#define  TSClick 4

bool TWavLoad::IsWav(ZXString FileName)
{
        if (FileNameGetExt(FileName)==".WAV") return(true);
        return(false);
}

void TWavLoad::ClearImage()
{
        TCanvas *Img;
        TRect rect;

        Img=Canvas;

        rect.Top=ImgY; rect.Left=ImgX;
        rect.Right=ImgW; rect.Bottom=ImgH;

        Img->Pen->Color = clWhite;

        Img->FillRect(rect);
        Img->Pen->Color = clRed;
        Img->MoveTo(0,ImgY+ImgH/ImgScale);
        Img->LineTo(ImgW,ImgY+ImgH/ImgScale);
        Img->Pen->Color = clGreen;
        Img->MoveTo(0,ImgY+(ImgH/2)-(Threshold/ImgScale));
        Img->LineTo(ImgW,ImgY+(ImgH/2)-(Threshold/ImgScale));
        DoCaption("");
}

void TWavLoad::UpdateImage()
{
        TCanvas *Img;
        TRect rect;
        long x;
        int Pos,Sample;

        if (!Visible) return;

        Img=Canvas;
        Pos=ScrollBar->Position;

        rect.Top=ImgY; rect.Left=ImgX;
        rect.Right=ImgX+ImgW; rect.Bottom=ImgY+ImgH;

        if (ImgW<1 || ImgH<1) return;

        Img->Pen->Color = clWhite;
        Img->FillRect(rect);

        Img->Pen->Color = clRed;
        Img->MoveTo(ImgX,ImgY+ImgH/2);
        Img->LineTo(ImgX+ImgW-1,ImgY+ImgH/2);
        Img->Pen->Color = clGreen;
        Img->MoveTo(ImgX,ImgY+(ImgH/2)-(Threshold/ImgScale));
        Img->LineTo(ImgX+ImgW-1,ImgY+(ImgH/2)-(Threshold/ImgScale));

        Img->Pen->Color = clBlack;
        Img->MoveTo(ImgX, ImgY+Wav.Sample(Pos,Channel)/ImgScale);
        for (x=0; x<ImgW; x++)
        {
                Sample=Wav.Sample(Pos+x,Channel);

                Img->LineTo(ImgX+x, ImgY+Sample/ImgScale);

                if  (    (((128-Sample) > Threshold) && Threshold>=0)
                      || (((128-Sample) < Threshold) && Threshold<0)  )
                {
                        Img->MoveTo(ImgX+x,ImgY+(ImgH/2)-(Threshold/ImgScale));
                        Img->LineTo(ImgX+x, ImgY+Wav.Sample(Pos+x,Channel)/ImgScale);
                }
        }
        DoCaption("");
}
//---------------------------------------------------------------------------
__fastcall TWavLoad::TWavLoad(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        DragAcceptFiles(Handle, true);

        ImgW=Volume->Left;
        ImgH=ScrollBar->Top;
        ImgX=ImgY=0;
        ImgScale=256.0/ImgH;

        TStatesSample=0;
        TapePos=0;
        CurrentTStates=0;
        ScreenCounter=0;
        Playing=0;
        Threshold=50;
        Channel=0;
        StatusText="Stopped";

        IgnoreZX81=false;

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        NewClick(this);
}
//---------------------------------------------------------------------------

void TWavLoad::LoadFile(ZXString FName)
{
        ZXString Extension;

        DoCaption("Loading");

        Extension=FileNameGetExt(FName);
        if (Extension == ".ZIP")
        {
                FName=ZipFile->ExpandZIP(FName, ZXString(LoadWavDialog->Filter));
                if (FName=="") return;
                Extension = FileNameGetExt(FName);
        }

        Wav.LoadFile(FName);

        FileName=FName;

        TStatesSample=(machine.clockspeed / Wav.SampleRate);//*7)/8;
        TapePos=0;
        CurrentTStates=0;
        Playing=0;
        Channel=0;
        ScrollBar->Max = Wav.NoSamples;
        ScrollBar->Position = 0;

        Caption="Wave Loader - "+FileNameGetFname(FileName)+FileNameGetExt(FileName);

        UpdateImage();
        DoCaption("Stopped");
        RecordBtn->Enabled=Wav.CanRecord();
        if (AutoloadonInsert1->Down) AutoLoadCount=2;
        Form1->InWaveLoaderClick(NULL);
        Form1->OutWaveLoaderClick(NULL);
}
            
void __fastcall TWavLoad::LoadClick(TObject *Sender)
{
        if (LoadWavDialog->Execute()) LoadFile(LoadWavDialog->FileName);
        RecordBtn->Enabled=Wav.CanRecord();
}

//---------------------------------------------------------------------------
void __fastcall TWavLoad::ScrollBarChange(TObject *Sender)
{
        if (!Playing && !Recording)
        {
                TapePos=ScrollBar->Position;
                UpdateImage();
        }
        else ScrollBar->Position=TapePos;
}
//---------------------------------------------------------------------------

bool TWavLoad::GetEarState()
{
        int i;

        if (!Playing) return (0);
        
        i=128-Wav.Sample(TapePos+200,Channel);

        if (    (Threshold>=0 && i>=Threshold)
             || (Threshold<0 && i<Threshold) ) return(1);
        else return(0);
}

void TWavLoad::ClockTick(int TStates, bool ZX81, bool MicState)
{
        if (!Playing && !Recording) return;
        if (IgnoreZX81 && ZX81) return;

        ScreenCounter += TStates;
        CurrentTStates += TStates;

        if (CurrentTStates<TStatesSample) return;

        CurrentTStates -= TStatesSample;
        TapePos++;

        if (Recording)
        {
                static int count=0, level=128, lastbit=0;
                //int curve[12] = { 36, 56, 60,  64, 64, 64,  64, 64, 64,  60, 56, 36 };

                if (SoundOn->Down) Sound.Beeper(MicState, frametstates);

                if (MicState == lastbit)
                        count ++;// (emulator.machine==MACHINEACE)?1:3;
                else
                        count=0;
                //count=0;
                //if (count<12) level = MicState ? 128-curve[count] : 128+curve[count];
                //else level = ((level-128)/3)+128;

                //level = MicState ? 64 : 192;

                const int zx81PulseWidth = 6;
                const int midLevel = 128;
                bool limitPulseWidth = (emulator.machine == MACHINEZX80) || (emulator.machine == MACHINEZX81) ||
                                       (emulator.machine == MACHINETS1500) || (emulator.machine == MACHINETS1000) ||
                                       (emulator.machine == MACHINEZX97LE) || (emulator.machine == MACHINELAMBDA) ||
                                       (emulator.machine == MACHINER470) || (emulator.machine == MACHINETK85);
                if (count < 128)
                {
                        level = MicState ? midLevel-96 : midLevel+96;
                        if (limitPulseWidth && (count > zx81PulseWidth))
                        {
                                level = midLevel;
                        }
                }
                else
                        level = (((level-midLevel)*9)/(10))+midLevel;

                Wav.SetSample(TapePos, level);
                lastbit=MicState;
        }
        else    if (SoundOn->Down) Sound.Beeper(GetEarState(), frametstates);

        if (TapePos>Wav.NoSamples)
        {
                Stop(true);
                return;
        }

        if (ScreenCounter<1083333) return;

        ScreenCounter=0;
        ScrollBar->Max=((int)Wav.NoSamples - ScrollBar->Width) >= 0 ? Wav.NoSamples - ScrollBar->Width : Wav.NoSamples;
        ScrollBar->Position = TapePos;
        UpdateImage();    Application->ProcessMessages();
        if (Playing) DoCaption("Playing");
        else DoCaption("Recording");
}

void TWavLoad::Stop(bool force)
{
        if (!AutoStartStop1->Down && !force) return;
        if (!Playing && !Recording) return;
        StopBtnClick(NULL);
}

void TWavLoad::Start()
{
        if (!AutoStartStop1->Down) return;
        if (Playing) return;

        if (Recording) RecordBtnClick(NULL);
        if (TapePos < Wav.NoSamples) PlayBtnClick(NULL);
}

void TWavLoad::StartRec()
{
        if (!AutoStartStop1->Down) return;
        if (Recording) return;

        if (Playing) StopBtnClick(NULL);
        RecordBtnClick(NULL);
}

void TWavLoad::DoCaption(ZXString Message)
{
        if (Message=="") Message=StatusText;
        else StatusText=Message;

        StatusBar1->Panels->Items[2]->Text=Message;
        if (Wav.SampleRate>0)
        {
                ZXString text;
                text = TapePos / Wav.SampleRate;
                text += " / ";
                text += Wav.NoSamples / Wav.SampleRate;
                text += "s";
                StatusBar1->Panels->Items[1]->Text=text;

                text = Wav.SampleRate / 1000;
                text += "kHz, ";
                text += Wav.Bits;
                text += "bit ";
                if (Wav.Stereo) text += "Stereo";
                else text += "Mono";

                StatusBar1->Panels->Items[0]->Text=text;

        }

        if (FileName=="") Caption="Untitled - Wave";
        else Caption=FileNameGetFname(FileName)+FileNameGetExt(FileName)+" - Wave";
}

void __fastcall TWavLoad::Image1MouseMove(TObject *Sender, TShiftState Shift, int X, int Y)
{
        if (Shift.Contains(ssLeft))
        {
                if (Y<ImgY || Y>(ImgY+ImgH)) return;
                if (X<ImgX || X>(ImgX+ImgW)) return;
                Y -= ImgY;

                Threshold = ((128.0/ImgScale)-Y)*ImgScale;
                UpdateImage();
        }
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::OKBtnClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->WavLoadBtn->Checked=false;
}
//---------------------------------------------------------------------------

void TWavLoad::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("WAVLOAD","Top",Top);
        Left = ini->ReadInteger("WAVLOAD","Left",Left);
        Height = ini->ReadInteger("WAVLOAD","Height",Height);
        Width = ini->ReadInteger("WAVLOAD","Width",Width);

        AutoStartStop1->Down = ini->ReadBool("WAVLOAD","AutoStart", AutoStartStop1->Down);
        AutoloadonInsert1->Down = ini->ReadBool("WAVLOAD","AutoLoad", AutoloadonInsert1->Down);
        SoundOn->Down=ini->ReadBool("WAVLOAD","Sound",SoundOn->Down);
        Threshold = ini->ReadInteger("WAVLOAD","Threshold",Threshold);

        LoadWavDialog->FileName = ini->ReadString("WAVLOAD","Filename",LoadWavDialog->FileName);
        LoadWavDialog->InitialDir = ini->ReadString("WAVLOAD","Dir",LoadWavDialog->InitialDir);

        if (Form1->WavLoadBtn->Checked) Show();
}

void TWavLoad::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("WAVLOAD","Top",Top);
        ini->WriteInteger("WAVLOAD","Left",Left);
        ini->WriteInteger("WAVLOAD","Height",Height);
        ini->WriteInteger("WAVLOAD","Width",Width);

        ini->WriteBool("WAVLOAD","AutoStart", AutoStartStop1->Down);
        ini->WriteBool("WAVLOAD","AutoLoad", AutoloadonInsert1->Down);
        ini->WriteBool("WAVLOAD","Sound",SoundOn->Down);

        ini->WriteInteger("WAVLOAD","Threshold",Threshold);

        ini->WriteString("WAVLOAD","Filename",LoadWavDialog->FileName);
        ini->WriteString("WAVLOAD","Dir",LoadWavDialog->InitialDir);
}

void __fastcall TWavLoad::SaveBtnClick(TObject *Sender)
{
        ZXString Filter = "Windows WAV Files|*.wav";

        if (FileName != "") SaveWavDialog->FileName = RemoveExt(FileName);
        else SaveWavDialog->FileName = RemoveExt(SaveWavDialog->FileName);

        SaveWavDialog->Filter = Filter;
        SaveWavDialog->DefaultExt = "wav";
        SaveWavDialog->FilterIndex=1;

        if (!SaveWavDialog->Execute()) return;

        Wav.SaveFile(SaveWavDialog->FileName);
}
//---------------------------------------------------------------------------
ZXString TWavLoad::RemoveExt(ZXString Fname)
{
        ZXString Ext;
        int len,pos;

        len=Fname.Length();

        if (len<3) return(Fname);

        pos=len;
        while(pos)
        {
                if (Fname[pos]=='.') break;
                pos--;
        }

        Ext = Fname.SubString(pos, 1+len-pos);

        if ( (Ext==".p" || Ext==".P" || Ext==".T81" || Ext==".P81") && pos>1)
                Fname = Fname.SubString(1,pos-1);

        return(Fname);
}

int TWavLoad::GetByte(bool start)
{
	int b,e;

	int data;
	int CF,ZF;
	int ResetCounter=1;

	data=1;
	b=256;
        ClockTick(14, false, false);

	while(--b)
	{
		ClockTick(68, false, false);
                if (!Playing) return(-1);
		if (GetEarState())
		{
			e=0x94;
                        ClockTick(23, false, false);
			do
			{
				if (ResetCounter)
				{
					b=0x1a;
					ResetCounter=0;
                                        ClockTick(7, false, false);
				}
	                        if (!Playing) return(-1);

				e--;
				ClockTick(31, false, false);
				CF= GetEarState();
				ZF= ((e&128)==0);

				if (CF)
                                {
                                        ResetCounter=1;
                                        ClockTick(12, false, false);
                                }
				else
                                {
                                        b--;
                                        ClockTick(20, false, false);
                                }

			} while(CF || b);
                        ClockTick(37, false, false);
			if (ZF && (e>=0x56)) b=256;
                        else
			{
				ClockTick(8, false, false);
				data = (data<<1) | (e<32);
				if (data&256) return(data&255);
			}
		}
	}

	return(-1);
}

void __fastcall TWavLoad::NewClick(TObject *Sender)
{
        Wav.NewFile();

        FileName="";

        TStatesSample=((machine.clockspeed / Wav.SampleRate));
        TapePos=0;
        CurrentTStates=0;
        Playing=0;
        Channel=0;
        ScrollBar->Max = Wav.NoSamples;
        ScrollBar->Position = 0;

        Caption="Wave Loader";

        UpdateImage();
        DoCaption("Stopped");
        RecordBtn->Enabled=Wav.CanRecord();
}
//---------------------------------------------------------------------------
void TWavLoad::EncodeBit(bool bit)
{
        int j,len;

        len = bit ? 8:4;

        while(len--)
        {
                for(j=0;j<(160);j++) ClockTick(TSClick, false, true);
                for(j=0;j<(160);j++) ClockTick(TSClick, false, false);
        }

        for(j=0;j<1280;j++) ClockTick(TSClick, false, true);
}

void TWavLoad::EncodeBlock(char *Data, int Len)
{
        int i;

        IgnoreZX81=true;

        if (!Recording) RecordBtnClick(this);

        if (!Data)
        {
        }
        else
        {
                for(i=0; i<(128*TStatesSample);i++) ClockTick(TSClick,false,false);

                while(Len)
                {
                        int byte;

                        byte = *(Data++);

                        for(i=0;i<8;i++)
                        {
                                EncodeBit(byte&128);
                                byte <<= 1;
                        }

                        Len--;

                        if (Len%64) Application->ProcessMessages();
                }

                for(i=0; i<(128*TStatesSample);i++) ClockTick(TSClick,false,false);
        }

        ScrollBar->Max = Wav.NoSamples;
        ScrollBar->Position = TapePos;
        UpdateImage();

        StopBtnClick(this);
        IgnoreZX81=false;
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::VolumeChange(TObject *Sender)
{
        Wav.Volume=-Volume->Position;
        UpdateImage(); Application->ProcessMessages();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::BiasChange(TObject *Sender)
{
        Wav.Bias=Bias->Position;
        UpdateImage(); Application->ProcessMessages();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::SaveWav1Click(TObject *Sender)
{
        ZXString Filter = "Windows WAV Files|*.wav";

        if (FileName != "") SaveWavDialog->FileName = RemoveExt(FileName);
        else SaveWavDialog->FileName = RemoveExt(SaveWavDialog->FileName);

        SaveWavDialog->Filter = Filter;
        SaveWavDialog->DefaultExt = "wav";
        SaveWavDialog->FilterIndex=1;

        if (!SaveWavDialog->Execute()) return;

        Wav.SaveFile(SaveWavDialog->FileName);
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::OpenWav1Click(TObject *Sender)
{
        if (LoadWavDialog->Execute())
        {
                LoadFile(LoadWavDialog->FileName);

                if (AutoloadonInsert1->Down)
                {
                        if (BasicLister->ListerAvailable())
                        {
                                BasicLister->Clear();
                        }
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::NewWav1Click(TObject *Sender)
{
        Wav.NewFile();

        FileName="";

        TStatesSample=((machine.clockspeed / Wav.SampleRate));
        TapePos=0;
        CurrentTStates=0;
        Playing=0;
        Channel=0;
        ScrollBar->Max = Wav.NoSamples;
        ScrollBar->Position = 0;

        Caption="Wave Loader";

        UpdateImage();
        DoCaption("Stopped");
        RecordBtn->Enabled=Wav.CanRecord();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::ConvertNextBlock1Click(TObject *Sender)
{
        BYTE	*inbuf, *dataptr;
	long	ByteCount, DataCount;
	unsigned 	buffersize = (1024 * 64);
        bool    PlayingSave,  start;
        int byte, Silence;
        BYTE  *Data;

        if (emulator.machine == MACHINESPECTRUM || emulator.machine == MACHINEACE) return;
        
        int StartTapePos;

        int E_LINE;

        IgnoreZX81=true;
        PlayingSave = Playing;

        if (!Playing) PlayBtnClick(this);

	inbuf =(unsigned char *) malloc(buffersize);

        memset(inbuf, 0, buffersize);

        dataptr = inbuf;
        Data=NULL;
        ByteCount=0;
        DataCount=0;

        E_LINE=-1;
        start=true;

        StartTapePos=TapePos;

        do
        {
                byte=GetByte(start);
        }
        while((byte==-1) && Playing);

        Silence = (TapePos - StartTapePos) * TStatesSample;

        bool zx81File = (emulator.machine != MACHINEZX80 || (emulator.machine == MACHINEZX80 && (LowerCase(machine.CurRom) != "zx80.rom")));

        while (byte!=-1 && DataCount!=E_LINE)
        {
                if (start) start=false;
                ByteCount++;
                *(dataptr++) = (BYTE)byte;
                if (!Data)
                {
                        if ((zx81File && byte & 0x80) || !zx81File) Data=dataptr;
                }
                else    DataCount++;

                if (zx81File)
                {
                        if (DataCount == 12)
                        {
                                E_LINE = (Data[11] + Data[12]*256) - 16392;
                        }
                }
                else
                {
                        if (DataCount == 11)
                        {
                                E_LINE = (Data[10]+Data[11]*256) - 16383;
                        }
                }

                Application->ProcessMessages();
                byte=GetByte(start);
        }

        TZX->AddBlock(NULL,Silence);
        if (ByteCount>32) TZX->AddBlock((char*)inbuf, (int)ByteCount);
        TZX->MergeBlocks();
        TZX->UpdateTable(false);

        if (!PlayingSave) StopBtnClick(this);

        free(inbuf);
        IgnoreZX81=false;
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::AutoStartStop1Click(TObject *Sender)
{
        AutoStartStop1->Down = !AutoStartStop1->Down;
        if (!AutoStartStop1->Down && AutoloadonInsert1->Down)
                AutoloadonInsert1Click(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::PlayBtnClick(TObject *Sender)
{
        if (Playing || Recording) return;

        if (Wav.NoSamples==0) return;

        PlayBtn->Down=true;
        StopBtn->Down=false;
        RecordBtn->Down=false;
        RewStart->Enabled = false;
        Rewind->Enabled = false;
        FastForward->Enabled = false;
        FFEnd->Enabled = false;
        RecordBtn->Enabled = false;
        Playing=true;
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::RecordBtnClick(TObject *Sender)
{
        if (Playing || Recording || !Wav.CanRecord()) return;

        PlayBtn->Down=false;
        StopBtn->Down=false;
        RecordBtn->Down=true;
        RewStart->Enabled = false;
        Rewind->Enabled = false;
        FastForward->Enabled = false;
        FFEnd->Enabled = false;
        PlayBtn->Enabled = false;
        Recording=true;
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::StopBtnClick(TObject *Sender)
{
        if (!Playing && !Recording) return;
        
        if (Recording)
        {
                ScrollBar->Max = Wav.NoSamples;
                ScrollBar->Position = ((int)Wav.NoSamples - ScrollBar->Width >= 0) ? Wav.NoSamples - ScrollBar->Width : Wav.NoSamples;
        }
        else
        {
                ScrollBar->Position = TapePos;
        }

        Playing=false;
        Recording=false;

        PlayBtn->Down=false;
        StopBtn->Down=true;
        RecordBtn->Down=false;
        RecordBtn->Enabled = true;
        RewStart->Enabled = true;
        Rewind->Enabled = true;
        FastForward->Enabled = true;
        FFEnd->Enabled = true;
        PlayBtn->Enabled = true;

        UpdateImage();
        DoCaption("Stopped");
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::RewStartClick(TObject *Sender)
{
        if (Playing || Recording) return;

        TapePos=0;
        ScrollBar->Position = TapePos;
        UpdateImage();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::FFEndClick(TObject *Sender)
{
        if (Playing || Recording) return;

        TapePos=ScrollBar->Max;
        ScrollBar->Position = TapePos;
        UpdateImage();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::RewindClick(TObject *Sender)
{
        if (Playing || Recording) return;

        int delta;

        delta=ScrollBar->Max /20 ;
        TapePos -= delta;
        if ((int)TapePos>ScrollBar->Max) TapePos=0;
        ScrollBar->Position = TapePos;
        UpdateImage();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::FastForwardClick(TObject *Sender)
{
        if (Playing || Recording) return;

        int delta;

        delta=ScrollBar->Max /20 ;
        TapePos += delta;
        if ((int)TapePos>ScrollBar->Max) TapePos=ScrollBar->Max;
        ScrollBar->Position = TapePos;
        UpdateImage();
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::AutoloadonInsert1Click(TObject *Sender)
{
        AutoloadonInsert1->Down = !AutoloadonInsert1->Down;
        if (!AutoStartStop1->Down && AutoloadonInsert1->Down)
                AutoStartStop1Click(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::FormResize(TObject *Sender)
{
        int w;
        //ImgW=Volume->Left;
        //ImgH=ScrollBar->Top;

        ImgX=Bevel3->Left + 4;
        ImgY=Bevel1->Top + 4;

        ImgW=Bevel4->Left+Bevel4->Width - Bevel3->Left - 8;
        ImgH=Bevel2->Top+Bevel2->Height - Bevel1->Top - 8;
        ImgScale=256.0/ImgH;

        UpdateImage();

        w=StatusBar1->ClientWidth;
        w-=StatusBar1->Panels->Items[0]->Width;
        w-=StatusBar1->Panels->Items[1]->Width;
        w-=StatusBar1->Panels->Items[2]->Width;
        StatusBar1->Panels->Items[3]->Width=w;
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::FormPaint(TObject *Sender)
{
        UpdateImage();
}
//---------------------------------------------------------------------------


void __fastcall TWavLoad::SoundOnClick(TObject *Sender)
{
        SoundOn->Down = !SoundOn->Down;
}
//---------------------------------------------------------------------------

void __fastcall TWavLoad::FormCreate(TObject *Sender)
{
        if ((Panel3->Top+Panel3->Height) > ClientHeight)
        {
                Panel3->Top = ClientHeight - Panel3->Height;
                Panel3->Width = ClientWidth;

                ScrollBar->Top = Panel3->Top - (ScrollBar->Height-4);
                Bias->Height = (ScrollBar->Top - Bias->Top)-1;
                Volume->Height = (ScrollBar->Top - Volume->Top)-1;

                ScrollBar->Width=ClientWidth;

                Panel1->Top=ScrollBar->Top + ScrollBar->Height;
                Panel1->Left = (Panel3->Width)/2 - (Panel1->Width)/2;

                Bevel3->Height = ScrollBar->Top - (Bevel3->Top+4);
                Bevel2->Top = Bevel3->Top+Bevel3->Height+1;

                Bevel1->Width = Volume->Left  - (Bevel1->Left+4);
                Bevel2->Width = Bevel1->Width;

                Bevel4->Left = Bevel1->Left + Bevel1->Width+1;
                Bevel4->Height = Bevel3->Height;
        }

        if (Panel2->Width>ClientWidth) Panel2->Width=ClientWidth;

        if ((Bias->Left+Bias->Width)>ClientWidth)
        {
                Bias->Left = ClientWidth - Bias->Width;
                Volume->Left = Bias->Left - Volume->Width;
        }
}
//---------------------------------------------------------------------------


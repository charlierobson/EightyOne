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
 * tzxEditGeneral.cpp
 */

//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "tzxEditGeneral.h"
#include "tzxfile.h"
#include "tzxman.h"
#include "utils.h"
extern TTZXFile TZXFile;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditGeneralForm *EditGeneralForm;
//---------------------------------------------------------------------------
__fastcall TEditGeneralForm::TEditGeneralForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TEditGeneralForm::OKClick(TObject *Sender)
{
        Close();
}

unsigned char ZX81CharSet[]=" ..........\"£$:?()><=+-*/;,.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ................................................................";
unsigned char ZX80CharSet[]=" \"..........£$:?()-+*/=><;,.0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ................................................................";

enum Index
{
        zx80Index = 0,
        zx81Index = 1,
        asciiIndex = 2
};

//---------------------------------------------------------------------------
void TEditGeneralForm::DecodeData(int BlockNo)
{
        unsigned char c;

        ZXString text1, text2;
        unsigned short *pulses;
        int i,j;
        bool eol;
        unsigned char *p;

        Pause->Text = TZXFile.Tape[BlockNo].Pause;

        text1=""; text2="";

        //j=TZXFile.Tape[BlockNo].Head.General.SyncPulses;
        //for(i=0;i<TZXFile.Tape[BlockNo].Head.General.SyncPulses;i++)
        //{
        //        text1 += TZXFile.Tape[BlockNo].SyncPulses[i];
        //        j--;
        //        if (j) text1 += " ";
        //}

        //SyncPulses->Lines->Text = text1;

        text1="";

        pulses = TZXFile.Tape[BlockNo].SymDefD ;

        for(i=0;i<TZXFile.Tape[BlockNo].Head.General.ASD ;i++)
        {
            eol=false;
            j=0;
            text1+=i;
            text1+=": ";
            while(j<TZXFile.Tape[BlockNo].Head.General.NPD)
            {
                if (!eol)
                {
                        text1+=*pulses;
                        text1 += " ";
                }
                pulses++;
                if (*pulses==0) eol=true;
                j++;
            }
            text1 += "\n";
        }

        SymDefD->Lines->Text = text1;

        text1="";

        pulses = TZXFile.Tape[BlockNo].SymDefP ;

        for(i=0;i<TZXFile.Tape[BlockNo].Head.General.ASP ;i++)
        {
            eol=false;
            j=0;
            text1+=i;
            text1+=": ";
            while(j<TZXFile.Tape[BlockNo].Head.General.NPP)
            {
                if (!eol)
                {
                        text1+=*pulses;
                        text1 += " ";
                }
                pulses++;
                if (*pulses==0) eol=true;
                j++;
            }
            text1 += "\n";
        }

        SymDefP->Lines->Text = text1;

        text1="";
        pulses=TZXFile.Tape[BlockNo].PRLE;
        for(i=0;i<TZXFile.Tape[BlockNo].Head.General.TOTP;i++)
        {
                text1 += pulses[0];
                text1 += " x ";
                text1 += pulses[1];
                pulses += 2;
                text1 += "\n";
        }

        PRLE->Lines->Text=text1;

        nx = 0;

        text1="0000: ";
        text2=": ";
        j=0;
        p=TZXFile.Tape[BlockNo].Data.Data;

        for(i=0;i<TZXFile.Tape[BlockNo].Head.General.DataLen;i++)
        {
                c=*(p++);
                pbuffer[nx++] = c;

                text1 += IntToHex(c, 2);
                text1 += " ";

                if (CharSet->ItemIndex == asciiIndex)
                {
                        if (c<32 || c>=128) c='.';
                }
                else
                {
                        if (c>=128) c-=128;
                        c = (emulator.machine == MACHINEZX80) ? ZX80CharSet[c] : ZX81CharSet[c];
                }

                text2 += (char) c;


                if (++j == 16)
                {
                        j=0;
                        text1 += text2;
                        text1 += "\n";
                        text1 += IntToHex(i,4);
                        text1 += ": ";
                        text2 = ": ";
                }
        }

        if (text2.Length()>2)
        {
                for(i=(text2.Length()-2); i<16; i++)
                        text1 += "   ";
                text1 += text2;
        }


        Data->Lines->Text = text1;
}

void TEditGeneralForm::Go(int BlockNo, int Mx, int My)
{
        if (CharSet->ItemIndex == -1) CharSet->ItemIndex=0;

        Top = My - Height/2;
        Left = Mx - Width/2;
        Forms::TMonitor* monitor = TZXFile.FindMonitor(Left, Top);
        if (Top<monitor->Top) Top=monitor->Top;
        if (Left<monitor->Left) Left=monitor->Left;
        if (Left+Width > monitor->Left+monitor->Width) Left = monitor->Left+monitor->Width - Width;
        if (Top+Height > monitor->Top+monitor->Height) Top = monitor->Top+monitor->Height - Height;

        Block=BlockNo;
        DecodeData(Block);
        ActiveControl= Pause;
        ShowModal();

        TZXFile.Tape[BlockNo].Pause = Pause->Text.ToInt();
}
void __fastcall TEditGeneralForm::CharSetChange(TObject *Sender)
{
        DecodeData(Block);
}
//---------------------------------------------------------------------------


void __fastcall TEditGeneralForm::ButtonSaveClick(TObject *Sender)
{
        if (emulator.machine == MACHINEZX80)
        {
                SaveZX80File();
        }
        else
        {
                SaveZX81File();
        }
}

void __fastcall TEditGeneralForm::SaveZX81File()
{
        byte *px = pbuffer;
        ZXString fn;
        // extract the original filename from the first few bytes of the tape
        while(1)
        {
                fn += (char)ZX81CharSet[(*px) & 127];
                if (*px > 128) break;
                ++px;
        }

        SaveDialog1->Filter = "ZX81 Nameless Program (*.p)|*.p|ZX81 Named Program (*.p81)|*.p81";
        SaveDialog1->FileName = fn;
        if (SaveDialog1->Execute())
        {
                int filterIndex = SaveDialog1->FilterIndex;
                ZXString filename = SaveDialog1->FileName;
                ZXString extension = FileNameGetExt(filename);
                if (filterIndex == 1)
                {
                        if (extension!=".P") filename += ".p";
                        px++;
                }
                else
                {
                        if (extension!=".P81") filename += ".p81";
                        px=pbuffer;
                }
                FILE* pee = _tfopen(filename.c_str(), _TEXT("wb"));
                if (pee)
                {
                        fwrite(px, 1, nx, pee);
                        fclose(pee);
                }
                else
                {
                        ShowMessage("File saving failed: '" + filename + "'");
                }
        }
}

void __fastcall TEditGeneralForm::SaveZX80File()
{
        SaveDialog1->Filter = "ZX80 Program (*.o)|*.o";
        SaveDialog1->FileName = "";
        if (SaveDialog1->Execute())
        {
                ZXString filename = SaveDialog1->FileName;
                ZXString extension = FileNameGetExt(filename);
                if (extension!=".O") filename += ".o";

                FILE* pee = _tfopen(filename.c_str(), _TEXT("wb"));
                if (pee)
                {
                        fwrite(pbuffer, 1, nx, pee);
                        fclose(pee);
                }
                else
                {
                        ShowMessage("File saving failed: '" + filename + "'");
                }
        }
}

//---------------------------------------------------------------------------

void __fastcall TEditGeneralForm::FormShow(TObject *Sender)
{
        ButtonSave->Visible = (emulator.machine == MACHINEZX81) || (emulator.machine == MACHINEZX80) ||
                              (emulator.machine == MACHINETS1000) || (emulator.machine == MACHINETS1500) ||
                              (emulator.machine == MACHINER470) || (emulator.machine == MACHINELAMBDA) ||
                              (emulator.machine == MACHINETK85) || (emulator.machine == MACHINEZX97LE);

        switch (emulator.machine)
        {
                case MACHINEZX80:
                        CharSet->ItemIndex = zx80Index;
                        break;

                 case MACHINEZX81:
                 case MACHINETS1000:
                 case MACHINETS1500:
                 case MACHINEZX97LE:
                 case MACHINER470:
                 case MACHINETK85:
                 case MACHINELAMBDA:
                        CharSet->ItemIndex = zx81Index;
                        break;

                 default:
                        CharSet->ItemIndex = asciiIndex;
                        break;
        }
}
//---------------------------------------------------------------------------



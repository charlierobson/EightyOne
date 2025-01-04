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
 * tzxEditPause.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "tzxEditPause.h"
#include "tzxfile.h"
#include "tzxman.h"
extern TTZXFile TZXFile;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TEditPauseForm *EditPauseForm;
//---------------------------------------------------------------------------
__fastcall TEditPauseForm::TEditPauseForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void TEditPauseForm::Go(int BlockNo, int Mx, int My)
{
        Top = My - Height/2;
        Left = Mx - Width/2;
        Forms::TMonitor* monitor = TZXFile.FindMonitor(Left, Top);
        if (Top<monitor->Top) Top=monitor->Top;
        if (Left<monitor->Left) Left=monitor->Left;
        if (Left+Width > monitor->Left+monitor->Width) Left = monitor->Left+monitor->Width - Width;
        if (Top+Height > monitor->Top+monitor->Height) Top = monitor->Top+monitor->Height - Height;

        Edit->Text = TZXFile.Tape[BlockNo].Pause;
        ActiveControl = Edit;
        ShowModal();
        TZXFile.Tape[BlockNo].Pause=Edit->Text.ToInt();
}

void __fastcall TEditPauseForm::OKClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

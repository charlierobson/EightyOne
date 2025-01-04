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
 * Hist.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "Hist.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
THistoryBox *HistoryBox;
//---------------------------------------------------------------------------
__fastcall THistoryBox::THistoryBox(TComponent* Owner)
        : TForm(Owner)
{
        Text->Top = ToolBar1->Height;
        Text->Left = 0;
        Text->Width = this->ClientWidth;
        Text->Height = this->ClientHeight - StatusBar1->Height - ToolBar1->Height;

        reverseOrder = false;
        show0K8KAddresses = true;
        show8K16KAddresses = true;
        show16K32KAddresses = true;
        show32K48KAddresses = true;
        show48K64KAddresses = true;
}
//---------------------------------------------------------------------------

void __fastcall THistoryBox::FormResize(TObject *Sender)
{
        Text->Invalidate();        
}
//---------------------------------------------------------------------------
                      
void __fastcall THistoryBox::ToolButtonReloadClick(TObject *Sender)
{
        Dbg->ReloadHistoryWindow();
}
//---------------------------------------------------------------------------


void __fastcall THistoryBox::ToolButtonClearClick(TObject *Sender)
{
        Dbg->ClearHistoryWindow();
}
//---------------------------------------------------------------------------

void __fastcall THistoryBox::ReverseToolButtonReverseClick(TObject *Sender)
{
        reverseOrder = !reverseOrder;
        Dbg->ReloadHistoryWindow();
}
//---------------------------------------------------------------------------
void __fastcall THistoryBox::ToolButton0K8KClick(TObject *Sender)
{
        show0K8KAddresses = !show0K8KAddresses;
        Dbg->ReloadHistoryWindow();
}
//---------------------------------------------------------------------------

void __fastcall THistoryBox::ToolButton8K16KClick(TObject *Sender)
{
        show8K16KAddresses = !show8K16KAddresses;
        Dbg->ReloadHistoryWindow();
}
//---------------------------------------------------------------------------

void __fastcall THistoryBox::ToolButton16K32KClick(TObject *Sender)
{
        show16K32KAddresses = !show16K32KAddresses;
        Dbg->ReloadHistoryWindow();
}
//---------------------------------------------------------------------------

void __fastcall THistoryBox::ToolButton32K48KClick(TObject *Sender)
{
        show32K48KAddresses = !show32K48KAddresses;
        Dbg->ReloadHistoryWindow();
}
//---------------------------------------------------------------------------

void __fastcall THistoryBox::ToolButton48K64KClick(TObject *Sender)
{
        show48K64KAddresses = !show48K64KAddresses;
        Dbg->ReloadHistoryWindow();
}
//---------------------------------------------------------------------------

bool __fastcall THistoryBox::ReverseOrder()
{
        return reverseOrder;
}
//---------------------------------------------------------------------------

bool __fastcall THistoryBox::Show0K8KAddresses()
{
        return show0K8KAddresses;
}
//---------------------------------------------------------------------------

bool __fastcall THistoryBox::Show8K16KAddresses()
{
        return show8K16KAddresses;
}
//---------------------------------------------------------------------------

bool __fastcall THistoryBox::Show16K32KAddresses()
{
        return show16K32KAddresses;
}
//---------------------------------------------------------------------------

bool __fastcall THistoryBox::Show32K48KAddresses()
{
        return show32K48KAddresses;
}
//---------------------------------------------------------------------------

bool __fastcall THistoryBox::Show48K64KAddresses()
{
        return show48K64KAddresses;
}
//---------------------------------------------------------------------------


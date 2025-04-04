/* EightyOne - A Windows emulator of the Sinclair ZX range of computers.
 * Copyright (C) 2003-2025 Michael D Wynne
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
 */

#pragma warn -8066
#pragma warn -8008

#include <vcl4.h>
#pragma hdrstop

#include <sstream>

#include "SearchSequence_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSearchSequence *SearchSequence;
//---------------------------------------------------------------------------
__fastcall TSearchSequence::TSearchSequence(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TSearchSequence::CentreOn(TForm* parent)
{
        Top = parent->Top + (parent->Height - Height) /2;
        Left = parent->Left + (parent->Width - Width) /2;
}
//---------------------------------------------------------------------------
void __fastcall TSearchSequence::FormKeyPress(TObject *Sender, char &Key)
{
        if (Key == VK_ESCAPE)
        {
                cancelled = true;
                Close();
        }
}
//---------------------------------------------------------------------------
void __fastcall TSearchSequence::CancelClick(TObject *Sender)
{
        cancelled=true;
        Close();          
}
//---------------------------------------------------------------------------
void __fastcall TSearchSequence::FindClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
bool TSearchSequence::EditSequenceBytes(AnsiString& searchText, std::vector<int>& bytes)
{
        cancelled=false;

        EditSequence->Text = searchText;

        ActiveControl = EditSequence;
        EditSequence->SelectAll();

        ShowModal();

        searchText = EditSequence->Text;

        if (cancelled)
        {
                return false;
        }

        return ValidateSearchSequence(bytes);
}

bool TSearchSequence::ValidateSearchSequence(std::vector<int>& bytes)
{
        AnsiString sequenceValue = EditSequence->Text.Trim();

        if (sequenceValue.Length() == 0)
        {
                return false;
        }

        // Split to entered text up into separate strings
        std::istringstream iss(sequenceValue.c_str());
        std::vector<std::string> strings((std::istream_iterator<std::string>(iss)),
                                         std::istream_iterator<std::string>());

        // Convert the strings into bytes
        std::vector<std::string>::iterator iter;
        for (iter = strings.begin(); iter < strings.end(); iter++)
        {
                std::string entry = *iter;

                if (entry.length() == 0)
                        return false;

                std::string entryStr = entry;
                
                if (entry.c_str()[0] == '$')
                {
                        entryStr = "0x" + entry.substr(1, entryStr.length() - 1);
                }

                char* endPtr;
                int entryValue = int(strtol(entryStr.c_str(), &endPtr, 0));
                if ((*endPtr != 0) && (entryValue >= 0) && (entryValue <= 255))
                {
                        return false;
                }

                bytes.push_back(entryValue);
        }

        return true;
}

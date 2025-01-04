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
 * About.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "About_.h"
#include "zx81config.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TAbout *About;

#define MaxWidth(label,width) if((label->Width + label->Left) > width) width=label->Width + label->Left

//---------------------------------------------------------------------------

static ZXString ExtractPart(ZXString versionNumber, int& s, int&e)
{
        ZXString part;

        while (e <= versionNumber.Length())
        {
                if (versionNumber[e] == '.')
                {
                        part = versionNumber.SubString(s, e - s);
                        s = e + 1;
                        e += 2;
                        break;
                }

                e++;
        }

        return part;
}

bool GetVersionNumber(int& versionNumberMajor, int& versionNumberMinor, int& versionNumberPart3, int& versionNumberPart4)
{
        versionNumberMajor = 0;
        versionNumberMinor = 0;
        versionNumberPart3 = 0;
        versionNumberPart4 = 0;

        ZXString versionNumber = "";

        ZXString fileName = Application->ExeName;

	// Get the size of the version information buffer
	DWORD dwHandle = 0;
	DWORD dwLength = GetFileVersionInfoSize(fileName.c_str(), &dwHandle);
	if (dwLength < 1)
        {
		return false;
	}

	// Allocate space for the version information buffer
	LPVOID lpvMem;
	HANDLE hMem = GlobalAlloc(GMEM_MOVEABLE, dwLength);
	if (hMem == NULL)
        {
		return false;
	}
        
	lpvMem = GlobalLock(hMem);
	if (lpvMem == NULL)
        {
		GlobalUnlock(hMem);
		GlobalFree(hMem);
		return false;
	}

	// Get the version information block to the buffer
	BOOL fRet = GetFileVersionInfo(	fileName.c_str(), dwHandle, dwLength, (LPVOID)lpvMem);
	if (!fRet)
        {
		GlobalUnlock(hMem);
		GlobalFree(hMem);
		return false;
	}

        // Get the root block
        VS_FIXEDFILEINFO fileInfo;
	LPVOID lpInfo;
	UINT cch;
	if (VerQueryValue(lpvMem, _TEXT("\\"), &lpInfo, &cch))
        {
                ZeroMemory((void*)(&fileInfo), sizeof(fileInfo));
                CopyMemory((void*)&fileInfo, (const void*)lpInfo, sizeof(fileInfo));
        }
	else
        {
		GlobalUnlock(hMem);
		GlobalFree(hMem);
		return false;
	}

	// Get translation information
        ZXString langID;
        ZXString charset;
        if (VerQueryValue(lpvMem, _TEXT("\\VarFileInfo\\Translation"), &lpInfo, &cch))
        {
    	        WORD wLangID = ((WORD*)lpInfo)[0];
                WORD wCharsetID = ((WORD*)lpInfo)[1];
                langID = langID.IntToHex((int)wLangID, 4);
                charset = charset.IntToHex((int)wCharsetID, 4);
   	}
  	else
        {
  		GlobalUnlock(hMem);
  		GlobalFree(hMem);
  		return false;
  	}

    // Fetch the version information
    _TCHAR key[80];

    _tcscpy(key, _TEXT("\\StringFileInfo\\"));
    ZXString versionInfoLangID(langID + charset);
    _tcscat(key, versionInfoLangID.c_str());
    _tcscat(key, _TEXT("\\"));
    _tcscat(key, _TEXT("FileVersion"));

    if (VerQueryValue(lpvMem, key, &lpInfo, &cch))
        {
                versionNumber = (_TCHAR*)lpInfo;
                versionNumber += ".";
        }
        else
        {
  		GlobalUnlock(hMem);
		GlobalFree(hMem);
  		return false;
  	}
        
	GlobalUnlock(hMem);
	GlobalFree(hMem);

        // Extract the version number parts

        int s = 1;
        int e = 2;
        versionNumberMajor = StrToInt(ExtractPart(versionNumber, s, e));
        versionNumberMinor = StrToInt(ExtractPart(versionNumber, s, e));
        versionNumberPart3 = StrToInt(ExtractPart(versionNumber, s, e));
        versionNumberPart4 = StrToInt(ExtractPart(versionNumber, s, e));

        return true;
}
//---------------------------------------------------------------------------
__fastcall TAbout::TAbout(TComponent* Owner)
        : TForm(Owner)
{
        int w=0;

        Left = (Screen->Width - Width) /2;
        Top = (Screen->Height - Height) /2;

        int versionNumberMajor;
        int versionNumberMinor;
        int versionNumberPart3;
        int versionNumberPart4;
        bool versionFound = GetVersionNumber(versionNumberMajor, versionNumberMinor, versionNumberPart3, versionNumberPart4);
        if (versionFound)
        {
                Version->Caption="Version ";
                Version->Caption = Version->Caption + versionNumberMajor;
                Version->Caption = Version->Caption + ".";
                Version->Caption = Version->Caption + versionNumberMinor;
                if (versionNumberPart3 != 0)
                {
                        Version->Caption = Version->Caption + ".";
                        Version->Caption = Version->Caption + versionNumberPart3;
                        if (versionNumberPart4 != 0)
                        {
                                Version->Caption = Version->Caption + ".";
                                Version->Caption = Version->Caption + versionNumberPart4;
                        }
                }
        }
        else
        {
                Version->Caption = "Unknown";
        }

        MaxWidth(Label1,w); MaxWidth(Label9,w);
        MaxWidth(Label2,w); MaxWidth(Label10,w);
        MaxWidth(Label3,w); MaxWidth(Label11,w);
        MaxWidth(Label4,w); MaxWidth(Label12,w);
        MaxWidth(Label5,w); MaxWidth(Label13,w);
        MaxWidth(Label6,w); MaxWidth(Label14,w);
        MaxWidth(Label7,w); MaxWidth(Label15,w);
        MaxWidth(Label8,w); MaxWidth(Label16,w);
        MaxWidth(Label17,w);

        ClientWidth=w+8;
}

//---------------------------------------------------------------------------
void __fastcall TAbout::Button1Click(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TAbout::Label15Click(TObject *Sender)
{
        //  Original = http://www.chuntey.com/eightyone/
        
        ShellExecute(0,NULL, _TEXT("https://sourceforge.net/projects/eightyone-sinclair-emulator/"), NULL, NULL, SW_NORMAL);
}
//---------------------------------------------------------------------------

void __fastcall TAbout::Label15MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        Label15->Font->Color=clRed;
}
//---------------------------------------------------------------------------

void __fastcall TAbout::Label15MouseUp(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        Label15->Font->Color=clHighlight;
}
//---------------------------------------------------------------------------




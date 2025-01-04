//---------------------------------------------------------------------------

#include <vcl4.h>
#include <windows.h>
#pragma hdrstop

#include "LiveMemoryWindow_.h"
#include "main_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLiveMemoryWindow *LiveMemoryWindow;
//---------------------------------------------------------------------------

__fastcall TLiveMemoryWindow::TLiveMemoryWindow(TComponent* Owner)
        : TForm(Owner)
{
        BITMAPINFOHEADER bmphdr = {sizeof(bmphdr), 256, -256, 1, 32, BI_RGB,
                0,0,0,0,0};

        _hdib = CreateDIBSection (Canvas->Handle, (BITMAPINFO*)&bmphdr,
                DIB_RGB_COLORS, (VOID**)&_pbits, NULL, 0);

        _memStart = 0;
        _memEnd = 64;

        Reset();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Reset()
{
        memset((void*)_pbits, 0, sizeof(RGBQUAD) * 65536);
        memset(_reads, 0, 65536);
        memset(_writes, 0, 65536);
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Write(unsigned short address)
{
        if (!Visible) return;
        if (!Writes1->Checked) return;

        _writes[address]=255;
        Update();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Read(unsigned short address)
{
        if (!Visible) return;
        if (!Reads1->Checked) return;

        _reads[address]=255;
        Update();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Update(void)
{
        BYTE touchCol = Touches1->Checked ? (BYTE)128 : (BYTE)0;

        // should really be time based, not access count based
        ++_count;
        if (_count < 1000) return;

        _count = 0;

        for(int i = 0; i < 65536; ++i)
        {
                if (_writes[i])
                {
                        --_writes[i];
                        _pbits[i].rgbBlue = _writes[i];
                        _pbits[i].rgbGreen = touchCol;
                }
                if (_reads[i])
                {
                        --_reads[i];
                        _pbits[i].rgbRed = _reads[i];
                        _pbits[i].rgbGreen = touchCol;
                }
        }

        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::WMEraseBkgnd(TWMEraseBkgnd &Message)
{
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::FormPaint(TObject *Sender)
{
        HDC hdcMem = CreateCompatibleDC (Canvas->Handle);
        HBITMAP hbmpOld = (HBITMAP)SelectObject (hdcMem, _hdib);

        StretchBlt (Canvas->Handle, 0, 0, 512, 256,
                hdcMem, 0, _memStart * 4, 256, (_memEnd - _memStart) * 4,
                        SRCCOPY);

        SelectObject (hdcMem, hbmpOld);
        DeleteDC (hdcMem);
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Reset1Click(TObject *Sender)
{
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Reads1Click(TObject *Sender)
{
        Reads1->Checked = !Reads1->Checked;
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Writes1Click(TObject *Sender)
{
        Writes1->Checked = !Writes1->Checked;
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::Touches1Click(TObject *Sender)
{
        Touches1->Checked = !Touches1->Checked;
        Reset();
        Invalidate();
}
//---------------------------------------------------------------------------

long map(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void __fastcall TLiveMemoryWindow::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        // coordinates can 'leak' past the control bounds >:(
        if (X > 511) X = 511;
        if (Y > 255) Y = 255;

        long my = map(Y, 0, 256, _memStart * 4, _memEnd * 4 - 1);
        unsigned short addr = (unsigned short)(my * 256 + X);

        StatusBar1->Panels->Items[0]->Text = Format("$%0.4x",
                ARRAYOFCONST((addr)));

        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::ROM1Click(TObject *Sender)
{
        TMenuItem* sender = static_cast<TMenuItem*>(Sender);

        _memStart = sender->Tag & 0xff;
        _memEnd = sender->Tag / 256;

        StatusBar1->Panels->Items[1]->Text = Format("%d - %dK",
                ARRAYOFCONST((_memStart, _memEnd)));

        Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TLiveMemoryWindow::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        // ugh
        Form1->LiveMemoryOverview->Checked=false;
}
//---------------------------------------------------------------------------


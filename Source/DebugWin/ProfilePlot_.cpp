//---------------------------------------------------------------------------

#include <vcl4.h>
#include <windows.h>

#pragma hdrstop

#include "ProfilePlot_.h"
#include "ProfileDetail.h"
#include "Profiler.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProfilePlot *ProfilePlot;
//---------------------------------------------------------------------------
__fastcall TProfilePlot::TProfilePlot(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::FormPaint(TObject *Sender)
{
        if (_pd == NULL || _pd->SampleCount() == 0)
        {
                ScrollBarHorizontal->Max = 0;
                ScrollBarHorizontal->Position = 0;
                ScrollBarHorizontal->Enabled = false;
                return;
        }

        int min = 0;
        int max = 1;
        if (_pd->SampleCount() != 0) {
                min = _pd->Min() - 100;
                max = _pd->Max() + 100;
        }
        if (min < 0) min = 0;
        int range = max - min;

        int height = ClientHeight - ScrollBarHorizontal->Height - StatusBar1->Height - 2;

        double scale = (double)height / (double)range;

        Canvas->Pen->Color = clBlue;
        Canvas->MoveTo(0, height - (int)((double)_pd->Sample(0) * scale));

        int count = _pd->SampleCount();

        int maximum = (count > ClientWidth) ? count - ClientWidth : ClientWidth;
        ScrollBarHorizontal->Max = maximum;
        if (ScrollBarHorizontal->Position > maximum)
        {
                ScrollBarHorizontal->Position = maximum;
        }

        int mini = ClientWidth;
        bool scrollable = (count > mini);
        if (!scrollable)
        {
                mini = count;
                ScrollBarHorizontal->Position = 0;
        }
        ScrollBarHorizontal->Enabled = scrollable;
        
        int sbBase = ScrollBarHorizontal->Enabled ? ScrollBarHorizontal->Position : 0;

        for (int i = 0; i < mini; ++i)
        {
                double sam = _pd->Sample(i + sbBase) - _pd->Min();
                int y = (int)(sam * scale);
                Canvas->LineTo(i, height - y);
        }

        UpdateMinMax(_pd);
}
//---------------------------------------------------------------------------

void TProfilePlot::InitScrollbar()
{
        if (_pd == NULL) return;

        ScrollBarHorizontal->Min = 0;
        bool enabled = _pd->SampleCount() > ClientWidth;
        ScrollBarHorizontal->Enabled = enabled;
        ScrollBarHorizontal->Max = std::max(_pd->SampleCount() - ClientWidth, ClientWidth);
        ScrollBarHorizontal->Position = 0;

        Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::PlotTGraph(ProfileDetail* pd, ZXString caption)
{
        _pd = pd;
        Caption = "Profiler - " + caption;
        InitScrollbar();
        UpdateMinMax(pd);
        Show();
}

void TProfilePlot::UpdateMinMax(ProfileDetail* pd)
{
        int min = 0;
        int max = 0;
        if (_pd->SampleCount() != 0) {
                min = _pd->Min();
                max = _pd->Max();
        }

        ZXString minmax = "Min: ---- Max: ----";
        if (_pd) {
                minmax = "Min: ";
                minmax += min;
                minmax += " Max: ";
                minmax += max;
        }
        StatusBar1->Panels->Items[0]->Text = minmax;
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::FormResize(TObject *Sender)
{
        InitScrollbar();
        Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::ScrollBarHorizontalChange(TObject *Sender)
{
        Refresh();
}

void __fastcall TProfilePlot::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        TPoint coords;
        coords.x = X;
        coords.y = Y;
        ScreenToClient(coords);
        int sample = coords.x;
        if (ScrollBarHorizontal->Enabled) {
                sample += ScrollBarHorizontal->Position;
        }

        ZXString cur = "Current: ----";
        ZXString sam = "Sample: ----";

        if (_pd) {
                cur = "Current: ";
                cur += _pd->Sample(sample);
                sam = "Sample: ";
                sam += sample;
        }
        StatusBar1->Panels->Items[1]->Text = sam;
        StatusBar1->Panels->Items[2]->Text = cur;
}
//---------------------------------------------------------------------------

void __fastcall TProfilePlot::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        Visible = false;
        Profiler->EnableButtons(true);
}
//---------------------------------------------------------------------------

void TProfilePlot::ShowLast()
{
        if (_pd == NULL) return;
        
        bool scrollable = (_pd->SampleCount() > ClientWidth);

        if (scrollable)
        {
                ScrollBarHorizontal->Position = ScrollBarHorizontal->Max;
        }
}


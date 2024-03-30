//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "Profiler.h"
#include "ProfSampEdit.h"
#include "ProfileDetail.h"
#include "ProfilePlot_.h"
#include "symbolstore.h"
#include "z80.h"

static const int START = 0;
static const int END = 1;
static const int MIN = 2;
static const int MAX = 3;
static const int COUNT = 4;

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProfiler *Profiler;
//---------------------------------------------------------------------------
__fastcall TProfiler::TProfiler(TComponent* Owner)
        : TForm(Owner)
{
        _pse = new TProfileSampleEdit(this);
}
//---------------------------------------------------------------------------
void __fastcall TProfiler::EnableButtons(bool enabled)
{
        if (enabled)
        {
                bool entriesExist = (ListViewProfileSamples->Items->Count > 0);
                bool entrySelected = entriesExist && ListViewProfileSamples->Selected;

                ButtonNew->Enabled = true;
                ButtonEdit->Enabled = entrySelected;
                ButtonReset->Enabled = entriesExist;
                ButtonDelete->Enabled = entrySelected;
                ButtonRefresh->Enabled = entriesExist && !AutoRefresh->Checked;
                ButtonPlot->Enabled = entrySelected && !ProfilePlot->Visible;
                AutoRefresh->Enabled = entriesExist;
        }
        else
        {
                ButtonNew->Enabled = false;
                ButtonEdit->Enabled = false;
                ButtonReset->Enabled = false;
                ButtonDelete->Enabled = false;
                ButtonRefresh->Enabled = false;
                ButtonPlot->Enabled = false;
                AutoRefresh->Enabled = false;
        }
}
//---------------------------------------------------------------------------
void __fastcall TProfiler::UpdateItem(TListItem* item, ZXString tag, ProfileDetail& pd)
{
        item->Caption = tag;
        item->SubItems->Strings[START] = symbolstore::addressToSymbolOrHex(pd._start);
        item->SubItems->Strings[END] = symbolstore::addressToSymbolOrHex(pd._end);
#if __CODEGEARC__ < 0x0620
        item->SubItems->Strings[MIN] = pd.Min() == INT_MAX ? "--" : IntToStr(pd.Min()).c_str();
        item->SubItems->Strings[MAX] = pd.Max() == INT_MIN ? "--" : IntToStr(pd.Max()).c_str();
#else
        item->SubItems->Strings[MIN] = pd.Min() == INT_MAX ? L"--" : IntToStr(pd.Min()).c_str();
        item->SubItems->Strings[MAX] = pd.Max() == INT_MIN ? L"--" : IntToStr(pd.Max()).c_str();
#endif
        item->SubItems->Strings[COUNT] = pd.SampleCount();
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonNewClick(TObject *Sender)
{
        EnableButtons(false);

        _newPD = new ProfileDetail(0, 0);
        _pse->EditValues("", _newPD, &SampleEditComplete);
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonEditClick(TObject *Sender)
{
        TListItem* editItem = ListViewProfileSamples->Selected;
        if (!editItem) return;

        EnableButtons(false);

        _pse->EditValues(editItem->Caption, &_profileDetails[editItem->Index], &SampleEditComplete);

        EnableButtons(true);
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonResetClick(TObject *Sender)
{
        TListItem* selected = ListViewProfileSamples->Selected;
        if (!selected) return;

        EnableButtons(false);

        int idx = selected->Index;
        ProfileDetail& detail = _profileDetails[idx];
        detail.Reset();
        UpdateItem(selected, selected->Caption, detail);
        ProfilePlot->Refresh();

        EnableButtons(true);
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonDeleteClick(TObject *Sender)
{
        TListItem* selected = ListViewProfileSamples->Selected;
        if (!selected) return;

        EnableButtons(false);

        Timer->Enabled = false;

#if __CODEGEARC__ < 0x0620
        int ret = Application->MessageBox("Are you sure you wish to delete this entry?", "Delete Profile Entry", MB_YESNO | MB_ICONQUESTION);
#else
        int ret = Application->MessageBox(L"Are you sure you wish to delete this entry?", L"Delete Profile Entry", MB_YESNO | MB_ICONQUESTION);
#endif

        if (ret == IDYES)
        {
                int idx = selected->Index;
                ListViewProfileSamples->Items->Delete(idx);
                _profileDetails.erase(_profileDetails.begin() + idx);

                bool entriesExist = (ListViewProfileSamples->Items->Count > 0);
                if (entriesExist)
                {
                        ListViewProfileSamples->Selected = ListViewProfileSamples->Items->Item[0];
                }
                else
                {
                        ProfilePlot->Close();
                }
        }

        Timer->Enabled = true;

        EnableButtons(true);
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::DebugTick(processor* z80)
{
        for (size_t i = 0; i < _profileDetails.size(); ++i) {
                _profileDetails[i].Update(z80->pc.w, z80->tstates);
        }
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::SampleEditCompleteImpl(bool valid, ZXString tag)
{
        if (!valid) {
                delete(_newPD);
                _newPD = NULL;
                EnableButtons(true);
                return;
        }

        if (_newPD)
        {
                _profileDetails.push_back(*_newPD);

                TListItem* newItem = ListViewProfileSamples->Items->Add();
                newItem->Caption = tag;
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");
                newItem->SubItems->Add("0");

                _newPD = NULL;
        }

        EnableButtons(true);

        Refresh();
}

void TProfiler::SampleEditComplete(bool valid, ZXString tag)
{
        Profiler->SampleEditCompleteImpl(valid, tag);
}

//---------------------------------------------------------------------------

void __fastcall TProfiler::Refresh()
{
        for (int i = 0; i < ListViewProfileSamples->Items->Count; i++)
        {
                TListItem* item = ListViewProfileSamples->Items->Item[i];
                int idx = item->Index;
                ProfileDetail& detail = _profileDetails[idx];
                UpdateItem(item, item->Caption, detail);
        }
        
        if (ProfilePlot->Visible)
                ProfilePlot->Refresh();
}

void __fastcall TProfiler::ButtonRefreshClick(TObject *Sender)
{
        Refresh();
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::ButtonPlotClick(TObject *Sender)
{
        TListItem* selected = ListViewProfileSamples->Selected;
        if (!selected) return;

        ProfilePlot->PlotTGraph(&_profileDetails[selected->Index], selected->Caption);

        EnableButtons(true);
}
//---------------------------------------------------------------------------

 void __fastcall TProfiler::ListViewProfileSamplesSelectItem(
      TObject *Sender, TListItem *Item, bool Selected)
{
        if (ProfilePlot->Visible)
        {
                TListItem* selected = ListViewProfileSamples->Selected;
                if (selected)
                {
                        ProfilePlot->PlotTGraph(&_profileDetails[selected->Index], selected->Caption);
                }
        }
        
        EnableButtons(true);
}
//---------------------------------------------------------------------------


void __fastcall TProfiler::ListViewProfileSamplesEditing(TObject *Sender,
      TListItem *Item, bool &AllowEdit)
{
        AllowEdit = false;        
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::TimerTimer(TObject *Sender)
{
        if ((ListViewProfileSamples->Items->Count > 0) && AutoRefresh->Checked)
        {
                Refresh();

                if (ProfilePlot->Visible)
                {
                        ProfilePlot->ShowLast();
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::FormClose(TObject *Sender, TCloseAction &Action)
{
        Timer->Enabled = false;

        ProfilePlot->Close();    
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::FormShow(TObject *Sender)
{
        Timer->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TProfiler::AutoRefreshClick(TObject *Sender)
{
        EnableButtons(true);
}
//---------------------------------------------------------------------------


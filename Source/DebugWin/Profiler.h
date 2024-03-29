//---------------------------------------------------------------------------

#ifndef TProfilerH
#define TProfilerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>
#include <vector>
#include <z80.h>
#include "zx81config.h"

//---------------------------------------------------------------------------
class TProfiler : public TForm
{
__published:	// IDE-managed Components
        TListView *ListViewProfileSamples;
        TButton *ButtonNew;
        TButton *ButtonDelete;
        TButton *ButtonEdit;
        TButton *ButtonReset;
        TButton *ButtonRefresh;
        TButton *ButtonPlot;
        TTimer *Timer;
        TCheckBox *AutoRefresh;
        void __fastcall ButtonNewClick(TObject *Sender);
        void __fastcall ButtonEditClick(TObject *Sender);
        void __fastcall ButtonResetClick(TObject *Sender);
        void __fastcall ButtonDeleteClick(TObject *Sender);
        void __fastcall ButtonRefreshClick(TObject *Sender);
        void __fastcall ButtonPlotClick(TObject *Sender);
        void __fastcall ListViewProfileSamplesSelectItem(TObject *Sender,
          TListItem *Item, bool Selected);
        void __fastcall ListViewProfileSamplesEditing(TObject *Sender,
          TListItem *Item, bool &AllowEdit);
        void __fastcall TimerTimer(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall AutoRefreshClick(TObject *Sender);
private:	// User declarations
        void __fastcall UpdateItem(TListItem* item, ZXString tag, class ProfileDetail& pd);
        void __fastcall SampleEditCompleteImpl(bool valid, ZXString tag);
        int Count();

        std::vector<class ProfileDetail> _profileDetails;

        class ProfileDetail* _newPD;
        class TProfileSampleEdit* _pse;

public:		// User declarations
        void __fastcall EnableButtons(bool enabled);
        __fastcall TProfiler(TComponent* Owner);

        static void SampleEditComplete(bool valid, ZXString tag);

        void __fastcall DebugTick(processor* z80);

        void __fastcall Refresh();
};
//---------------------------------------------------------------------------
extern PACKAGE TProfiler *Profiler;
//---------------------------------------------------------------------------
#endif

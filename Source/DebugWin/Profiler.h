//---------------------------------------------------------------------------

#ifndef TProfilerH
#define TProfilerH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <vector>
#include <z80.h>

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
        void __fastcall ButtonNewClick(TObject *Sender);
        void __fastcall ButtonEditClick(TObject *Sender);
        void __fastcall ButtonResetClick(TObject *Sender);
        void __fastcall ButtonDeleteClick(TObject *Sender);
        void __fastcall ButtonRefreshClick(TObject *Sender);
        void __fastcall ButtonPlotClick(TObject *Sender);
private:	// User declarations
        void __fastcall UpdateItem(TListItem* item, AnsiString tag, class ProfileDetail& pd);
        void __fastcall SampleEditCompleteImpl(bool valid, AnsiString tag);
        void __fastcall EnableButtons(bool enabled);

        std::vector<class ProfileDetail> _profileDetails;

        class ProfileDetail* _newPD;
        class TProfileSampleEdit* _pse;

public:		// User declarations
        __fastcall TProfiler(TComponent* Owner);

        static void SampleEditComplete(bool valid, AnsiString tag);

        void __fastcall DebugTick(processor* z80);

        void __fastcall Refresh();
};
//---------------------------------------------------------------------------
extern PACKAGE TProfiler *Profiler;
//---------------------------------------------------------------------------
#endif
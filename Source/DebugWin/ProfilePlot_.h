//---------------------------------------------------------------------------

#ifndef ProfilePlot_H
#define ProfilePlot_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "zx81config.h"
//---------------------------------------------------------------------------
class TProfilePlot : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBarHorizontal;
        TStatusBar *StatusBar1;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall ScrollBarHorizontalChange(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift, int X, int Y);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
private:	// User declarations
        class ProfileDetail* _pd;
        void InitScrollbar();
        void UpdateMinMax(ProfileDetail* pd);
        int _mouseX;
        
public:		// User declarations
        __fastcall TProfilePlot(TComponent* Owner);
        void __fastcall PlotTGraph(class ProfileDetail* pd, ZXString caption);
        void ShowLast();
};
//---------------------------------------------------------------------------
extern PACKAGE TProfilePlot *ProfilePlot;
//---------------------------------------------------------------------------
#endif

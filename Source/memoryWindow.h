//---------------------------------------------------------------------------

#ifndef memoryWindowH
#define memoryWindowH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <Grids.hpp>
#include <ExtCtrls.hpp>
#include <Menus.hpp>
#include "OffBtn.hpp"
#include <set>
#include <vector>
//---------------------------------------------------------------------------

static const int SBARWIDTH = 24;

class RowRenderer
{
public:
        RowRenderer() :
                mLMargin(0),
                mKern(0),
                mCellWidth(1),
                mBytesPerCell(1),
                mDisplayCellsPerRow(1)
        {};

        virtual ~RowRenderer(){};

        virtual void RenderRow(void) = 0;
        virtual void SetGeometry(int, TSize&) = 0;

        virtual bool ByteAtX(const int, int&);
        void RenderColumnHeadings(const TSize& charSize);
        int BytesPerCell() { return mBytesPerCell; };

        // in
        std::set<int>::iterator mDirty;
        std::set<int>::iterator mLast;
        int mAddress, mY, mSelectedAddress;
        HDC mCHDC;

        // out
        int mDisplayCellsPerRow;

protected:
        void SetGeometry(int, const TSize&, int);
        void ChooseTextColour(int itemSize);
        void AddressOut();
        void SetCharacterBackgroundColour(int xpos, int ypos, COLORREF paper, COLORREF ink);

        int mLMargin, mKern;
        int mCellWidth, mBytesPerCell;
};

class ByteRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};
class DecimalRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};
class WordRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};
class BinaryRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
};
class TraditionalRowRenderer : public RowRenderer
{
public:
        virtual void RenderRow(void);
        virtual void SetGeometry(int, TSize&);
        virtual bool ByteAtX(const int, int&);
};


class TMemoryWindow : public TForm
{
__published:	// IDE-managed Components
        TScrollBar *ScrollBar1;
        TPopupMenu *PopupMenu1;
        TMenuItem *ViewBytes1;
        TMenuItem *ViewWords1;
        TMenuItem *SetAddress1;
        TStatusBar *StatusBar1;
        TMenuItem *JumpTo1;
        TMenuItem *ViewTraditional1;
        TButton *ButtonFirstChange;
        TButton *ButtonPrevChange;
        TButton *ButtonNextChange;
        TButton *ButtonLastChange;
        TMenuItem *ViewBinary1;
        TUpDown *IncDecAddress;
        TMenuItem *ViewDecimal1;
        TMenuItem *Search1;
        TMenuItem *ClearHighlights;
        void __fastcall FormPaint(TObject *Sender);
        void __fastcall FormResize(TObject *Sender);
        void __fastcall ScrollBar1Change(TObject *Sender);
        void __fastcall ViewBytes1Click(TObject *Sender);
        void __fastcall SetAddress1Click(TObject *Sender);
        void __fastcall FormClick(TObject *Sender);
        void __fastcall FormMouseMove(TObject *Sender, TShiftState Shift,
          int X, int Y);
        void __fastcall FormMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall JumpTo1Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ButtonChangeMouseMove(TObject *Sender,
          TShiftState Shift, int X, int Y);
        void __fastcall ButtonPrevChangeClick(TObject *Sender);
        void __fastcall ButtonFirstChangeClick(TObject *Sender);
        void __fastcall ButtonNextChangeClick(TObject *Sender);
        void __fastcall ButtonLastChangeClick(TObject *Sender);
        void __fastcall IncDecAddressChangingEx(TObject *Sender,
          bool &AllowChange, short NewValue, TUpDownDirection Direction);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall Search1Click(TObject *Sender);
        void __fastcall FormMouseWheel(TObject *Sender, TShiftState Shift,
          int WheelDelta, TPoint &MousePos, bool &Handled);
        void __fastcall ClearHighlightsClick(TObject *Sender);

private:
       void __fastcall OnEraseBkgnd (TMessage msg);

  BEGIN_MESSAGE_MAP
    MESSAGE_HANDLER(WM_ERASEBKGND, TMessage, OnEraseBkgnd)
  END_MESSAGE_MAP(TForm)

        void __fastcall SetBaseAddress(int value);
        bool __fastcall xyToAddress(int xIn, int yIn, int& address);

        void __fastcall SetSBButtonPosition(TButton* btn, int idx);
		void __fastcall GlueButtonsToStatusBar();

        int mRows;
        int mBaseAddress;
        bool ignoreScrollChange;
        
        HWND mHWND;
        HBITMAP mOffscreenBitmap;

        int mBMWidth;
        int mBMHeight;

        int mViewMode;
        TSize mCharSize;
        int mHeadingHeight;
        RowRenderer* mRowRenderer;

        int mSelectedAddress;
        ZXString mSearchText;

        void CreateBitmap(void);
        bool FindSequence(std::vector<int>& bytes, int& addr);
        bool FindMatch(int& addr, std::vector<int>& bytes);
        void DoSearch();
        void PerformSearch(std::vector<int>& bytes);

public:		// User declarations
        __fastcall TMemoryWindow(TComponent* Owner);
        virtual __fastcall ~TMemoryWindow();

        __property int BaseAddress  = { write=SetBaseAddress };

        enum { MWVM_BYTE, MWVM_WORD, MWVM_BINARY, MWVM_DECIMAL, MWVM_TRADITIONAL };

        void __fastcall SetViewMode(int value);

        void __fastcall UpdateChanges();
        void __fastcall ClearChanges();
};

//---------------------------------------------------------------------------
extern PACKAGE TMemoryWindow *MemoryWindow;
//---------------------------------------------------------------------------
#endif

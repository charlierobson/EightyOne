//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "z80.h"
#include "zx81.h"
#include "debug.h"
#include "memoryWindow.h"
#include "EditValue_.h"
#include "SearchSequence_.h"
#include <set>
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "OffBtn"
#pragma resource "*.dfm"
TMemoryWindow *MemoryWindow;
//---------------------------------------------------------------------------

extern bool directMemoryAccess;

std::set<int> dirtyBird;
std::set<int>::iterator changeCursor;

__fastcall TMemoryWindow::TMemoryWindow(TComponent* Owner)
        : TForm(Owner)
{
        mHWND = this->Handle;

        mOffscreenBitmap = NULL;
        mRowRenderer = NULL;

        mSelectedAddress = -1;

        ignoreScrollChange = false;

        SetViewMode(MWVM_TRADITIONAL);

        mCharSize = Canvas->TextExtent(ZXString("0"));
       	mHeadingHeight = mCharSize.cy + (mCharSize.cy / 2);
}

 __fastcall TMemoryWindow::~TMemoryWindow()
{
        if (mOffscreenBitmap)
        {
                ::DeleteObject(mOffscreenBitmap);
        }
        delete mRowRenderer;
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::SetViewMode(int mode)
{
        delete mRowRenderer;

        if (mode == MWVM_BYTE)
        {
                mRowRenderer = new ByteRowRenderer;
        }
        else if (mode == MWVM_WORD)
        {
                mRowRenderer = new WordRowRenderer;
        }
        else if (mode == MWVM_BINARY)
        {
                mRowRenderer = new BinaryRowRenderer;
        }
        else if (mode == MWVM_DECIMAL)
        {
                mRowRenderer = new DecimalRowRenderer;
        }
        else
        {
                mRowRenderer = new TraditionalRowRenderer;
        }

        mViewMode = mode;

        UpdateChanges();
}
//---------------------------------------------------------------------------

void RowRenderer::ChooseTextColour(int itemSize = 1)
{
        if (mDirty != mLast &&
            (mAddress == *mDirty || (itemSize > 1 && (mAddress+1) == *mDirty)))
        {
                SetTextColor(mCHDC, RGB(200,0,0));

                ++mDirty;

                if (itemSize > 1 && (mAddress+1) == *mDirty)
                {
                        ++mDirty;
                }
        }
        else
        {
                SetTextColor(mCHDC, GetSysColor(COLOR_WINDOWTEXT));
        }
}

void RowRenderer::AddressOut(void)
{
        SetTextColor(mCHDC, GetSysColor(COLOR_WINDOWTEXT));
        if (mAddress >= 0)
        {
                TextOut(mCHDC, 0, mY, ZXString::IntToHex(mAddress,4).c_str(), 4);
        }
        else
        {
                TextOut(mCHDC, 0, mY, ZXString::IntToHex(65536 + mAddress,4).c_str(), 4);
        }
}

bool RowRenderer::ByteAtX(const int x, int& byte)
{
        if (x <= mLMargin || x >= mLMargin + (mDisplayCellsPerRow * mCellWidth))
        {
                return false;
        }

        byte = ((x - mLMargin) / mCellWidth) * mBytesPerCell;
        return true;
}

bool TraditionalRowRenderer::ByteAtX(const int x, int& byte)
{
        int basex = mLMargin + (mDisplayCellsPerRow * mCellWidth) + mCellWidth;

        if (x >= basex && x < basex + (mDisplayCellsPerRow * 8))
        {
                byte = (x - basex) / 8;
        }
        else if (x <= mLMargin || x >= mLMargin + (mDisplayCellsPerRow * mCellWidth))
        {
                return false;
        }
        else
        {
                byte = (x - mLMargin) / mCellWidth;
        }

        return true;
}

void RowRenderer::SetGeometry(int width, const TSize& charSize, int nCharsPerCell)
{
        mKern = charSize.cx / 2;
        mCellWidth = charSize.cx * (nCharsPerCell + 1);
        mLMargin = (charSize.cx * 6);
		
        int availWidth = width - mLMargin - SBARWIDTH;

        mDisplayCellsPerRow = availWidth / mCellWidth;

        if (mDisplayCellsPerRow * mCellWidth + (charSize.cx * nCharsPerCell) < availWidth)
        {
                ++mDisplayCellsPerRow;
        }
}

void ByteRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 2);
}

void DecimalRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 3);
}

void WordRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 4);
        mBytesPerCell = 2;
}

void BinaryRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 8);
}

void TraditionalRowRenderer::SetGeometry(int width, TSize& charSize)
{
        RowRenderer::SetGeometry(width, charSize, 2);

        double availableWidth = width - mLMargin - mCellWidth - SBARWIDTH;

        float cells = 8 + mCellWidth;
        int Rw = int(availableWidth * (8 / cells));
        int Lw = int(availableWidth) - Rw;

        mDisplayCellsPerRow = Lw / mCellWidth;
}

void ByteRowRenderer::RenderRow(void)
{
        COLORREF prevBackgroundColour = GetBkColor(mCHDC);
        COLORREF paper = RGB(255,255,0);
        COLORREF ink = RGB(0,0,255);

        AddressOut();
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();

                if ((mAddress >= 0x0000) && (mAddress <= 0xFFFF))
                {
                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, paper);
                                SetTextColor(mCHDC, ink);
                        }

                        int val = getbyte(mAddress);

                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                ZXString::IntToHex(val ,2).c_str(), 2);

                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, prevBackgroundColour);
                        }
                }
                ++mAddress;
        }

        SetBkColor(mCHDC, prevBackgroundColour);
}

void WordRowRenderer::RenderRow(void)
{
        COLORREF prevBackgroundColour = GetBkColor(mCHDC);
        COLORREF paper = RGB(255,255,0);
        COLORREF ink = RGB(0,0,255);

        AddressOut();
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                std::set<int>::iterator curDirty = mDirty;
                ChooseTextColour(2);
                if (curDirty == mDirty)
                {
   //                     ++mDirty;
                }

                if ((mAddress >= 0x0000) && (mAddress <= 0xFFFF))
                {
                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, paper);
                                SetTextColor(mCHDC, ink);
                        }

                        int b1 = getbyte(mAddress);
                        int b2 = (mAddress <= 0xFFFF) ? getbyte(mAddress+1) : getbyte(0);
                        int val = b1 + 256 * b2;

                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                ZXString::IntToHex(val ,4).c_str(), 4);

                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, prevBackgroundColour);
                        }
                }
                mAddress += 2;
              //####  ++mDirty;
        }

        SetBkColor(mCHDC, prevBackgroundColour);
}

void DecimalRowRenderer::RenderRow(void)
{
        COLORREF prevBackgroundColour = GetBkColor(mCHDC);
        COLORREF paper = RGB(255,255,0);
        COLORREF ink = RGB(0,0,255);

        AddressOut();
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();

                if ((mAddress >= 0x0000) && (mAddress <= 0xFFFF))
                {
                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, paper);
                                SetTextColor(mCHDC, ink);
                        }

                        int val = getbyte(mAddress);
                        ZXString value = Format("%.3d", ARRAYOFCONST((val)));

                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                value.c_str(), 3);

                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, prevBackgroundColour);
                        }
                }
                ++mAddress;
        }

        SetBkColor(mCHDC, prevBackgroundColour);
}

void BinaryRowRenderer::RenderRow(void)
{
        COLORREF prevBackgroundColour = GetBkColor(mCHDC);
        COLORREF paper = RGB(255,255,0);
        COLORREF ink = RGB(0,0,255);

        AddressOut();
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();

                if ((mAddress >= 0x0000) && (mAddress <= 0xFFFF))
                {
                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, paper);
                                SetTextColor(mCHDC, ink);
                        }

                        int val = getbyte(mAddress);

                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                        Dbg->Bin8(val).c_str(), 8);

                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, prevBackgroundColour);
                        }
                }
                ++mAddress;
        }

        SetBkColor(mCHDC, prevBackgroundColour);
}

void TraditionalRowRenderer::RenderRow(void)
{
        HDC another = CreateCompatibleDC(mCHDC);
        HGDIOBJ oldBM = NULL;

        // yuk yuk yuk don't like it
        // options? default cset? machine->ascii mapping?

        if (machine.cset)
        {
                oldBM = SelectObject(another, (HGDIOBJ)((Graphics::TBitmap*)machine.cset)->Handle);
        }

        COLORREF prevBackgroundColour = GetBkColor(mCHDC);
        COLORREF paper = RGB(255,255,0);
        COLORREF ink = RGB(0,0,255);

        AddressOut();
        int basex = mLMargin + (mDisplayCellsPerRow * mCellWidth) + mCellWidth;
        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
                ChooseTextColour();

                int by = getbyte(mAddress);
                if ((mAddress >= 0x0000) && (mAddress <= 0xFFFF))
                {
                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, paper);
                                SetTextColor(mCHDC, ink);
                        }

                        TextOut(mCHDC, x * mCellWidth + mLMargin + mKern, mY,
                                ZXString::IntToHex(by, 2).c_str(), 2);

                        if (mSelectedAddress == mAddress)
                        {
                                SetBkColor(mCHDC, prevBackgroundColour);
                        }
                        
                        if (machine.cset)
                        {
                                int charX = by % 32;
                                int charY = by / 32;
                                int xpos = x * 8 + basex;
                                int ypos = 3 + mY;
                                BitBlt(mCHDC, xpos, ypos, 8, 8,
                                        another, charX * 8, charY * 8, SRCCOPY);

                                if (mSelectedAddress == mAddress)
                                {
                                        SetCharacterBackgroundColour(xpos, ypos, paper, ink);
                                }
                        }
                }
                ++mAddress;
        }

        SelectObject(another, oldBM);
        DeleteDC(another);

        SetBkColor(mCHDC, prevBackgroundColour);
}

void RowRenderer::SetCharacterBackgroundColour(int xpos, int ypos, COLORREF paper, COLORREF ink)
{
        for (int bx = 0; bx < 8; bx++)
        {
                for (int by = 0; by < 8; by++)
                {
                        if (GetPixel(mCHDC, xpos + bx, ypos + by) == RGB(255,255,255))
                        {
                                SetPixelV(mCHDC, xpos + bx, ypos + by, paper);
                        }
                        else
                        {
                                SetPixelV(mCHDC, xpos + bx, ypos + by, ink);
                        }
                }
        }
}

void RowRenderer::RenderColumnHeadings(const TSize& charSize)
{
//        TextOut(mCHDC, mLMargin + 2 * mKern - charSize.cy, 0, "+", 1);

        int columnInset = ((mCellWidth - charSize.cy) / 2) + mLMargin;

        for (int x = 0; x < mDisplayCellsPerRow; ++x)
        {
		ZXString heading = ZXString::IntToHex(x, 2);
                TextOut(mCHDC, (x * mCellWidth) + columnInset, 0, heading.c_str(), 2);
        }
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::SetBaseAddress(int value)
{
        mSelectedAddress = -1;
        ignoreScrollChange = true;
        ScrollBar1->Position = value;
        ignoreScrollChange = false;
}
//---------------------------------------------------------------------------

void TMemoryWindow::CreateBitmap(void)
{
        if (mOffscreenBitmap)
        {
            ::DeleteObject(mOffscreenBitmap);
        }

        RECT rect;
        ::GetClientRect(mHWND, (LPRECT) &rect);

        mBMWidth = rect.right - rect.left;
        mBMHeight = rect.bottom - rect.top - StatusBar1->Height;
        if (mBMWidth < 1)
                mBMWidth  = 1;
        if (mBMHeight < 1)
                mBMHeight  = 1;

        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);

        mOffscreenBitmap = ::CreateCompatibleBitmap(hdc, mBMWidth, mBMHeight);
        HGDIOBJ oldbm = SelectObject(chdc, mOffscreenBitmap);

        HGDIOBJ oldfont = SelectObject(chdc, Canvas->Font->Handle);

        FillRect(chdc, &rect, (HBRUSH)(COLOR_BTNFACE+1));

        SetBkColor(chdc, GetSysColor(COLOR_BTNFACE));

        mRowRenderer->SetGeometry(mBMWidth, mCharSize);

        mRows = (mBMHeight - mHeadingHeight) / mCharSize.cy;

        int bytesPerRow = mRowRenderer->BytesPerCell() * mRowRenderer->mDisplayCellsPerRow;

        ScrollBar1->SmallChange = bytesPerRow;
        ScrollBar1->LargeChange = 8 * ScrollBar1->SmallChange;

        ScrollBar1->Min = 0;
        ScrollBar1->Max = 65536 - (mRows * bytesPerRow) + bytesPerRow - 1;

        mRowRenderer->mAddress = mBaseAddress;
        mRowRenderer->mSelectedAddress = mSelectedAddress;
        mRowRenderer->mDirty = dirtyBird.lower_bound(mRowRenderer->mAddress);
        mRowRenderer->mLast = dirtyBird.end();
        mRowRenderer->mCHDC = chdc;
        mRowRenderer->mY = mHeadingHeight;

        mRowRenderer->RenderColumnHeadings(mCharSize);

        for (int i = 0; i < mRows; ++i)
        {
                mRowRenderer->RenderRow();
                mRowRenderer->mY += mCharSize.cy;
        }

        SelectObject(chdc, oldfont);
        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ScrollBar1Change(TObject *Sender)
{
        if ((ScrollBar1->Position > 0) && !ignoreScrollChange)
        {
                int delta = ScrollBar1->Position - mBaseAddress;
                int bytesPerRow = mRowRenderer->BytesPerCell() * mRowRenderer->mDisplayCellsPerRow;
                int offset = delta % bytesPerRow;

                ScrollBar1->Position -= offset;
        }

        mBaseAddress = ScrollBar1->Position;

        UpdateChanges();
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::OnEraseBkgnd (TMessage msg)
{
}
//---------------------------------------------------------------------------


void __fastcall TMemoryWindow::UpdateChanges()
{
        if (mOffscreenBitmap)
        {
            ::DeleteObject(mOffscreenBitmap);
            mOffscreenBitmap = NULL;
        }

        Invalidate();

        changeCursor = dirtyBird.lower_bound(mBaseAddress);

        bool ena = dirtyBird.size() != 0;
        ButtonFirstChange->Enabled = ena;
        ButtonPrevChange->Enabled = ena;
        ButtonNextChange->Enabled = ena;
        ButtonLastChange->Enabled = ena;
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ClearChanges()
{
        dirtyBird.clear();
        UpdateChanges();
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormPaint(TObject *Sender)
{
        if (!mOffscreenBitmap)
        {
                CreateBitmap();
        }

        HDC hdc = (HDC)Canvas->Handle;
        HDC chdc = CreateCompatibleDC(hdc);
        HGDIOBJ oldbm = SelectObject(chdc, mOffscreenBitmap);
        ::BitBlt(hdc, 0, 0, mBMWidth, mBMHeight, chdc, 0, 0, SRCCOPY);
        SelectObject(chdc, oldbm);
        DeleteDC(chdc);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormResize(TObject *Sender)
{
        if (mHWND)
        {
                UpdateChanges();

                // Required if the form has been resized using the grip control
                GlueButtonsToStatusBar();
        }
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ViewBytes1Click(TObject *Sender)
{
        TMenuItem* item = (TMenuItem*)Sender;
        ViewBytes1->Checked = false;
        ViewWords1->Checked = false;
        ViewBinary1->Checked = false;
        ViewDecimal1->Checked = false;
        ViewTraditional1->Checked = false;
        item->Checked = true;
        SetViewMode(item->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::SetAddress1Click(TObject *Sender)
{
        EditValue->CentreOn(this);

        int v = SetAddress1->Tag;
        if (EditValue->Edit2(v, 2))
        {
                SetBaseAddress(v);
        }
}
//---------------------------------------------------------------------------

bool  __fastcall TMemoryWindow::xyToAddress(int xIn, int yIn, int& address)
{
        int x;
        int y = (yIn - mHeadingHeight) / mCharSize.cy;

        if (!mRowRenderer->ByteAtX(xIn, x) || yIn < mHeadingHeight || yIn >= mHeadingHeight + (mRows * mCharSize.cy))
        {
                return false;
        }

        address = mBaseAddress + x + (y * mRowRenderer->mDisplayCellsPerRow * mRowRenderer->BytesPerCell());

        return (address <= 0xFFFF);
}

void __fastcall TMemoryWindow::FormClick(TObject *Sender)
{
        int address;
        TPoint cp = ScreenToClient(Mouse->CursorPos);

        // CR  this is nasty - but it's the quickest way to tell if the debugger  
        // is running continuously
        if (Dbg->SingleStep->Enabled == false) return;

        if (xyToAddress(cp.x, cp.y, address))
        {
                EditValue->Top = Mouse->CursorPos.y;
                EditValue->Left = Mouse->CursorPos.x;

                char format = '$';
                if (mViewMode == MWVM_BINARY)
                {
                        format = '%';
                }
                else if (mViewMode == MWVM_DECIMAL)
                {
                        format = '0';
                }

                if (mViewMode != MWVM_WORD)
                {
                        int n = getbyte(address);
                        if (EditValue->Edit2(n, 1, format))
                        {
                                setbyte(address,n);
                                UpdateChanges();  // CR  refresh after edit
                        }
                }
                else
                {
                        int n = getbyte(address) + 256 * getbyte(address + 1);
                        if (EditValue->Edit2(n, 2, format))
                        {
                                setbyte(address, n & 255);
                                setbyte(address+1, n >> 8);
                                UpdateChanges();  // CR  refresh after edit
                        }
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
        int address;
        ZXString t;
        mSelectedAddress = -1;

        if (xyToAddress(X,Y,address))
        {
                t = "$" + ZXString::IntToHex(address,4);
                mSelectedAddress = address;
        }

        StatusBar1->Panels->Items[4]->Text = t;

        UpdateChanges();
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        JumpTo1->Caption = "Jump To";

        int address = 0;
        bool gotAddress = xyToAddress(X,Y,address);
        if (gotAddress)
        {
                SetAddress1->Tag = address;

                // indirect address

                address = getbyte(address) + 256 * getbyte(address + 1);
                JumpTo1->Caption += " $" + ZXString::IntToHex(address,4);
                JumpTo1->Tag = address;
         }
        else
        {
                SetAddress1->Tag = mBaseAddress;
        }

        JumpTo1->Enabled = gotAddress;
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::JumpTo1Click(TObject *Sender)
{
        SetBaseAddress(JumpTo1->Tag);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::SetSBButtonPosition(TButton* btn, int idx)
{
        RECT r;
        StatusBar1->Perform(SB_GETRECT, idx, (LPARAM)&r);

        // Detach the button and then re-attach it (required to ensure the button
	// stays in the status bar when the form is resized using the grip control)
        btn->Parent = NULL;		
        btn->Parent = StatusBar1;
		
        btn->Top = r.top;
        btn->Left = r.left;
        btn->Width = r.right - r.left;
        btn->Height = r.bottom - r.top;
}

void __fastcall TMemoryWindow::FormShow(TObject *Sender)
{
        GlueButtonsToStatusBar();
}

void __fastcall TMemoryWindow::GlueButtonsToStatusBar()
{
        // glue the change navigation buttons to the status bar
        //
        SetSBButtonPosition(ButtonFirstChange, 0);
        SetSBButtonPosition(ButtonPrevChange, 1);
        SetSBButtonPosition(ButtonNextChange, 2);
        SetSBButtonPosition(ButtonLastChange, 3);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonChangeMouseMove(TObject *Sender,
      TShiftState Shift, int X, int Y)
{
		const ZXString btnDescs[] =
        {
                "First", "Previous", "Next", "Last"
        };

        StatusBar1->Panels->Items[4]->Text = btnDescs[((TButton*)Sender)->Tag] + " Change";
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonFirstChangeClick(TObject *Sender)
{
        changeCursor = dirtyBird.begin();
        SetBaseAddress(*changeCursor);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonPrevChangeClick(TObject *Sender)
{
       // find the change before the one that is greater or equal
       // to the start address
       //
       changeCursor = dirtyBird.lower_bound(mBaseAddress);
        if (changeCursor != dirtyBird.begin())
        {
                --changeCursor;
        }
        SetBaseAddress(*changeCursor);
}

//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonNextChangeClick(TObject *Sender)
{
        // find the next change greater than the base address
        //
        changeCursor = dirtyBird.upper_bound(mBaseAddress);
        SetBaseAddress(*changeCursor);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ButtonLastChangeClick(TObject *Sender)
{
        changeCursor = dirtyBird.end();
        --changeCursor;
        SetBaseAddress(*changeCursor);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::IncDecAddressChangingEx(TObject *Sender,
      bool &AllowChange, short NewValue, TUpDownDirection Direction)
{
     AllowChange = false;

     SetBaseAddress(mBaseAddress + NewValue);
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        if (Shift.Contains(ssCtrl) && (Key == 'F'))
        {
                if (Shift.Contains(ssShift))
                {
                        std::vector<int> bytes;
                        bool performSearch = SearchSequence->ValidateSearchSequence(bytes);
                        if (performSearch)
                        {
                                PerformSearch(bytes);
                        }
                }
                else
                {
                        DoSearch();
                }
        }
}
//---------------------------------------------------------------------------
bool TMemoryWindow::FindSequence(std::vector<int>& bytes, int& addr)
{
        bool found;

        do
        {
                found = FindMatch(addr, bytes);
        }
        while (!found && (addr < 65536));

        return found;
}

bool TMemoryWindow::FindMatch(int& addr, std::vector<int>& bytes)
{
        int firstByte = bytes[0];
        
        while ((addr < 65536) && (getbyte(addr) != firstByte))
        {
                ++addr;
        }

        if (addr == 65536)
        {
                return false;
        }

        for (int b = 1; b < (int)bytes.size(); b++)
        {
                ++addr;
                int byte = getbyte(addr);

                if (byte != bytes[b])
                {
                        return false;
                }
        }

        
        addr -= bytes.size() - 1;

        return true;
}
//---------------------------------------------------------------------------
void TMemoryWindow::DoSearch()
{
        SearchSequence->CentreOn(this);

        std::vector<int> bytes;
        bool performSearch = SearchSequence->EditSequenceBytes(mSearchText, bytes);
        if (performSearch)
        {
                PerformSearch(bytes);
        }
}

void TMemoryWindow::PerformSearch(std::vector<int>& bytes)
{
        int address = mBaseAddress + 1;
        
        if ((address < 65536) && FindSequence(bytes, address))
        {
                SetBaseAddress(address);
        }
}

void __fastcall TMemoryWindow::Search1Click(TObject *Sender)
{
         DoSearch();
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::FormMouseWheel(TObject *Sender,
      TShiftState Shift, int WheelDelta, TPoint &MousePos, bool &Handled)
{
        int currentPos = ScrollBar1->Position;
        int newPos = currentPos - (ScrollBar1->SmallChange * (WheelDelta / 120));

        if (newPos < ScrollBar1->Min)
        {
                newPos = ScrollBar1->Min;
        }

        if (newPos > ScrollBar1->Max)
        {
                newPos = ScrollBar1->Max;
        }

        ScrollBar1->Position = newPos;

        Handled = true;
}
//---------------------------------------------------------------------------

void __fastcall TMemoryWindow::ClearHighlightsClick(TObject *Sender)
{
        ClearChanges();
}
//---------------------------------------------------------------------------


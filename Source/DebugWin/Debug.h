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
 * Debug.h
 *
 */

//---------------------------------------------------------------------------

#ifndef DebugH
#define DebugH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <Grids.hpp>
#include <IniFiles.hpp>
#include <Menus.hpp>
#include "breakpoint.h"
#include <ComCtrls.hpp>
#include <ExtCtrls.hpp>

//---------------------------------------------------------------------------

enum IODirection
{
        IO_NONE,
        IO_OUT,
        IO_IN
};

struct LastIOAccess
{
        IODirection direction;
        int address;
        BYTE data;
};

extern void DebugUpdate(void);
extern void ResetLastIOAccesses();
extern void LogInAccess(int address, BYTE data);
extern void LogOutAccess(int address, BYTE data);

class TDbg : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TLabel *HL;
        TLabel *BC;
        TLabel *DE;
        TLabel *A;
        TLabel *F;
        TLabel *Label6;
        TLabel *HL_;
        TLabel *BC_;
        TLabel *DE_;
        TLabel *A_;
        TLabel *F_;
        TLabel *Label12;
        TLabel *Label13;
        TLabel *Label14;
        TLabel *Label15;
        TLabel *Label16;
        TLabel *IX;
        TLabel *Label18;
        TLabel *IY;
        TLabel *Label20;
        TLabel *IR;
        TLabel *Label22;
        TLabel *SP;
        TLabel *Label24;
        TLabel *PC;
        TLabel *Label7;
        TLabel *IM;
        TLabel *Label9;
        TLabel *Interrupts;
        TLabel *Label11;
        TLabel *Halt;
        TGroupBox *GroupBox2;
        TLabel *Disass0;
        TLabel *Disass1;
        TLabel *Disass2;
        TLabel *Disass3;
        TLabel *Disass4;
        TLabel *Disass5;
        TLabel *Disass6;
        TLabel *Disass7;
        TLabel *Disass8;
        TLabel *Disass9;
        TGroupBox *GroupBoxZX81;
        TLabel *Label39;
        TLabel *Label10;
        TLabel *Label19;
        TLabel *Label21;
        TLabel *Label23;
        TLabel *NMIGen;
        TLabel *Scanline;
        TLabel *RowCount;
        TLabel *ShiftReg;
        TGroupBox *GroupBox4;
        TLabel *SyncOutput;
        TGroupBox *GroupBox5;
        TLabel *Stack0;
        TLabel *Stack1;
        TLabel *Stack2;
        TLabel *Stack3;
        TLabel *Stack4;
        TLabel *Stack5;
        TLabel *Stack6;
        TButton *AddBrkBtn;
        TButton *DelBrkBtn;
        TStringGrid *BPList;
        TLabel *Label27;
        TLabel *TStates;
        TGroupBox *GroupBoxAce;
        TLabel *AceStk0;
        TLabel *AceStk1;
        TLabel *AceStk2;
        TLabel *AceStk3;
        TLabel *AceStk4;
        TLabel *AceStk5;
        TLabel *AceStk6;
        TLabel *AceStk7;
        TLabel *Label37;
        TLabel *Label38;
        TLabel *Label40;
        TLabel *Label41;
        TLabel *Label42;
        TLabel *Label43;
        TLabel *Label44;
        TLabel *Label45;
        TLabel *AceStkVal0;
        TLabel *AceStkVal1;
        TLabel *AceStkVal2;
        TLabel *AceStkVal3;
        TLabel *AceStkVal4;
        TLabel *AceStkVal5;
        TLabel *AceStkVal6;
        TLabel *AceStkVal7;
        TLabel *Label28;
        TLabel *Label29;
        TPopupMenu *MemDumpPopup;
        TMenuItem *MemDumpFromHere1;
        TLabel *Label8;
        TMenuItem *OnExecute1;
        TMenuItem *OnRead1;
        TMenuItem *OnWrite1;
        TButton *EditBrkBtn;
        TGroupBox *Execute;
        TButton *SingleStep;
        TButton *StepOver;
        TButton *RunStop;
        TCheckBox *Continuous;
        TCheckBox *SkipNMIBtn;
        TCheckBox *SkipINTBtn;
        TGroupBox *GroupBox3;
        TCheckBox *EnableHistory;
        TButton *History;
        TGroupBox *IOPorts;
        TButton *Memory;
        TButton *Symbols;
        TLabel *SymRom;
        TLabel *SymApp;
        TLabel *IOPort0Address;
        TLabel *IOPort2Address;
        TLabel *IOPort1Address;
        TLabel *IOPort3Address;
        TLabel *IOPort0Data;
        TLabel *IOPort0Direction;
        TLabel *IOPort2Data;
        TLabel *IOPort1Data;
        TLabel *IOPort3Data;
        TLabel *IOPort3Direction;
        TLabel *IOPort1Direction;
        TLabel *IOPort2Direction;
        TGroupBox *GroupBoxChroma;
        TLabel *ChromaColourModeLabel;
        TLabel *ChromaColourMode;
        TGroupBox *GroupBoxZXC;
        TLabel *ZXCModeLabel;
        TLabel *ZXCMode;
        TGroupBox *GroupBoxSpectra;
        TLabel *SpectraModeLabel;
        TLabel *SpectraMode;
        TLabel *Label17;
        TLabel *Label25;
        TLabel *Label26;
        TLabel *Label30;
        TLabel *Disass10;
        TLabel *Disass11;
        TLabel *Label31;
        TLabel *TStatesCount;
        TButton *ButtonProfiler;
        TButton *StepOut;
        TPopupMenu *InputContextPopup;
        TMenuItem *BreakOnInput;
        TMenuItem *BreakOnInputLow;
        TMenuItem *BreakOnInputHigh;
        TPopupMenu *OutputContextPopup;
        TMenuItem *BreakOnOutput;
        TMenuItem *BreakOnOutputHigh;
        TMenuItem *BreakOnOutputLow;
        TPopupMenu *BreakpointWindowPopup;
        TMenuItem *Disable;
        TMenuItem *Enable;
        TMenuItem *ResetHitCount;
        TMenuItem *N1;
        TMenuItem *CurrentHitCount;
        TButton *ResetBrkBtn;
        TCheckBox *AutoUpdateMemory;
        TTimer *MemoryWindowTimer;
        void __fastcall RunStopClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall ContinuousClick(TObject *Sender);
        void __fastcall SingleStepClick(TObject *Sender);
        void __fastcall StepOverClick(TObject *Sender);
        void __fastcall DelBrkBtnClick(TObject *Sender);
        void __fastcall SkipNMIBtnClick(TObject *Sender);
        void __fastcall SkipINTBtnClick(TObject *Sender);
        void __fastcall HistoryClick(TObject *Sender);
        void __fastcall NMIGenClick(TObject *Sender);
        void __fastcall SyncOutputClick(TObject *Sender);
        void __fastcall HLClick(TObject *Sender);
        void __fastcall BCClick(TObject *Sender);
        void __fastcall DEClick(TObject *Sender);
        void __fastcall HL_Click(TObject *Sender);
        void __fastcall DE_Click(TObject *Sender);
        void __fastcall BC_Click(TObject *Sender);
        void __fastcall IXClick(TObject *Sender);
        void __fastcall IYClick(TObject *Sender);
        void __fastcall SPClick(TObject *Sender);
        void __fastcall PCClick(TObject *Sender);
        void __fastcall AClick(TObject *Sender);
        void __fastcall A_Click(TObject *Sender);
        void __fastcall InterruptsClick(TObject *Sender);
        void __fastcall IMClick(TObject *Sender);
        void __fastcall Stack0Click(TObject *Sender);
        void __fastcall IRClick(TObject *Sender);
        void __fastcall FClick(TObject *Sender);
        void __fastcall F_Click(TObject *Sender);
        void __fastcall BCMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall DEMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall HLMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall BC_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall DE_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall HL_MouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall IXMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall IYMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall SPMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall PCMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall MemDumpFromHere1Click(TObject *Sender);
        void __fastcall Stack0MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall IRMouseDown(TObject *Sender, TMouseButton Button,
          TShiftState Shift, int X, int Y);
        void __fastcall AceStk0Click(TObject *Sender);
        void __fastcall AceStkVal0Click(TObject *Sender);
        void __fastcall AddBrkBtnClick(TObject *Sender);
        void __fastcall SymbolsClick(TObject *Sender);
        void __fastcall AddBreak1Click(TObject *Sender);
        void __fastcall MemoryClick(TObject *Sender);
        void __fastcall EditBrkBtnClick(TObject *Sender);
        void __fastcall BPListSelectCell(TObject *Sender, int ACol,
          int ARow, bool &CanSelect);
        void __fastcall Disass3MouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall ButtonProfilerClick(TObject *Sender);
        void __fastcall StepOutClick(TObject *Sender);
        void __fastcall FormKeyDown(TObject *Sender, WORD &Key,
          TShiftState Shift);
        void __fastcall BreakOnInputOutputClick(TObject *Sender);
        void __fastcall BreakOnInputOutputHighClick(TObject *Sender);
        void __fastcall BreakOnInputOutputLowClick(TObject *Sender);
        void __fastcall IOPort0AddressMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall IOPort1AddressMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall IOPort2AddressMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall IOPort3AddressMouseDown(TObject *Sender,
          TMouseButton Button, TShiftState Shift, int X, int Y);
        void __fastcall BPListDrawCell(TObject *Sender, int ACol, int ARow,
          TRect &Rect, TGridDrawState State);
        void __fastcall BPListDblClick(TObject *Sender);
        void __fastcall DisableClick(TObject *Sender);
        void __fastcall EnableClick(TObject *Sender);
        void __fastcall BPListContextPopup(TObject *Sender,
          TPoint &MousePos, bool &Handled);
        void __fastcall ResetBrkBtnClick(TObject *Sender);
        void __fastcall ResetHitCountClick(TObject *Sender);
        void __fastcall AutoUpdateMemoryClick(TObject *Sender);
        void __fastcall MemoryWindowTimerExpired(TObject *Sender);
private:	// User declarations
        void EnableValues(bool enable);
        void EnableVals(void);
        void DisableVals(void);
        void SetMenuContent(int memloc);
        void SetIoContextPopupContent(ZXString ioPort);
        ZXString StrRep(ZXString Text, int Pos, int Len, ZXString NewText);

        struct breakpoint Breakpoint[99];
        int Breakpoints;

        // default values are evil. never ever use them >:)
        void SetLabelInfo(TLabel* label, int value, int valueWidth = 4);

        void __fastcall DoEditStack(int);
        void __fastcall DoEditReg(WORD&);
        void __fastcall DoEditReg(BYTE&);

        void PopulateHistoryWindow();
        int getRegisterValue(int registerIndex);
        ZXString ConstructRegisterBreakpointText(breakpoint* const bp);
        ZXString ConstructExeBreakpointText(breakpoint* const bp);
        ZXString ConstructTStatesBreakpointText(breakpoint* const bp);
        ZXString ConstructFlagBreakpointText(breakpoint* const bp);
        ZXString ConstructLowIOBreakpointText(ZXString type, breakpoint* const bp);
        ZXString ConstructHighIOBreakpointText(ZXString type, breakpoint* const bp);
        ZXString ConstructRdWrInOutMemBreakpointText(ZXString type, breakpoint* const bp);
        ZXString GetBreakpointText(breakpoint* const bp);

        bool BPInOutHit(BreakpointType type, int addr, int value, breakpoint* const bp);
        bool BPTCyclesHit(int addr, breakpoint* const bp);
        bool BPReadWriteHit(BreakpointType type, int addr, int value, breakpoint* const bp);
        bool BPFlagValueHit(breakpoint* const bp);
        bool BPMemoryValueHit(breakpoint* const bp);
        bool BPRegisterValueHit(breakpoint* const bp);
        bool IsStepOverInstruction(int Addr);
        void EditBreakpoint();
        void UpdateBreakpointButtons();

public:		// User declarations
        __fastcall TDbg(TComponent* Owner);
        int NMIRetAddr, INTRetAddr;
        void UpdateVals(void);
        bool DoNext;

        bool TStatesBreakPointHit(int Addr);
        void DelTempBreakPoints(void);
        bool BreakPointHit();
        bool BPExeHit(int addr, breakpoint* const bp);
        void RefreshBreakpointList();
        void ResetBreakpointHits();
        void ClearSkipAddresses();
        void Reset();

        bool AddBreakPoint(struct breakpoint& bp);
        int FindBreakPointEntry(int index, struct breakpoint& bp, bool editing);
        void DelBreakPoint(int index);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
        ZXString DisassembleAddress(int* Ad);
        ZXString Disassemble(int addr, BYTE* bytes);
        ZXString Disassemble(int *Ad);
        ZXString Hex16(int value);
        ZXString Hex8(int value);
        ZXString Bin8(int Value);
        int Hex2Dec(ZXString num);

        LastIOAccess lastIOAccess[4];
        int lastPortInAddr, lastPortOutAddr;
        int lastPortInValue, lastPortOutValue;

        void ReloadHistoryWindow();
        void ClearHistoryWindow();
        void DisableMemoryWindowAutoUpdates();
};
//---------------------------------------------------------------------------
extern PACKAGE TDbg *Dbg;
//---------------------------------------------------------------------------
#endif

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
 * Debug.cpp
 */

//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "Debug.h"
#include "zx81.h"
#include "z80.h"
#include "zx81config.h"
#include "hist.h"

#include "main_.h"
#include "EditValue_.h"
#include "SetBreakpoint_.h"
#include "SearchSequence_.h"
#include "EditFlags.h"
#include "memoryWindow.h"
#include "symbolstore.h"
#include "SymBrowse.h"
#include "Profiler.h"
#include "DbgDissassem.cpp"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDbg *Dbg;

// TODO
//
// ability to disable the breakpoint w/o deleting it
// re-calculate breakpoints if symbols change???

extern int frametstates, lineCounter, RasterY;
extern BOOL syncOutputWhite, nmiGeneratorEnabled;
extern int tStatesCount;
extern unsigned char shift_store;

extern unsigned char memory[];
extern int lastMemoryReadAddrLo, lastMemoryWriteAddrLo;
extern int lastMemoryReadAddrHi, lastMemoryWriteAddrHi;
extern int lastMemoryReadValueLo, lastMemoryWriteValueLo;
extern int lastMemoryReadValueHi, lastMemoryWriteValueHi;

const int maxInstructionBytes = 4;		// Max instruction size should be 4 but in theory could be longer if there are repeated prefixes
const int maxBreakpoints = 99;

struct InstructionEntry
{
	int Address;
	BYTE Bytes[maxInstructionBytes];
};

const int historySize = 3000;
InstructionEntry HistoryLog[historySize];
int HistoryPos=0;
bool historyWrappedAround = false;

int recentHistory[4];
int recentHistoryPos=0;

int displayedTStatesCount;

int StepOverStack;
int StepOverInstructionSize;
int StepOverAddr;
int StepOverStartAddr;
bool StepOverInstruction;
int StepOutRequested;
int RetExecuted;
int StackChange;

void DebugUpdate(void)
{
        static int lastpc;
        int i;

        if (z80.pc.w==0x66 && Dbg->SkipNMIBtn->Checked)
        {
                Dbg->NMIRetAddr = getbyte(z80.sp.w) + 256*getbyte(z80.sp.w + 1);
        }
        if (z80.pc.w == Dbg->NMIRetAddr || !Dbg->SkipNMIBtn->Checked)
        {
                Dbg->NMIRetAddr=-1;
        }

        if (z80.pc.w==0x38 && Dbg->SkipINTBtn->Checked)
        {
                Dbg->INTRetAddr = getbyte(z80.sp.w) + 256*getbyte(z80.sp.w + 1);
        }
        if (z80.pc.w == Dbg->INTRetAddr || !Dbg->SkipINTBtn->Checked)
        {
                Dbg->INTRetAddr=-1;
        }

        i=z80.pc.w;
        if (lastpc!=z80.pc.w)
        {
                recentHistory[recentHistoryPos] = i;
                recentHistoryPos = (recentHistoryPos + 1) & 3;

                if (Dbg->EnableHistory->Checked)
                {
                        bool show0K8KAddresses = HistoryBox->Show0K8KAddresses();
                        bool show8K16KAddresses = HistoryBox->Show8K16KAddresses();
                        bool show16K32KAddresses = HistoryBox->Show16K32KAddresses();
                        bool show32K48KAddresses = HistoryBox->Show32K48KAddresses();
                        bool show48K64KAddresses = HistoryBox->Show48K64KAddresses();
                        bool includeInstruction = (show0K8KAddresses   && (i <= 0x1FFF)) ||
                                                  (show8K16KAddresses  && (i >= 0x2000) && (i <= 0x3FFF)) ||
                                                  (show16K32KAddresses && (i >= 0x4000) && (i <= 0x7FFF)) ||
                                                  (show32K48KAddresses && (i >= 0x8000) && (i <= 0xBFFF)) ||
                                                  (show48K64KAddresses && (i >= 0xC000) && (i <= 0xFFFF));

                        if (includeInstruction && Dbg->NMIRetAddr==-1 && Dbg->INTRetAddr==-1)
                        {
                                HistoryLog[HistoryPos].Address = i;

		        	for (int i = 0; i < maxInstructionBytes; ++i)
		        	{
			        	// The instruction bytes must be saved in case these address locations are subsequently overwritten,
			                // i.e. by self modifying code
		                	HistoryLog[HistoryPos].Bytes[i] = getbyte(z80.pc.w + i);
		    	        }
		    	        ++HistoryPos;
                                if (HistoryPos==historySize)
                                {
                                        HistoryPos=0;
                                        historyWrappedAround = true;
                                }
                        }
                }

                lastpc=z80.pc.w;
        }

        displayedTStatesCount = tStatesCount;

        if (Dbg->BreakPointHit() || (RetExecuted && StepOutRequested && (StackChange < 0)))
        {
                StepOutRequested = 0;
                Dbg->DoNext=false;
                Dbg->UpdateVals();
                Dbg->RunStopClick(NULL);
        }

        if (Dbg->NMIRetAddr!=-1 || Dbg->INTRetAddr!=-1) return;

        if (Dbg->DoNext)
        {
                emulation_stop=1;
                Dbg->DoNext=false;
                Dbg->UpdateVals();
                emulator.single_step = (CFGBYTE)(Dbg->Continuous->Checked ? 1 : 0);
        }

        Profiler->DebugTick(&z80);

        if (Dbg->Continuous->Checked==true && Dbg->Visible==true)
                Dbg->UpdateVals();

        Dbg->Symbols->Enabled = symbolstore::romLoaded() || symbolstore::fileLoaded();
        Dbg->SymRom->Enabled = symbolstore::romLoaded();
        Dbg->SymApp->Enabled = symbolstore::fileLoaded();
}

int TDbg::FindBreakPointEntry(int index, struct breakpoint& bp, bool editing)
{
        int breakpointIndex = -1;

        for (int i=0; i<Breakpoints; i++)
        {
                bool existingBreakpoint = false;

                switch (bp.Type)
                {
                        case BP_TCYCLES:
                                existingBreakpoint = !editing &&
                                                     (Breakpoint[i].Type == bp.Type);
                                break;

                        case BP_REGISTER:
                                existingBreakpoint = (Breakpoint[i].Type == bp.Type) &&
                                                     (Breakpoint[i].RegisterId == bp.RegisterId) &&
                                                     (Breakpoint[i].ConditionValue == bp.ConditionValue) &&
                                                     (Breakpoint[i].Value == bp.Value);
                                break;

                        case BP_FLAG:
                                existingBreakpoint = !editing &&
                                                     (Breakpoint[i].Type == bp.Type) &&
                                                     (Breakpoint[i].FlagId == bp.FlagId);
                                break;

                        case BP_MEMORY:
                                existingBreakpoint = (Breakpoint[i].Type == bp.Type) &&
                                                     (Breakpoint[i].Addr == bp.Addr) &&
                                                     (Breakpoint[i].ConditionValue == bp.ConditionValue) &&
                                                     (Breakpoint[i].Value == bp.Value);
                                break;

                        case BP_RD:
                        case BP_WR:
                        case BP_EXE:
                        case BP_OUT:
                        case BP_OUTL:
                        case BP_OUTH:
                        case BP_IN:
                        case BP_INL:
                        case BP_INH:
                                existingBreakpoint = (Breakpoint[i].Type == bp.Type) &&
                                                     (Breakpoint[i].ConditionAddr == bp.ConditionAddr) &&
                                                     (Breakpoint[i].Addr == bp.Addr) &&
                                                     (Breakpoint[i].ConditionValue == bp.ConditionValue) &&
                                                     (Breakpoint[i].Value == bp.Value);
                                break;
                }

                if (existingBreakpoint && (!editing || (editing && (index != i))))
                {
                        breakpointIndex = i;
                        break;
                }
        }

        return breakpointIndex;
}

bool TDbg::AddBreakPoint(struct breakpoint& bp)
{
        if (Breakpoints == maxBreakpoints)
                return false;

        const bool editingState = false;
        int breakpointIndex = FindBreakPointEntry(-1, bp, editingState);
        if (breakpointIndex != -1)
                DelBreakPoint(breakpointIndex);

        Breakpoint[Breakpoints] = bp;

        ZXString str = GetBreakpointText(&bp);

        BPList->Rows[Breakpoints]->Text = str;

        Breakpoints++;
        BPList->RowCount++;
        if (BPList->RowCount > 1)
                BPList->Row = BPList->RowCount - 2;

        UpdateBreakpointButtons();

        return true;
}

ZXString TDbg::GetBreakpointText(breakpoint* const bp)
{
        ZXString str;
                        
        switch (bp->Type)
        {
                case BP_EXE:
                        str = ConstructExeBreakpointText(bp);
                        break;

                case BP_RD:
                        str = ConstructRdWrInOutMemBreakpointText("RD ", bp);
                        break;

                case BP_WR:
                        str = ConstructRdWrInOutMemBreakpointText("WR ", bp);
                        break;

                case BP_IN:
                        str = ConstructRdWrInOutMemBreakpointText("IN ", bp);
                        break;

                case BP_OUT:
                        str = ConstructRdWrInOutMemBreakpointText("OUT", bp);
                        break;

                case BP_MEMORY:
                        str = ConstructRdWrInOutMemBreakpointText("MEM", bp);
                        break;

                case BP_INL:
                        str = ConstructLowIOBreakpointText("IN ", bp);
                        break;

                case BP_INH:
                        str = ConstructHighIOBreakpointText("IN ", bp);
                        break;

                case BP_OUTL:
                        str = ConstructLowIOBreakpointText("OUT", bp);
                        break;

                case BP_OUTH:
                        str = ConstructHighIOBreakpointText("OUT", bp);
                        break;

                case BP_TCYCLES:
                        str = ConstructTStatesBreakpointText(bp);
                        break;

                case BP_REGISTER:
                        str = ConstructRegisterBreakpointText(bp);
                        break;

                case BP_FLAG:
                        str = ConstructFlagBreakpointText(bp);
                        break;
        }

        if (!bp->Permanent)
        {
                str = "{" + str + "}";
        }
        else
        {
                str += "             ";
                str = str.SubString(1, 23);

                if (bp->HitCount > 1)
                {
                        str += " * ";
                        str += bp->HitCount;
                }
        }

        return str;
}

static ZXString GetConditionAddr(BreakpointCondition condition)
{
        const ZXString conditions[] = { " = ", " <>", " <=", " >=", " ->", " @ " };

        return conditions[condition];
}

static ZXString GetConditionValue(BreakpointCondition condition)
{
        const ZXString conditions[] = { " = ", " <>", " <=", " >=", "..." };

        return conditions[condition];
}

ZXString TDbg::ConstructTStatesBreakpointText(breakpoint* const bp)
{
        ZXString ca = GetConditionAddr(bp->ConditionAddr);
        ZXString str = "CYC" + ca + "$" + Hex16(bp->Addr) + " <>" + bp->TStates;
        return str;
}

static ZXString Pad(ZXString string)
{
        ZXString paddedString = string + "    ";
        return paddedString.SubString(1, 5);
}

ZXString TDbg::ConstructFlagBreakpointText(breakpoint* const bp)
{
        ZXString FlagNames[] = {"C", "N", "P/V", "Bit3", "H", "Bit5", "Z", "S"};

        ZXString ca = GetConditionAddr(bp->ConditionAddr);
        ZXString cv = GetConditionValue(bp->ConditionValue);
        ZXString str = "FLG" + ca + Pad(FlagNames[bp->FlagId]) + cv + bp->Value;

        return str;
}

ZXString TDbg::ConstructRegisterBreakpointText(breakpoint* const bp)
{
        ZXString RegisterNames[] = {"BC", "DE", "HL", "IX", "IY", "PC", "SP", "BC'", "DE'", "HL'",
                                      "A", "B", "C", "D", "E", "H", "L", "I", "R", "IXh", "IXl", "IYh", "IYl",
                                      "A'", "B'", "C'", "D'", "E'", "H'", "L'"};

        ZXString ca = GetConditionAddr(bp->ConditionAddr);
        ZXString cv = GetConditionValue(bp->ConditionValue);
        ZXString str = "REG" + ca + Pad(RegisterNames[bp->RegisterId]) + cv + "$";
        if (bp->RegisterId >= RegA)
        {
                str += Hex8(bp->Value);
        }
        else
        {
                str += Hex16(bp->Value);
        }

        return str;
}

ZXString TDbg::ConstructLowIOBreakpointText(ZXString type, breakpoint* const bp)
{
        ZXString ca = GetConditionAddr(bp->ConditionAddr);
        ZXString cv = GetConditionValue(bp->ConditionValue);
        ZXString str = type + ca + "$xx" + Hex8(bp->Addr) + cv + "$" + Hex8(bp->Value);
        return str;
}

ZXString TDbg::ConstructHighIOBreakpointText(ZXString type, breakpoint* const bp)
{
        ZXString ca = GetConditionAddr(bp->ConditionAddr);
        ZXString cv = GetConditionValue(bp->ConditionValue);
        ZXString str = type + ca + "$" + Hex8(bp->Addr >> 8) + "xx" + cv + "$" + Hex8(bp->Value);
        return str;
}

ZXString TDbg::ConstructRdWrInOutMemBreakpointText(ZXString type, breakpoint* const bp)
{
        ZXString ca = GetConditionAddr(bp->ConditionAddr);
        ZXString str = type + ca + "$" + Hex16(bp->Addr);

        if (bp->ConditionAddr == Range)
        {
                str += "...$" + Hex16(bp->EndAddr);
        }
        else
        {
                ZXString cv = GetConditionValue(bp->ConditionValue);
                str += cv + "$" + Hex8(bp->Value);
        }
        
        return str;
}

ZXString TDbg::ConstructExeBreakpointText(breakpoint* const bp)
{
        ZXString ca = GetConditionAddr(bp->ConditionAddr);
        ZXString str = "EXE" + ca + "$" + Hex16(bp->Addr);

        if (bp->ConditionAddr == Range)
        {
                str += "...$" + Hex16(bp->EndAddr);
        }
        else
        {
                ZXString symbol;
                if (symbolstore::addressToSymbol(bp->Addr, symbol))
                {
                        str += " " + symbol;
                }
        }

        return str;
}

void TDbg::DelBreakPoint(int index)
{
        int j;
        for(j=index; j<Breakpoints;j++)
        {
                Breakpoint[j] = Breakpoint[j+1];
                BPList->Cells[0][j] = BPList->Cells[0][j+1];
        }
        Breakpoints--;
        BPList->RowCount--;

        if ((BPList->Row > 0) && ((BPList->Row + 1) >= BPList->RowCount))
        {
                BPList->Row--;
        }
}

bool TDbg::BreakPointHit()
{
        int lmrl = lastMemoryReadAddrLo;
        int lmrlv = lastMemoryReadValueLo;
        lastMemoryReadAddrLo = -1;

        int lmrh = lastMemoryReadAddrHi;
        int lmrhv = lastMemoryReadValueHi;
        lastMemoryReadAddrHi = -1;

        int lmwl = lastMemoryWriteAddrLo;
        int lmwlv = lastMemoryWriteValueLo;
        lastMemoryWriteAddrLo = -1;

        int lmwh = lastMemoryWriteAddrHi;
        int lmwhv = lastMemoryWriteValueHi;
        lastMemoryWriteAddrHi = -1;

        int lpi = Dbg->lastPortInAddr;
        int lpiv = Dbg->lastPortInValue;
        Dbg->lastPortInAddr = -1;

        int lpo = Dbg->lastPortOutAddr;
        int lpov = Dbg->lastPortOutValue;
        Dbg->lastPortOutAddr = -1;

        for (int idx = 0; idx < Breakpoints; ++idx)
        {
		breakpoint* bp = &Breakpoint[idx];

 		if (Dbg->BPExeHit(z80.pc.w, bp) ||
                    Dbg->BPInOutHit(BP_IN, lpi, lpiv, bp) ||
                    Dbg->BPInOutHit(BP_INL, lpi, lpiv, bp) ||
                    Dbg->BPInOutHit(BP_INH, lpi, lpiv, bp) ||
                    Dbg->BPInOutHit(BP_OUT, lpo, lpov, bp) ||
                    Dbg->BPInOutHit(BP_OUTL, lpo, lpov, bp) ||
                    Dbg->BPInOutHit(BP_OUTH, lpo, lpov, bp) ||
                    Dbg->BPReadWriteHit(BP_WR, lmwh, lmwhv, bp) ||
                    Dbg->BPReadWriteHit(BP_WR, lmwl, lmwlv, bp) ||
                    Dbg->BPReadWriteHit(BP_RD, lmrh, lmrhv, bp) ||
                    Dbg->BPReadWriteHit(BP_RD, lmrl, lmrlv, bp) ||
                    Dbg->BPRegisterValueHit(bp) ||
                    Dbg->BPFlagValueHit(bp) ||
                    Dbg->BPMemoryValueHit(bp) ||
                    Dbg->BPTCyclesHit(z80.pc.w, bp))
		{
                        if (!bp->Permanent)
                        {
                                DelBreakPoint(idx);
                                return true;
                        }

                        BPList->Row = idx;

                        bp->Hits++;
                        UpdateBreakpointButtons();

                        if (bp->Hits >= bp->HitCount)
                        {
                                bp->Hits = 0;
                                return true;
                        }
		}
	}

	return false;
}

bool TDbg::BPExeHit(int addr, breakpoint* const bp)
{
        if (bp->HitExe(BP_EXE, addr))
	{
                breakpoint* lastBP = &Breakpoint[Breakpoints - 1];

                if (bp->Permanent)
                {
                        if (lastBP->Permanent ||
                           (!lastBP->Permanent && (((lastBP->Addr - bp->Addr) != StepOverInstructionSize) || (bp->ConditionAddr != Equal))))
                        {
                                return true;
                        }
                }
                else
                {
                        if (!bp->Permanent && (StepOverStack == z80.sp.w))
                        {
                                return true;
                        }
                }
        }

        return false;
}

bool TDbg::BPInOutHit(BreakpointType type, int addr, int value, breakpoint* const bp)
{
        if (addr == -1)
        {
                return false;
        }

        switch (type)
        {
                case BP_INL:
                case BP_OUTL:
                        addr = addr & 0x00FF;
                        break;

                case BP_INH:
                case BP_OUTH:
                        addr = addr & 0xFF00;
                        break;

                default:
                        break;
        }

        if (bp->HitRdWrInOut(type, addr, value))
	{
                return true;
        }

	return false;        
}

bool TDbg::BPReadWriteHit(BreakpointType type, int addr, int value, breakpoint* const bp)
{
	if ((addr != -1) && bp->HitRdWrInOut(type, addr, value))
	{
		return true;
	}

        return false;
}

bool TDbg::BPTCyclesHit(int addr, breakpoint* const bp)
{
        if (bp->Type != BP_TCYCLES || !bp->Enabled)
        {
                return false;
        }

      	if (addr == bp->Addr)
	{
        	tStatesCount = 0;

		if (displayedTStatesCount != bp->TStates)
		{
                	return true;
	        }
	}

        return false;
}


bool TDbg::BPFlagValueHit(breakpoint* const bp)
{
        if (bp->Type != BP_FLAG || !bp->Enabled)
        {
                return false;
        }

        BYTE regF = z80.af.b.l;
        int mask = (1 << bp->FlagId);

        switch (bp->ConditionValue)
        {
                case Equal:
                        return ((regF & mask) == bp->Value);

                case NotEqual:
                        return ((regF & mask) != bp->Value);

                default:
                        break;
        }
        
        return false;
}

bool TDbg::BPMemoryValueHit(breakpoint* const bp)
{
        if (bp->Type != BP_MEMORY || !bp->Enabled)
        {
                return false;
        }

        BYTE value = getbyte(bp->Addr);

        switch (bp->ConditionValue)
        {
                case Equal:
                        return (value == bp->Value);

                case LessThanEquals:
                        return (value <= bp->Value);

                case GreaterThanEquals:
                        return (value >= bp->Value);

                case NotEqual:
                        return (value != bp->Value);

                default:
                        break;
        }
        
        return false;
}

bool TDbg::BPRegisterValueHit(breakpoint* const bp)
{
        if (bp->Type != BP_REGISTER || !bp->Enabled)
        {
                return false;
        }

        int value = getRegisterValue(bp->RegisterId);

        switch (bp->ConditionValue)
        {
                case Equal:
                        return (value == bp->Value);

                case LessThanEquals:
                        return (value <= bp->Value);

                case GreaterThanEquals:
                        return (value >= bp->Value);

                case NotEqual:
                        return (value != bp->Value);

                default:
                        break;
        }

        return false;
}

int TDbg::getRegisterValue(int registerIndex)
{
        int value;

        switch (registerIndex)
        {
		case RegA:
                        value = z80.af.b.h;
			break;

		case RegB:
                        value = z80.bc.b.h;
			break;

		case RegC:
                        value = z80.bc.b.l;
			break;

		case RegD:
                        value = z80.de.b.h;
			break;

		case RegE:
                        value = z80.de.b.l;
			break;

		case RegH:
                        value = z80.hl.b.h;
			break;

		case RegL:
                        value = z80.hl.b.l;
			break;

		case RegI:
                        value = z80.i;
			break;

		case RegR:
                        value = z80.r;
			break;

		case RegIXh:
                        value = z80.ix.b.h;
			break;

		case RegIXl:
                        value = z80.ix.b.l;
			break;

		case RegIYh:
                        value = z80.iy.b.h;
			break;

		case RegIYl:
                        value = z80.iy.b.l;
			break;

		case RegAltA:
                        value = z80.af_.b.h;
			break;

		case RegAltB:
                        value = z80.bc_.b.h;
			break;

		case RegAltC:
                        value = z80.bc_.b.l;
			break;

		case RegAltD:
                        value = z80.de_.b.h;
			break;

		case RegAltE:
                        value = z80.de_.b.l;
			break;

		case RegAltH:
                        value = z80.hl_.b.h;
			break;

		case RegAltL:
                        value = z80.hl_.b.l;
			break;

		case RegBC:
                        value = z80.bc.w;
			break;

		case RegDE:
                        value = z80.de.w;
			break;

		case RegHL:
                        value = z80.hl.w;
			break;

		case RegIX:
                        value = z80.ix.w;
			break;

		case RegIY:
                        value = z80.iy.w;
			break;

		case RegPC:
                        value = z80.pc.w;
			break;

		case RegSP:
                        value = z80.sp.w;
			break;
	
		case RegAltBC:
                        value = z80.bc_.w;
			break;

		case RegAltDE:
                        value = z80.de_.w;
			break;

		case RegAltHL:
                        value = z80.hl_.w;
			break;
        }

        return value;
}

void TDbg::DelTempBreakPoints(void)
{
        int i;
        for(i=0; i<Breakpoints; i++)
                if (!Breakpoint[i].Permanent)
                        DelBreakPoint(i);
}
//---------------------------------------------------------------------------
int TDbg::Hex2Dec(ZXString num)
{
        int val=0;
        int i;

        for(i=1; i<=num.Length(); i++)
        {
                if (num[i]>='0' && num[i]<='9') val = val*16 + num[i]-'0';
                else if (num[i]>='a' && num[i]<='f') val = val*16 + 10 + num[i]-'a';
                else if (num[i]>='A' && num[i]<='F') val = val*16 + 10 + num[i]-'A';
                else return(val);
        }
        return(val);
}
//---------------------------------------------------------------------------
ZXString TDbg::Hex16(int Value)
{
        return ZXString::IntToHex(Value,4);
}
//---------------------------------------------------------------------------
ZXString TDbg::Hex8(int Value)
{
        return ZXString::IntToHex(Value,2);
}
//---------------------------------------------------------------------------
ZXString TDbg::Bin8(int Value)
{
        char arry[9] = {0};
        for (int i = 7; i >= 0; --i)
        {
                arry[i] = (char)((Value & 1) + '0');
                Value >>= 1;
        }

        return ZXString(arry);
}
//---------------------------------------------------------------------------

// set the caption and other good stuff for a register label
//
void TDbg::SetLabelInfo(TLabel* label, int value, int valueWidth)
{
        // the tag is used for the editvalue function
        label->Tag = value;

        // the caption is fairly standard
        label->Caption = "$"+ZXString::IntToHex(label->Tag, valueWidth);
        if (valueWidth == 4)
        {
                // ahaa!
                ZXString hint("");

                // enable showHint if there is a symbol for the given 16bit value
                label->ShowHint = symbolstore::addressToSymbol(label->Tag, hint);
                label->Hint = hint;

                // lowlight the label if there's a hint to show
                //
                label->Color = label->ShowHint ? clInfoBk : clBtnFace;
        }
}

void TDbg::UpdateVals(void)
{
        int i, Stack;

        switch(emulator.machine)
        {
        case MACHINEACE:
                Stack=getbyte(0x3C3B)+256*getbyte(0x3C3C) -2;

                AceStk0->Caption = "$"+Hex16(Stack); AceStkVal0->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk1->Caption = "$"+Hex16(Stack); AceStkVal1->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk2->Caption = "$"+Hex16(Stack); AceStkVal2->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk3->Caption = "$"+Hex16(Stack); AceStkVal3->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk4->Caption = "$"+Hex16(Stack); AceStkVal4->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk5->Caption = "$"+Hex16(Stack); AceStkVal5->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk6->Caption = "$"+Hex16(Stack); AceStkVal6->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                Stack-=2;
                AceStk7->Caption = "$"+Hex16(Stack); AceStkVal7->Caption = "$"+Hex16(getbyte(Stack)+256*getbyte(Stack+1));
                break;

        case MACHINESPECTRUM:
                SpectraModeLabel->Enabled = (machine.colour == COLOURSPECTRA);
                SpectraMode->Enabled = (machine.colour == COLOURSPECTRA) && spectrum.spectraColourSwitchOn;
                SpectraMode->Caption = "$"+Hex8(spectrum.spectraMode);
                break;

        default:
                RowCount->Caption = lineCounter;
                Scanline->Caption = RasterY;
                ShiftReg->Caption = Bin8(shift_store);
                TStates->Caption = frametstates;
                NMIGen->Caption = nmiGeneratorEnabled ? "On":"Off";
                SyncOutput->Caption = syncOutputWhite ? "White":"Black";

                ChromaColourModeLabel->Enabled = (machine.colour == COLOURCHROMA);
                ChromaColourMode->Enabled = (machine.colour == COLOURCHROMA) && zx81.chromaColourSwitchOn;
                ChromaColourMode->Caption = "$"+Hex8(zx81.chromaMode);
                break;
        }

        switch (romcartridge.type)
        {
                case ROMCARTRIDGEZXC1:
                        GroupBoxZXC->Caption = "ZXC1 Cartridge";
                        break;
                case ROMCARTRIDGEZXC2:
                        GroupBoxZXC->Caption = "ZXC2 Cartridge";
                        break;
                case ROMCARTRIDGEZXC3:
                        GroupBoxZXC->Caption = "ZXC3 Cartridge";
                        break;
                case ROMCARTRIDGEZXC4:
                        GroupBoxZXC->Caption = "ZXC4 Cartridge";
                        break;
                default:
                        GroupBoxZXC->Caption = "ZXC Cartridge";
                        break;
        }

        bool zxcEnabled = (romcartridge.type == ROMCARTRIDGEZXC1 || romcartridge.type == ROMCARTRIDGEZXC2 || romcartridge.type == ROMCARTRIDGEZXC3 || romcartridge.type == ROMCARTRIDGEZXC4);
        ZXCModeLabel->Enabled = zxcEnabled;
        ZXCMode->Enabled = zxcEnabled;
        if (romcartridge.type == ROMCARTRIDGEZXC1)
        {
                if (romcartridge.zxc1PageOut)
                {
                        ZXCMode->Caption = "OUT";
                }
                else
                {
                        ZXCMode->Caption = "$"+Hex8(romcartridge.zxc1ActiveBank);
                }
        }
        else
        {
                ZXCMode->Caption = "$"+Hex8(romcartridge.zxcPaging);
        }
        SetLabelInfo(HL, z80.hl.w);
        SetLabelInfo(BC, z80.bc.w);
        SetLabelInfo(DE, z80.de.w);
        SetLabelInfo(HL_, z80.hl_.w);
        SetLabelInfo(BC_, z80.bc_.w);
        SetLabelInfo(DE_, z80.de_.w);
        SetLabelInfo(IX, z80.ix.w);
        SetLabelInfo(IY, z80.iy.w);
        SetLabelInfo(PC, z80.pc.w);
        SetLabelInfo(SP, z80.sp.w);
        SetLabelInfo(IR, (z80.i<<8) | (z80.r7 & 128) | ((z80.r) & 127));
        SetLabelInfo(A, z80.af.b.h, 2);
        SetLabelInfo(A_, z80.af_.b.h, 2);
        F->Caption = Bin8(z80.af.b.l);
        F_->Caption = Bin8(z80.af_.b.l);

        i=z80.sp.w;
        Stack0->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack1->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack2->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack3->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack4->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack5->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;
        Stack6->Caption = "$"+Hex16( getbyte(i)+256*getbyte(i+1) ); i+=2;

        i = recentHistory[(recentHistoryPos+0)&3];
        Disass0->Caption = Disassemble(&i);
        i = recentHistory[(recentHistoryPos+1)&3];
        Disass1->Caption = Disassemble(&i);
        i = recentHistory[(recentHistoryPos+2)&3];
        Disass2->Caption = Disassemble(&i);
        i=z80.pc.w;
        StepOverStartAddr=i;
        StepOverInstruction = IsStepOverInstruction(i);
        Disass3->Caption = (Disassemble(&i) + "                ").SetLength(50);
        Disass4->Caption = Disassemble(&i);
        Disass5->Caption = Disassemble(&i);
        Disass6->Caption = Disassemble(&i);
        Disass7->Caption = Disassemble(&i);
        Disass8->Caption = Disassemble(&i);
        Disass9->Caption = Disassemble(&i);
        Disass10->Caption = Disassemble(&i);
        Disass11->Caption = Disassemble(&i);

        Halt->Caption = z80.halted ? "Yes":"No" ;
        Interrupts->Caption = z80.iff1 ? "Enabled":"Disabled" ;
        IM->Caption = z80.im;

        if ((displayedTStatesCount >= 0) && (displayedTStatesCount < 1000000))
        {
                TStatesCount->Caption = displayedTStatesCount;
        }
        else
        {
                TStatesCount->Caption = "999999";
        }

        MemoryWindow->UpdateChanges();

        if (lastIOAccess[0].direction != IO_NONE)
        {
                if (lastIOAccess[0].direction == IO_IN) IOPort0Direction->Caption = "IN"; else IOPort0Direction->Caption = "OUT";
                IOPort0Address->Caption = "$"+ZXString::IntToHex(lastIOAccess[0].address, 4);
                IOPort0Data->Caption = "$"+Hex8(lastIOAccess[0].data);

                IOPort0Address->PopupMenu = (lastIOAccess[0].direction == IO_IN) ? InputContextPopup : OutputContextPopup;
        }
        else
        {
                IOPort0Direction->Caption = "-";
                IOPort0Address->Caption = "-";
                IOPort0Data->Caption = "-";

                IOPort0Address->PopupMenu = NULL;
        }

        if (lastIOAccess[1].direction != IO_NONE)
        {
                if (lastIOAccess[1].direction == IO_IN) IOPort1Direction->Caption = "IN"; else IOPort1Direction->Caption = "OUT";
                IOPort1Address->Caption = "$"+ZXString::IntToHex(lastIOAccess[1].address, 4);
                IOPort1Data->Caption = "$"+Hex8(lastIOAccess[1].data);

                IOPort1Address->PopupMenu = (lastIOAccess[1].direction == IO_IN) ? InputContextPopup : OutputContextPopup;
        }
        else
        {
                IOPort1Direction->Caption = "-";
                IOPort1Address->Caption = "-";
                IOPort1Data->Caption = "-";

                IOPort1Address->PopupMenu = NULL;
        }

        if (lastIOAccess[2].direction != IO_NONE)
        {
                if (lastIOAccess[2].direction == IO_IN) IOPort2Direction->Caption = "IN"; else IOPort2Direction->Caption = "OUT";
                IOPort2Address->Caption = "$"+ZXString::IntToHex(lastIOAccess[2].address, 4);
                IOPort2Data->Caption = "$"+Hex8(lastIOAccess[2].data);

                IOPort2Address->PopupMenu = (lastIOAccess[2].direction == IO_IN) ? InputContextPopup : OutputContextPopup;
        }
        else
        {
                IOPort2Direction->Caption = "-";
                IOPort2Address->Caption = "-";
                IOPort2Data->Caption = "-";

                IOPort2Address->PopupMenu = NULL;
        }

        if (lastIOAccess[3].direction != IO_NONE)
        {
                if (lastIOAccess[3].direction == IO_IN) IOPort3Direction->Caption = "IN"; else IOPort3Direction->Caption = "OUT";
                IOPort3Address->Caption = "$"+ZXString::IntToHex(lastIOAccess[3].address, 4);
                IOPort3Data->Caption = "$"+Hex8(lastIOAccess[3].data);

                IOPort3Address->PopupMenu = (lastIOAccess[3].direction == IO_IN) ? InputContextPopup : OutputContextPopup;
        }
        else
        {
                IOPort3Direction->Caption = "-";
                IOPort3Address->Caption = "-";
                IOPort3Data->Caption = "-";

                IOPort3Address->PopupMenu = NULL;
        }

        if (emulation_stop)
        {
                RunStop->Caption = "Run";
                SingleStep->Enabled = true;
                StepOver->Enabled = true;
                StepOut->Enabled = true;
                EnableVals();
                DelTempBreakPoints();

                Profiler->Refresh();
        }
        else
        {
                RunStop->Caption = "Stop";
                SingleStep->Enabled = false;
                StepOver->Enabled = false;
                StepOut->Enabled = false;
                if (!Continuous->Checked) DisableVals();
        }

}
//---------------------------------------------------------------------------

bool TDbg::IsStepOverInstruction(int Addr)
{
	bool stepOverInstruction = false;

        int Opcode;
        
        Opcode = GetMem(Addr);
        Addr++;

        if (Addr>=zx81.m1not && !(Opcode&64) && !(emulator.machine==MACHINEACE || emulator.machine==MACHINESPECTRUM))
        {
		Opcode=-1;
	}

        switch(Opcode)
        {
        case 0x10: // DJNZ fz
        case 0x76: // HALT
        case 0xc4: // CALL NZ,nnnn
        case 0xc7: // RST 00
        case 0xcc: // CALL Z,nnnn
        case 0xcd: // CALL nnnn
        case 0xcf: // RST 08
        case 0xd4: // CALL NC,nnnn
        case 0xd7: // RST 10
        case 0xdc: // CALL C,nnnn
        case 0xdf: // RST 18
        case 0xe4: // CALL PO,nnnn
        case 0xe7: // RST 20
        case 0xec: // CALL PE,nnnn
        case 0xef: // RST 28
        case 0xf4: // CALL P,nnnn
        case 0xf7: // RST 30
        case 0xfc: // CALL M,nnnn
        case 0xff: // RST 38
		stepOverInstruction = true;
		break;
        case 0xed:
                Opcode = GetMem(Addr);
                if (Addr>=zx81.m1not && !(Opcode&64) && !(emulator.machine==MACHINEACE || emulator.machine==MACHINESPECTRUM))
                {
			Opcode=-1;
		}
 
                switch(Opcode)
                {
                case 0xb0:	// LDIR
                case 0xb1:	// CPIR
                case 0xb2:	// INIR
                case 0xb3:	// OTIR
                case 0xb8:	// LDDR
                case 0xb9:	// CPDR
                case 0xba:	// INDR
                case 0xbb:	// OTDR
			stepOverInstruction = true;
                        break;
                }
                break;
        }
	
	return stepOverInstruction;
}
//---------------------------------------------------------------------------
void TDbg::EnableValues(bool enable)
{
        HL->Enabled = enable;
        BC->Enabled = enable;
        DE->Enabled = enable;
        HL_->Enabled = enable;
        BC_->Enabled = enable;
        DE_->Enabled = enable;
        IX->Enabled = enable;
        IY->Enabled = enable;
        PC->Enabled = enable;
        SP->Enabled = enable;
        IR->Enabled = enable;
        A->Enabled = enable;
        A_->Enabled = enable;
        F->Enabled = enable;
        F_->Enabled = enable;

        Stack0->Enabled = enable;
        Stack1->Enabled = enable;
        Stack2->Enabled = enable;
        Stack3->Enabled = enable;
        Stack4->Enabled = enable;
        Stack5->Enabled = enable;
        Stack6->Enabled = enable;

        Disass0->Enabled = enable;
        Disass1->Enabled = enable;
        Disass2->Enabled = enable;
        Disass3->Enabled = enable;
        Disass4->Enabled = enable;
        Disass5->Enabled = enable;
        Disass6->Enabled = enable;
        Disass7->Enabled = enable;
        Disass8->Enabled = enable;
        Disass9->Enabled = enable;
        Disass10->Enabled = enable;
        Disass11->Enabled = enable;

        Halt->Enabled = enable;
        Interrupts->Enabled = enable;
        IM->Enabled = enable;

        RowCount->Enabled = enable;
        Scanline->Enabled = enable;
        ShiftReg->Enabled = enable;
        TStates->Enabled = enable;

        NMIGen->Enabled = enable;
        SyncOutput->Enabled = enable;

        AceStk0->Enabled=enable; AceStkVal0->Enabled=enable;
        AceStk1->Enabled=enable; AceStkVal1->Enabled=enable;
        AceStk2->Enabled=enable; AceStkVal2->Enabled=enable;
        AceStk3->Enabled=enable; AceStkVal3->Enabled=enable;
        AceStk4->Enabled=enable; AceStkVal4->Enabled=enable;
        AceStk5->Enabled=enable; AceStkVal5->Enabled=enable;
        AceStk6->Enabled=enable; AceStkVal6->Enabled=enable;
        AceStk7->Enabled=enable; AceStkVal7->Enabled=enable;

        IOPort0Direction->Enabled = enable;
        IOPort0Address->Enabled = enable;
        IOPort0Data->Enabled = enable;
        IOPort1Direction->Enabled = enable;
        IOPort1Address->Enabled = enable;
        IOPort1Data->Enabled = enable;
        IOPort2Direction->Enabled = enable;
        IOPort2Address->Enabled = enable;
        IOPort2Data->Enabled = enable;
        IOPort3Direction->Enabled = enable;
        IOPort3Address->Enabled = enable;
        IOPort3Data->Enabled = enable;

        TStatesCount->Enabled = enable;

        SpectraMode->Enabled = enable;
        ChromaColourMode->Enabled = enable;
        ZXCMode->Enabled = enable;
}

void TDbg::DisableVals(void)
{
        EnableValues(false);
}

void TDbg::EnableVals(void)
{
        EnableValues(true);
}
//---------------------------------------------------------------------------
__fastcall TDbg::TDbg(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile *ini;

        DoNext=false;
        Breakpoints=0;
        NMIRetAddr=INTRetAddr=-1;

        ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;

        ClientHeight=GroupBox2->Top+GroupBox2->Height+4;
        ClientWidth=GroupBox4->Left+GroupBox4->Width+4;

        BPList->DefaultColWidth = BPList->Width;

        ResetLastIOAccesses();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::RunStopClick(TObject *Sender)
{
        StepOutRequested = 0;

        emulation_stop = !emulation_stop;
        if(!emulation_stop)
        {
                MemoryWindow->ClearChanges();
        }
        UpdateVals();
        StepOutRequested = 0;
        StackChange = 0;
        ClearSkipAddresses();

        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FormClose(TObject *Sender, TCloseAction &Action)
{
        emulator.single_step=0;
        Form1->DebugWin->Checked=false;
        emulation_stop=0;
        StepOutRequested = 0;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FormShow(TObject *Sender)
{
        Continuous->Enabled = true;

        if (Continuous->Checked==true) emulator.single_step=1;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::ContinuousClick(TObject *Sender)
{
        if (Continuous->Enabled)
        {
                if (Continuous->Checked==true)
                {
                        emulator.single_step=1;
                        EnableVals();
                }
                else
                {
                        emulator.single_step=0;
                        DisableVals();
                }

                UpdateVals();
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SingleStepClick(TObject *Sender)
{
        StepOutRequested = 0;
        MemoryWindow->ClearChanges();
        emulation_stop=0;
        emulator.single_step=1;
        StackChange = 0;
        DoNext=true;
}
//---------------------------------------------------------------------------
void __fastcall TDbg::StepOverClick(TObject *Sender)
{
        StepOutRequested = 0;

        if (!StepOverInstruction)
        {
                SingleStepClick(Sender);
                return;
        }

        int i = z80.pc.w;
        Disassemble(&i);
        StepOverAddr = i;
        StepOverStack = z80.sp.w;
        StepOverInstructionSize = StepOverAddr - StepOverStartAddr;

        MemoryWindow->ClearChanges();
        breakpoint bp(StepOverAddr, BP_EXE);
        bp.Permanent = false;
        AddBreakPoint(bp);
        RunStopClick(NULL);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AddBrkBtnClick(TObject *Sender)
{
        SetBreakpoint->CentreOn(this);

        breakpoint bp(0, BP_EXE);
        if (SetBreakpoint->EditBreakpoint(bp))
        {
                AddBreakPoint(bp);
        }

        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DelBrkBtnClick(TObject *Sender)
{
        if (BPList->Row < (BPList->RowCount-1))
                DelBreakPoint(BPList->Row);

        if ((BPList->RowCount > 1) && ((BPList->Row + 1) >= BPList->RowCount))
        {
                BPList->Row -= 1;
        }

        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------
                                                
void __fastcall TDbg::SkipNMIBtnClick(TObject *Sender)
{
        if (!SkipNMIBtn->Checked)
                NMIRetAddr=-1;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SkipINTBtnClick(TObject *Sender)
{
        if (!SkipINTBtn->Checked)
                INTRetAddr=-1;
}
//---------------------------------------------------------------------------

void TDbg::LoadSettings(TIniFile *ini)
{
        Top = ini->ReadInteger("DEBUG","Top",Top);
        Left = ini->ReadInteger("DEBUG","Left",Left);

        if (Form1->DebugWin->Checked)
        {
                Form1->DebugWin->Checked=false;
        }

        Continuous->Checked = ini->ReadBool("DEBUG", "Continuous", false);
        SkipNMIBtn->Checked = ini->ReadBool("DEBUG", "SkipNMI", false);
        SkipINTBtn->Checked = ini->ReadBool("DEBUG", "SkipINT", false);
        EnableHistory->Checked = ini->ReadBool("DEBUG", "EnableHistory", false);
        AutoUpdateMemory->Checked = ini->ReadBool("DEBUG", "AutoUpdateMemory", false);
}

//---------------------------------------------------------------------------
void TDbg::SaveSettings(TIniFile *ini)
{
        ini->WriteInteger("DEBUG","Top",Top);
        ini->WriteInteger("DEBUG","Left",Left);
        ini->WriteBool("DEBUG", "Continuous", Continuous->Checked);
        ini->WriteBool("DEBUG", "SkipNMI", SkipNMIBtn->Checked);
        ini->WriteBool("DEBUG", "SkipINT", SkipINTBtn->Checked);
        ini->WriteBool("DEBUG", "EnableHistory", EnableHistory->Checked);
        ini->WriteBool("DEBUG", "AutoUpdateMemory", AutoUpdateMemory->Checked);
}

//---------------------------------------------------------------------------
void TDbg::ClearHistoryWindow()
{
        HistoryPos = 0;
        historyWrappedAround = false;

        PopulateHistoryWindow();
}

//---------------------------------------------------------------------------
void TDbg::ReloadHistoryWindow()
{
        PopulateHistoryWindow();
}

//---------------------------------------------------------------------------
void __fastcall TDbg::HistoryClick(TObject *Sender)
{
        if (HistoryBox->Visible)
        {
                HistoryBox->Close();
                return;
        }

        PopulateHistoryWindow();
        HistoryBox->Show();
}

//---------------------------------------------------------------------------
void TDbg::PopulateHistoryWindow()
{
        HistoryBox->Text->Enabled = false;
        HistoryBox->Text->Clear();

        bool reverseOrder = HistoryBox->ReverseOrder();
        bool show0K8KAddresses = HistoryBox->Show0K8KAddresses();
        bool show8K16KAddresses = HistoryBox->Show8K16KAddresses();
        bool show16K32KAddresses = HistoryBox->Show16K32KAddresses();
        bool show32K48KAddresses = HistoryBox->Show32K48KAddresses();
        bool show48K64KAddresses = HistoryBox->Show48K64KAddresses();

        int i=HistoryPos-1;
        if (i == -1) i=historySize-1;
        while(i != HistoryPos)
        {
                if (historyWrappedAround || (i < HistoryPos))
		{
                        int address = HistoryLog[i].Address;
                        
                        bool includeInstruction = (show0K8KAddresses   && (address <= 0x1FFF)) ||
                                                  (show8K16KAddresses  && (address >= 0x2000) && (address <= 0x3FFF)) ||
                                                  (show16K32KAddresses && (address >= 0x4000) && (address <= 0x7FFF)) ||
                                                  (show32K48KAddresses && (address >= 0x8000) && (address <= 0xBFFF)) ||
                                                  (show48K64KAddresses && (address >= 0xC000) && (address <= 0xFFFF));

                        if (includeInstruction)
                        {
		 	        ZXString instruction = Dbg->Disassemble(address, HistoryLog[i].Bytes);
                                if (reverseOrder)
                                {
        		         	HistoryBox->Text->Lines->Insert(0, instruction);
                                }
                                else
                                {
        		         	HistoryBox->Text->Lines->Add(instruction);
                                }
                        }
		}
					
                --i;
                if (i==-1) i=historySize-1;
        }

        HistoryBox->Text->Enabled = true;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::NMIGenClick(TObject *Sender)
{
        nmiGeneratorEnabled = !nmiGeneratorEnabled;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SyncOutputClick(TObject *Sender)
{
        syncOutputWhite = !syncOutputWhite;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DoEditReg(WORD& value)
{
        EditValue->CentreOn(this);

        int n = value;
        if (EditValue->Edit2(n, 2))
        {
                value = (WORD)n;
                UpdateVals();
        }
}

//---------------------------------------------------------------------------

void __fastcall TDbg::DoEditReg(BYTE& value)
{
        EditValue->CentreOn(this);

        int n = value;
        if (EditValue->Edit2(n, 1))
        {
                value = (BYTE)n;
                UpdateVals();
        }
}

//---------------------------------------------------------------------------

void __fastcall TDbg::HLClick(TObject *Sender)
{
        DoEditReg(z80.hl.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BCClick(TObject *Sender)
{
        DoEditReg(z80.bc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DEClick(TObject *Sender)
{
        DoEditReg(z80.de.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HL_Click(TObject *Sender)
{
        DoEditReg(z80.hl_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DE_Click(TObject *Sender)
{
        DoEditReg(z80.de_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BC_Click(TObject *Sender)
{
        DoEditReg(z80.bc_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IXClick(TObject *Sender)
{
        DoEditReg(z80.ix.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IYClick(TObject *Sender)
{
        DoEditReg(z80.iy.w);
}
//---------------------------------------------------------------------------


void __fastcall TDbg::SPClick(TObject *Sender)
{
        DoEditReg(z80.sp.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::PCClick(TObject *Sender)
{
        DoEditReg(z80.pc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AClick(TObject *Sender)
{
        DoEditReg(z80.af.b.h);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::A_Click(TObject *Sender)
{
        DoEditReg(z80.af_.b.h);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::InterruptsClick(TObject *Sender)
{
        // DI - IFF1=IFF2=0
        // EI - IFF1=IFF2=1;

        z80.iff1 = z80.iff2 = !z80.iff1;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IMClick(TObject *Sender)
{
        if (++z80.im == 3) z80.im=0;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DoEditStack(int offs)
{
        EditValue->CentreOn(this);

        int i = 2 * offs + z80.sp.w;
        int v = getbyte(i)+256*getbyte(i+1);
        if (EditValue->Edit2(v, 2))
        {
                setbyte(i, v&255);
                setbyte(i+1, v>>8);
                UpdateVals();
        }
}

void __fastcall TDbg::Stack0Click(TObject *Sender)
{
        TLabel* label = (TLabel*)Sender;
        int len = label->Name.Length();
        int idx = label->Name.SubString(len, 1).ToInt();
        DoEditStack(idx);
}
//---------------------------------------------------------------------------

// dereferences the stack values. requires the stack
// window value labels to be labelled with a number according to the
// index on the stack. (SP-0) = 0, (SP-2) = 1, etc.
//
void __fastcall TDbg::Stack0MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        TLabel* label = (TLabel*)Sender;
        int len = label->Name.Length();
        int idx = label->Name.SubString(len, 1).ToInt();

        int i = z80.sp.w;
        i += 2 * idx;
        SetMenuContent(getbyte(i)+256*getbyte(i+1));
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IRClick(TObject *Sender)
{
        EditValue->CentreOn(this);

        int v;
        v = (z80.i << 8) | (z80.r7 & 128) | z80.r;
        if (EditValue->Edit2(v, 2))
        {
                z80.r = (WORD)(v&127);
                z80.r7 = (BYTE)(v&128);
                z80.i = (BYTE)((v>>8)&255);
                UpdateVals();
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FClick(TObject *Sender)
{
        // this relies on the textbox that displays the flag bits
        // being exactly 8 char_width pixels wide.
        //
        TPoint cp = ScreenToClient(Mouse->CursorPos);
        cp.x -= F->Left + 2;
        cp.y -= F->Top;
        int f = cp.x / (F->Width / 8);

        int bit = 0x80 >> f;
        z80.af.b.l ^= (BYTE)bit;
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::F_Click(TObject *Sender)
{
        // see above.
        //
        TPoint cp = ScreenToClient(Mouse->CursorPos);
        cp.x -= F_->Left + 2;
        cp.y -= F_->Top;
        int f = (cp.x) / (F_->Width / 8);

        int bit = 0x80 >> f;
        z80.af_.b.l ^= (BYTE)bit;
        UpdateVals();
}
//---------------------------------------------------------------------------

void TDbg::SetMenuContent(int memloc)
{
        MemDumpFromHere1->Tag = memloc;
        MemDumpPopup->Tag = memloc;
}
//---------------------------------------------------------------------------


void __fastcall TDbg::BCMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.bc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DEMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.de.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HLMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.hl.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BC_MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.bc_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DE_MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.de_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::HL_MouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.hl_.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IXMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.ix.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IYMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.iy.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IRMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        int ir = (z80.i << 8) | (z80.r7 & 128) | (z80.r & 127);
        SetMenuContent(ir);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SPMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
        SetMenuContent(z80.sp.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::PCMouseDown(TObject *Sender, TMouseButton Button,
      TShiftState Shift, int X, int Y)
{
   SetMenuContent(z80.pc.w);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::MemDumpFromHere1Click(TObject *Sender)
{
        MemoryWindow->Show();
        MemoryWindow->BaseAddress = MemDumpFromHere1->Tag;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AddBreak1Click(TObject *Sender)
{
        TMenuItem* mi = (TMenuItem*)Sender;
        breakpoint bp(MemDumpPopup->Tag, (BreakpointType)mi->Tag);
        AddBreakPoint(bp);

        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AceStk0Click(TObject *Sender)
{
//        fred;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AceStkVal0Click(TObject *Sender)
{
//        bert
}
//---------------------------------------------------------------------------

void __fastcall TDbg::SymbolsClick(TObject *Sender)
{
        if (SymbolBrowser->Visible)
        {
                SymbolBrowser->Close();
                return;
        }

        SymbolBrowser->Show();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::MemoryClick(TObject *Sender)
{
        if (MemoryWindow->Visible)
        {
                MemoryWindow->Close();
                return;
        }

        MemoryWindow->BaseAddress = z80.pc.w;
        MemoryWindow->Show();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::EditBrkBtnClick(TObject *Sender)
{
        EditBreakpoint();
        UpdateBreakpointButtons();
}

void TDbg::EditBreakpoint()
{
       SetBreakpoint->CentreOn(this);

        int idx = BPList->Row;
        breakpoint bp = Breakpoint[idx];

        const bool editingState = true;
        if (SetBreakpoint->EditBreakpoint(bp) && FindBreakPointEntry(idx, bp, editingState) == -1)
        {
                Breakpoint[idx] = bp;

                ZXString str = GetBreakpointText(&bp);
                BPList->Rows[idx]->Text = str;
        }
}
//---------------------------------------------------------------------------

void ResetLastIOAccesses()
{
        for (int i = 0; i < 4; i++)
        {
                Dbg->lastIOAccess[i].direction = IO_NONE;
        }
}
//---------------------------------------------------------------------------

void LogInAccess(int address, BYTE data)
{
        Dbg->lastPortInAddr = address;
        Dbg->lastPortInValue = data;

        for (int i = 2; i >= 0; i--)
        {
                  Dbg->lastIOAccess[i+1] = Dbg->lastIOAccess[i];
        }

        Dbg->lastIOAccess[0].direction = IO_IN;
        Dbg->lastIOAccess[0].address = address;
        Dbg->lastIOAccess[0].data = data;
}
//---------------------------------------------------------------------------

void LogOutAccess(int address, BYTE data)
{
        Dbg->lastPortOutAddr = address;
        Dbg->lastPortOutValue = data;

        for (int i = 2; i >= 0; i--)
        {
                  Dbg->lastIOAccess[i+1] = Dbg->lastIOAccess[i];
        }

        Dbg->lastIOAccess[0].direction = IO_OUT;
        Dbg->lastIOAccess[0].address = address;
        Dbg->lastIOAccess[0].data = data;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BPListSelectCell(TObject *Sender, int ACol, int ARow,
      bool &CanSelect)
{
        if ((ARow + 1) >= BPList->RowCount)
        {
                CanSelect = false;
        }
        else
        {
                EditBrkBtn->Enabled = (BPList->RowCount > 1) && BPList->Row >= 0 && Breakpoint[ARow].Permanent;
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::Disass3MouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        ZXString t = ((TLabel*)Sender)->Caption;
        ZXString t2 = t.SubString(13,5);
        SetMenuContent(StrToInt(t2));
}
//---------------------------------------------------------------------------

void __fastcall TDbg::ButtonProfilerClick(TObject *Sender)
{
        Profiler->Show();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::StepOutClick(TObject *Sender)
{
        RunStopClick(NULL);

        StepOutRequested = 1;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::FormKeyDown(TObject *Sender, WORD &Key,
      TShiftState Shift)
{
        if (Shift.Contains(ssShift) && (Key == VK_F8))
        {
                RunStopClick(Sender);
        }
        else if (emulation_stop && Shift.Contains(ssShift))
        {
                switch(Key)
                {
                case VK_F5:
                        SingleStepClick(Sender);
                        break;

                case VK_F6:
                        StepOverClick(Sender);
                        break;

                case VK_F7:
                        StepOutClick(Sender);
                        break;
                }
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IOPort0AddressMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        SetIoContextPopupContent(IOPort0Address->Caption);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IOPort1AddressMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        SetIoContextPopupContent(IOPort1Address->Caption);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IOPort2AddressMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        SetIoContextPopupContent(IOPort2Address->Caption);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::IOPort3AddressMouseDown(TObject *Sender,
      TMouseButton Button, TShiftState Shift, int X, int Y)
{
        SetIoContextPopupContent(IOPort3Address->Caption);
}
//---------------------------------------------------------------------------
void TDbg::SetIoContextPopupContent(ZXString ioPort)
{
        if (ioPort != "-")
        {
                ZXString val = "0x" + ioPort.SubString(2, 4);
                int portNumber = StrToInt(val);
                InputContextPopup->Tag = portNumber;
                OutputContextPopup->Tag = portNumber;
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BreakOnInputOutputClick(TObject *Sender)
{
        TMenuItem* mi = (TMenuItem*)Sender;
        breakpoint bp(InputContextPopup->Tag, (BreakpointType)mi->Tag);
        AddBreakPoint(bp);

        UpdateBreakpointButtons();
}

//---------------------------------------------------------------------------
void __fastcall TDbg::BreakOnInputOutputHighClick(TObject *Sender)
{
        InputContextPopup->Tag = InputContextPopup->Tag & 0xFF00;
        OutputContextPopup->Tag = OutputContextPopup->Tag & 0xFF00;

        TMenuItem* mi = (TMenuItem*)Sender;
        breakpoint bp(InputContextPopup->Tag, (BreakpointType)mi->Tag);
        AddBreakPoint(bp);

        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BreakOnInputOutputLowClick(TObject *Sender)
{
        InputContextPopup->Tag = InputContextPopup->Tag & 0x00FF;
        OutputContextPopup->Tag = OutputContextPopup->Tag & 0x00FF;

        TMenuItem* mi = (TMenuItem*)Sender;
        breakpoint bp(InputContextPopup->Tag, (BreakpointType)mi->Tag);
        AddBreakPoint(bp);

        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------
void TDbg::UpdateBreakpointButtons()
{
        AddBrkBtn->Enabled = (Breakpoints < maxBreakpoints);
        DelBrkBtn->Enabled = (BPList->RowCount > 1);
        EditBrkBtn->Enabled = (BPList->RowCount > 1) && BPList->Row >= 0 && Breakpoint[BPList->Row].Permanent;

        bool breakpointWithHitCount = false;

        for (int b = 0; b < Breakpoints; b++)
        {
                if (Breakpoint[b].HitCount > 1 && Breakpoint[b].Hits > 0)
                {
                        breakpointWithHitCount = true;
                        break;
                }
        }

        ResetBrkBtn->Enabled = breakpointWithHitCount && (BPList->RowCount > 1);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BPListDrawCell(TObject *Sender, int ACol, int ARow,
      TRect &Rect, TGridDrawState State)
{
        if (Breakpoint[ARow].Enabled)
        {
                if (State.Contains(gdSelected))
                {
                        BPList->Canvas->Font->Color = clHighlightText;
                }
                else
                {
                        BPList->Canvas->Font->Color = clWindowText;
                }
        }
        else
        {
                BPList->Canvas->Font->Color = clGrayText;
        }

        BPList->Canvas->TextRect(Rect, Rect.Left + 6, Rect.Top, BPList->Cells[ACol][ARow]);
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BPListDblClick(TObject *Sender)
{
        if (Breakpoints == 0 || BPList->Row == -1)
        {
                return;
        }

        int col, row;

        TPoint p = BPList->ScreenToClient(Mouse->CursorPos);
        BPList->MouseToCell(p.x, p.y, col, row);

        if (row == -1 || row >= Breakpoints)
        {
                return;
        }

        EditBreakpoint();
        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::DisableClick(TObject *Sender)
{
        Breakpoint[BPList->Row].Enabled = false;
        BPList->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::EnableClick(TObject *Sender)
{
        Breakpoint[BPList->Row].Enabled = true;
        BPList->Invalidate();
}
//---------------------------------------------------------------------------

void __fastcall TDbg::BPListContextPopup(TObject *Sender, TPoint &MousePos,
      bool &Handled)
{
        if (Breakpoints == 0)
        {
                Handled = true;
                return;
        }

        TPoint p = BPList->ScreenToClient(Mouse->CursorPos);

        int col, row;
        BPList->MouseToCell(p.x, p.y, col, row);

        if (row == -1 || row >= Breakpoints)
        {
                Handled = true;
                return;
        }

        TGridRect rect;
        rect.Top = row;
        rect.Bottom = row;
        rect.Left = 0;
        rect.Right = 0;
        BPList->Selection = rect;

        const int Disable = 0;
        const int Enable = 1;
        const int Reset = 2;
        const int Separator = 3;
        const int Count = 4;

        if (Breakpoint[BPList->Row].Enabled)
        {
                BreakpointWindowPopup->Items->Items[Disable]->Visible = true;
                BreakpointWindowPopup->Items->Items[Enable]->Visible = false;
        }
        else
        {
                BreakpointWindowPopup->Items->Items[Disable]->Visible = false;
                BreakpointWindowPopup->Items->Items[Enable]->Visible = true;
        }

        if (Breakpoint[BPList->Row].HitCount > 1)
        {
                BreakpointWindowPopup->Items->Items[Reset]->Visible = true;
                BreakpointWindowPopup->Items->Items[Separator]->Visible = true;
                BreakpointWindowPopup->Items->Items[Count]->Visible = true;

                ZXString hitCount = "Hit Count = ";
                hitCount += Breakpoint[BPList->Row].Hits;
                BreakpointWindowPopup->Items->Items[Count]->Caption = hitCount;

                BreakpointWindowPopup->Items->Items[Reset]->Enabled = (Breakpoint[BPList->Row].Hits > 0);
        }
        else
        {
                BreakpointWindowPopup->Items->Items[Reset]->Visible = false;
                BreakpointWindowPopup->Items->Items[Separator]->Visible = false;
                BreakpointWindowPopup->Items->Items[Count]->Visible = false;
        }
}
//---------------------------------------------------------------------------


void __fastcall TDbg::ResetBrkBtnClick(TObject *Sender)
{
        ResetBreakpointHits();
        UpdateBreakpointButtons();
}
//---------------------------------------------------------------------------
void TDbg::ResetBreakpointHits()
{
        for (int b = 0; b < Breakpoints; b++)
        {
                Breakpoint[b].Hits = 0;
        }
}
//---------------------------------------------------------------------------
void TDbg::ClearSkipAddresses()
{
        INTRetAddr = -1;
        NMIRetAddr = -1;
}
//---------------------------------------------------------------------------
void TDbg::Reset()
{
        ResetBreakpointHits();
        RefreshBreakpointList();
        ClearSkipAddresses();
}
//---------------------------------------------------------------------------
void TDbg::RefreshBreakpointList()
{
        for (int b = 0; b < Breakpoints; b++)
        {
                ZXString str = GetBreakpointText(&Breakpoint[b]);
                BPList->Rows[b]->Text = str;
        }
}
//---------------------------------------------------------------------------

void __fastcall TDbg::ResetHitCountClick(TObject *Sender)
{
        Breakpoint[BPList->Row].Hits = 0;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::AutoUpdateMemoryClick(TObject *Sender)
{
        MemoryWindowTimer->Enabled = AutoUpdateMemory->Checked;
}
//---------------------------------------------------------------------------

void __fastcall TDbg::MemoryWindowTimerExpired(TObject *Sender)
{
        if (MemoryWindow->Visible)
        {
                MemoryWindow->UpdateChanges();
        }
}
//---------------------------------------------------------------------------

void TDbg::DisableMemoryWindowAutoUpdates()
{
        if (MemoryWindow->Visible)
        {
                MemoryWindowTimer->Enabled = false;
        }
}


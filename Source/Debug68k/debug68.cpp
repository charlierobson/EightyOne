//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "debug68.h"
#include "main_.h"
#include "68000.h"
#include "zx81config.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TDebug68k *Debug68k;

extern "C" int m68k_disassemble(char* str_buff, int pc);

extern int emulation_stop;

void DebugUpdate68k(void)
{
        if (emulation_stop) Debug68k->UpdateVals();
}

//---------------------------------------------------------------------------
__fastcall TDebug68k::TDebug68k(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TDebug68k::FormClose(TObject *Sender, TCloseAction &Action)
{
        Form1->DebugWin->Checked=false;
}
//---------------------------------------------------------------------------

ZXString TDebug68k::Hex32(int val)
{
        return("$"+IntToHex(val, 8));
}

void TDebug68k::UpdateVals(void)
{
        //void *p1, *p2;
        int p1;
        char buf[256];

        D0->Caption=Hex32(reg[0]);
        D1->Caption=Hex32(reg[1]);
        D2->Caption=Hex32(reg[2]);
        D3->Caption=Hex32(reg[3]);
        D4->Caption=Hex32(reg[4]);
        D5->Caption=Hex32(reg[5]);
        D6->Caption=Hex32(reg[6]);
        D7->Caption=Hex32(reg[7]);

        A0->Caption=Hex32(reg[8]);
        A1->Caption=Hex32(reg[9]);
        A2->Caption=Hex32(reg[10]);
        A3->Caption=Hex32(reg[11]);
        A4->Caption=Hex32(reg[12]);
        A5->Caption=Hex32(reg[13]);
        A6->Caption=Hex32(reg[14]);

        USP->Caption=Hex32(usp);
        SSP->Caption=Hex32(ssp);

        p1=pc&0xfffff;
        
        Dis1->Caption=Hex32(p1)+" "; p1+=m68k_disassemble(buf,p1); Dis1->Caption = Dis1->Caption + buf;
        Dis2->Caption=Hex32(p1)+" "; p1+=m68k_disassemble(buf,p1); Dis2->Caption = Dis2->Caption + buf;
        Dis3->Caption=Hex32(p1)+" "; p1+=m68k_disassemble(buf,p1); Dis3->Caption = Dis3->Caption + buf;
        Dis4->Caption=Hex32(p1)+" "; p1+=m68k_disassemble(buf,p1); Dis4->Caption = Dis4->Caption + buf;
        Dis5->Caption=Hex32(p1)+" "; p1+=m68k_disassemble(buf,p1); Dis5->Caption = Dis5->Caption + buf;
        Dis6->Caption=Hex32(p1)+" "; p1+=m68k_disassemble(buf,p1); Dis6->Caption = Dis6->Caption + buf;
        Dis7->Caption=Hex32(p1)+" "; p1+=m68k_disassemble(buf,p1); Dis7->Caption = Dis7->Caption + buf;
        Dis8->Caption=Hex32(p1)+" ";     m68k_disassemble(buf,p1); Dis8->Caption = Dis8->Caption + buf;
}

void __fastcall TDebug68k::StopClick(TObject *Sender)
{
        emulation_stop = !emulation_stop;
        emulator.single_step=0;
        if (emulation_stop) Stop->Caption="Run";
        else Stop->Caption="Stop";
        UpdateVals();
}
//---------------------------------------------------------------------------

void __fastcall TDebug68k::SingleStepClick(TObject *Sender)
{
        emulation_stop=0;
        emulator.single_step=1;
}
//---------------------------------------------------------------------------


void __fastcall TDebug68k::FormShow(TObject *Sender)
{
        UpdateVals();        
}
//---------------------------------------------------------------------------


//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "ProfSampEdit.h"
#include "z80.h"
#include "SymbolStore.h"
#include "AddressResolver.h"
#include "ProfileDetail.h"
#include "SymBrowse.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TProfileSampleEdit *ProfileSampleEdit;
__fastcall TProfileSampleEdit::TProfileSampleEdit(TComponent* Owner) :
        TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TProfileSampleEdit::ButtonOKClick(TObject *Sender)
{
        int start, end;
        if (!AddressResolver::Validate(EditStart->Text, start) ||
                !AddressResolver::Validate(EditEnd->Text, end)) return;

        _pd->_start = start;
        _pd->_end = end;
        _pd->_tag = EditTag->Text;

        _valid = true;

        EditTag->SetFocus();
        
        Close();
}
//---------------------------------------------------------------------------

void __fastcall TProfileSampleEdit::EditValues(ProfileDetail* pd,
                void (*completion)(bool))
{
        _pd = pd;
        _completion = completion;

        EditTag->Text = pd->_tag;
        EditStart->Text = symbolstore::addressToSymbolOrHex(pd->_start);
        EditEnd->Text = symbolstore::addressToSymbolOrHex(pd->_end);

        // hope for the best...
        _valid = false;

        Show();
}
//---------------------------------------------------------------------------

void __fastcall TProfileSampleEdit::EditDblClick(TObject *Sender)
{
        SymbolBrowser->Show();
}
//---------------------------------------------------------------------------

void __fastcall TProfileSampleEdit::FormClose(TObject *Sender,
      TCloseAction &Action)
{
        _completion(_valid);
}
//---------------------------------------------------------------------------

void __fastcall TProfileSampleEdit::EditStartChange(TObject *Sender)
{
        int start;
        bool valid = AddressResolver::Validate(EditStart->Text, start);
        EditStart->Font->Color = valid ? clWindowText : clRed;

        SetOKButtonStatus();
}
//---------------------------------------------------------------------------

void __fastcall TProfileSampleEdit::EditEndChange(TObject *Sender)
{
        int end;
        bool valid = AddressResolver::Validate(EditEnd->Text, end);
        EditEnd->Font->Color = valid ? clWindowText : clRed;

        SetOKButtonStatus();
}
//---------------------------------------------------------------------------

void TProfileSampleEdit::SetOKButtonStatus()
{
        int start, end;
        bool validAddresses = (AddressResolver::Validate(EditStart->Text, start) &&
                               AddressResolver::Validate(EditEnd->Text, end));

        bool enable = validAddresses && (EditStart->Text != "") && (EditEnd->Text != "");
        ButtonOK->Enabled = enable;
}

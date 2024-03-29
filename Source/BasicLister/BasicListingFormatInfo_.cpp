//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "BasicListingFormatInfo_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TBasicListingFormatInfoForm *BasicListingFormatInfoForm;
//---------------------------------------------------------------------------
__fastcall TBasicListingFormatInfoForm::TBasicListingFormatInfoForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void TBasicListingFormatInfoForm::SetActivePage(int index)
{
}
//---------------------------------------------------------------------------

void __fastcall TBasicListingFormatInfoForm::Weblink1Click(TObject *Sender)
{
        OpenWebsite();
}
//---------------------------------------------------------------------------

void __fastcall TBasicListingFormatInfoForm::Weblink2Click(TObject *Sender)
{
        OpenWebsite();
}
//---------------------------------------------------------------------------

void __fastcall TBasicListingFormatInfoForm::Weblink3Click(TObject *Sender)
{
        OpenWebsite();
}
//---------------------------------------------------------------------------
void TBasicListingFormatInfoForm::OpenWebsite()
{
		ShellExecute(NULL, _TEXT("open"), _TEXT("http://freestuff.grok.co.uk/zxtext2p/index.html"), NULL, NULL, SW_SHOW );
}

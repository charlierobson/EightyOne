//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop

#include "zx81config.h"
#include "BasicLoaderOptions_.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TLoadBasicListingOptionsForm *LoadBasicListingOptionsForm;
//---------------------------------------------------------------------------
__fastcall TLoadBasicListingOptionsForm::TLoadBasicListingOptionsForm(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile* ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------
bool TLoadBasicListingOptionsForm::GetTokeniseRemContents()
{
        return cboTokeniseRemContents->Checked;
}

bool TLoadBasicListingOptionsForm::GetTokeniseStringContents()
{
        return cboTokeniseStringContents->Checked;
}

bool TLoadBasicListingOptionsForm::GetDiscardSurplusSpaces()
{
        return cboDiscardSurplusSpaces->Checked;
}

bool TLoadBasicListingOptionsForm::GetAcceptAlternateKeywordSpelling()
{
        return cboAcceptAlternateKeywordSpelling->Checked;
}

int TLoadBasicListingOptionsForm::GetAutomaticLineNumberIncrement()
{
        return StrToInt(cboLineNumberIncrement->Text);
}

bool TLoadBasicListingOptionsForm::GetSupportZxTokenFormatExtensions()
{
        return cboAcceptZxTokenFormatExtensions->Checked;
}

void TLoadBasicListingOptionsForm::SaveSettings(TIniFile *ini)
{
        ini->WriteBool("BASICLISTING", "TokeniseRemContents", cboTokeniseRemContents->Checked);
        ini->WriteBool("BASICLISTING", "TokeniseStringContents", cboTokeniseStringContents->Checked);
        ini->WriteBool("BASICLISTING", "DiscardSurplusSpaces", cboDiscardSurplusSpaces->Checked);
        ini->WriteBool("BASICLISTING", "AcceptAlternateKeywordSpelling", cboAcceptAlternateKeywordSpelling->Checked);
        ini->WriteInteger("BASICLISTING", "AutomaticLineNumberIncrementIndex", cboLineNumberIncrement->ItemIndex);
        ini->WriteBool("BASICLISTING", "SupportZxTokenFormatExtensions", cboAcceptZxTokenFormatExtensions->Checked);
}

void TLoadBasicListingOptionsForm::LoadSettings(TIniFile *ini)
{
        cboTokeniseRemContents->Checked = ini->ReadBool("BASICLISTING", "TokeniseRemContents", false);
        cboTokeniseStringContents->Checked = ini->ReadBool("BASICLISTING", "TokeniseStringContents", false);
        cboDiscardSurplusSpaces->Checked = ini->ReadBool("BASICLISTING", "DiscardSurplusSpaces", false);
        cboAcceptAlternateKeywordSpelling->Checked = ini->ReadBool("BASICLISTING", "AcceptAlternateKeywordSpelling", false);
        cboLineNumberIncrement->ItemIndex = ini->ReadInteger("BASICLISTING", "AutomaticLineNumberIncrementIndex", 0);
        cboAcceptZxTokenFormatExtensions->Checked = ini->ReadBool("BASICLISTING", "SupportZxTokenFormatExtensions", false);
}


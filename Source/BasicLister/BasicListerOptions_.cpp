//---------------------------------------------------------------------------

#include <vcl4.h>
#pragma hdrstop
#include "zx81config.h"
#include "BasicListerOptions_.h"

//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSaveBasicListingOptionsForm *SaveBasicListingOptionsForm;

//---------------------------------------------------------------------------
__fastcall TSaveBasicListingOptionsForm::TSaveBasicListingOptionsForm(TComponent* Owner)
        : TForm(Owner)
{
        TIniFile* ini = new TIniFile(emulator.inipath);
        LoadSettings(ini);
        delete ini;
}
//---------------------------------------------------------------------------

bool TSaveBasicListingOptionsForm::GetOutputRemTokensAsCharacterCodes()
{
        return cboOutputRemTokensAsCharacterCodes->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputStringTokensAsCharacterCodes()
{
        return cboOutputStringTokensAsCharacterCodes->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputNonAsciiAsCharacterCodes()
{
        return cboOutputNonAsciiAsCharacterCodes->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputVariableNamesInLowercase()
{
        return cboOutputVariableNamesInLowercase->Checked;
}

bool TSaveBasicListingOptionsForm::GetLimitLineLengths()
{
        return cboLimitLineLengths->Checked;
}

bool TSaveBasicListingOptionsForm::GetOutputFullWidthLineNumbers()
{
        return cboOutputFullWidthLineNumbers->Checked;
}

int TSaveBasicListingOptionsForm::GetScalingFator()
{
        return (cboTextScaling->Text == "100%") ? 1 : 2;
}

void TSaveBasicListingOptionsForm::SaveSettings(TIniFile *ini)
{
        ini->WriteBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", cboOutputRemTokensAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputStringTokensAsCharacterCodes", cboOutputStringTokensAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", cboOutputNonAsciiAsCharacterCodes->Checked);
        ini->WriteBool("BASICLISTING", "OutputVariableNamesInLowercase", cboOutputVariableNamesInLowercase->Checked);
        ini->WriteBool("BASICLISTING", "LimitLineLengths", cboLimitLineLengths->Checked);
        ini->WriteBool("BASICLISTING", "OutputFullSidthLineNumbers", cboOutputFullWidthLineNumbers->Checked);
        ini->WriteString("BASICLISTING", "TextScaling", cboTextScaling->Text);
}

void TSaveBasicListingOptionsForm::LoadSettings(TIniFile *ini)
{
        cboOutputRemTokensAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputRemTokensAsCharacterCodes", false);
        cboOutputStringTokensAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputStringTokensAsCharacterCodes", false);
        cboOutputNonAsciiAsCharacterCodes->Checked = ini->ReadBool("BASICLISTING", "OutputNonAsciiAsCharacterCodes", false);
        cboOutputVariableNamesInLowercase->Checked = ini->ReadBool("BASICLISTING", "OutputVariableNamesInLowercase", false);
        cboLimitLineLengths->Checked = ini->ReadBool("BASICLISTING", "LimitLineLengths", false);
        cboOutputFullWidthLineNumbers->Checked = ini->ReadBool("BASICLISTING", "OutputFullWidthLineNumbers", false);
        cboTextScaling->Text = ini->ReadString("BASICLISTING", "TextScaling", "100%");
}


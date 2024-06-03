//---------------------------------------------------------------------------

#ifndef BasicLoaderOptions_H
#define BasicLoaderOptions_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "iniFiles.hpp"

//---------------------------------------------------------------------------
class TLoadBasicListingOptionsForm : public TForm
{
__published:	// IDE-managed Components
        TCheckBox *cboTokeniseRemContents;
        TCheckBox *cboTokeniseStringContents;
        TButton *btnOK;
        TLabel *Label1;
        TCheckBox *cboDiscardSurplusSpaces;
        TCheckBox *cboAcceptAlternateKeywordSpelling;
        TComboBox *cboLineNumberIncrement;
        TLabel *Label6;
        TCheckBox *cboAcceptZxTokenFormatExtensions;
private:	// User declarations
public:		// User declarations
        __fastcall TLoadBasicListingOptionsForm(TComponent* Owner);
        bool GetTokeniseRemContents();
        bool GetTokeniseStringContents();
        bool GetDiscardSurplusSpaces();
        bool GetAcceptAlternateKeywordSpelling();
        int GetAutomaticLineNumberIncrement();
        bool GetSupportZxTokenFormatExtensions();
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TLoadBasicListingOptionsForm *LoadBasicListingOptionsForm;
//---------------------------------------------------------------------------
#endif

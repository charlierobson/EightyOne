//---------------------------------------------------------------------------

#ifndef BasicListingFormatInfo_H
#define BasicListingFormatInfo_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
//---------------------------------------------------------------------------
class TBasicListingFormatInfoForm : public TForm
{
__published:	// IDE-managed Components
        TButton *btnOK;
        TLabel *Label26;
        TLabel *Label27;
        TLabel *Label28;
        TLabel *Label25;
        TLabel *Label29;
        TLabel *Label30;
        TLabel *Label31;
        TLabel *Label32;
        TLabel *Label33;
        TLabel *Label34;
        TLabel *Label35;
        TLabel *Label36;
        void __fastcall Weblink1Click(TObject *Sender);
        void __fastcall Weblink2Click(TObject *Sender);
        void __fastcall Weblink3Click(TObject *Sender);
private:	// User declarations
        void OpenWebsite();
public:		// User declarations
        __fastcall TBasicListingFormatInfoForm(TComponent* Owner);
        void SetActivePage(int index);
};
//---------------------------------------------------------------------------
extern PACKAGE TBasicListingFormatInfoForm *BasicListingFormatInfoForm;
//---------------------------------------------------------------------------
#endif

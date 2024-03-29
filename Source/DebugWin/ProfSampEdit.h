//---------------------------------------------------------------------------

#ifndef ProfSampEditH
#define ProfSampEditH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "zx81config.h"
//---------------------------------------------------------------------------
class TProfileSampleEdit : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TEdit *EditTag;
        TEdit *EditStart;
        TEdit *EditEnd;
        TButton *ButtonOK;
        TLabel *Label4;
        void __fastcall ButtonOKClick(TObject *Sender);
        void __fastcall EditDblClick(TObject *Sender);
        void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
        void __fastcall EditStartChange(TObject *Sender);
        void __fastcall EditEndChange(TObject *Sender);
private:	// User declarations
        bool _valid;
        class ProfileDetail* _pd;
        void (*_completion)(bool, ZXString);
        void SetOKButtonStatus();
        int Count();

public:		// User declarations
        __fastcall TProfileSampleEdit(TComponent* Owner);

        void __fastcall EditValues(ZXString tag, ProfileDetail* pd, void(*)(bool, ZXString));
};
//---------------------------------------------------------------------------
extern PACKAGE TProfileSampleEdit *ProfileSampleEdit;
//---------------------------------------------------------------------------
#endif

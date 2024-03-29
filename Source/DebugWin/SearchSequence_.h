//---------------------------------------------------------------------------

#ifndef SearchSequence_H
#define SearchSequence_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <vector>
#include "zx81config.h"

//---------------------------------------------------------------------------
class TSearchSequence : public TForm
{
__published:	// IDE-managed Components
        TLabel *Label1;
        TEdit *EditSequence;
        TButton *Cancel;
        TButton *Find;
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall FindClick(TObject *Sender);
private:	// User declarations
        bool cancelled;
public:		// User declarations
        __fastcall TSearchSequence(TComponent* Owner);
        void CentreOn(TForm* parent);
        bool EditSequenceBytes(ZXString& searchText, std::vector<int>& bytes);
        bool ValidateSearchSequence(std::vector<int>& bytes);
};
//---------------------------------------------------------------------------
extern PACKAGE TSearchSequence *SearchSequence;
//---------------------------------------------------------------------------
#endif

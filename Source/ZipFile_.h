//---------------------------------------------------------------------------

#ifndef ZipFile_H
#define ZipFile_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <IniFiles.hpp>
#include "zx81config.h"

//---------------------------------------------------------------------------
class TZipFile : public TForm
{
__published:	// IDE-managed Components
        TListBox *ListBox;
        TButton *OK;
        TLabel *Label1;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ListBoxDblClick(TObject *Sender);
private:	// User declarations
        bool Cancelled;
        ZXString LastFile;
public:		// User declarations
        __fastcall TZipFile(TComponent* Owner);
        ZXString ExpandZIP(ZXString Path, ZXString Filter);
        void LoadSettings(TIniFile *ini);
        void SaveSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TZipFile *ZipFile;
//---------------------------------------------------------------------------
#endif

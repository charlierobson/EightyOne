//---------------------------------------------------------------------------

#ifndef midifrmH
#define midifrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
//---------------------------------------------------------------------------
class TMidiForm : public TForm
{
__published:	// IDE-managed Components
        TGroupBox *GroupBox1;
        TComboBox *MidiBox;
        TButton *OK;
        TGroupBox *GroupBox2;
        TTrackBar *ChAVol;
        TTrackBar *ChBVol;
        TTrackBar *ChCVol;
        TTrackBar *BeeperVol;
        TLabel *Label1;
        TLabel *Label2;
        TLabel *Label3;
        TLabel *Label4;
        TLabel *Label5;
        TCheckBox *AYMute;
        TCheckBox *BeeperMute;
        TLabel *Label6;
        TLabel *Label7;
        TRadioButton *RadioButtonMono;
        TRadioButton *RadioButtonACB;
        TGroupBox *BeeperSettingsGroupBox;
        TCheckBox *BeeperExcludeHSyncs;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormDestroy(TObject *Sender);
        void __fastcall MidiChange(TObject *Sender);
        void __fastcall BeeperExcludeHSyncsClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TMidiForm(TComponent* Owner);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);
};
//---------------------------------------------------------------------------
extern PACKAGE TMidiForm *MidiForm;
//---------------------------------------------------------------------------
#endif

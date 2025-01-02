//---------------------------------------------------------------------------

#ifndef HW_H
#define HW_H
#include "OffBtn.hpp"
#include <Classes.hpp>
#include <ComCtrls.hpp>
#include <Controls.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Graphics.hpp>
#include <ImgList.hpp>
#include <StdCtrls.hpp>
#include <vector>

using namespace std;

//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "OffBtn.hpp"
#include <ComCtrls.hpp>
#include <IniFiles.hpp>
#include <Dialogs.hpp>
#include <ExtCtrls.hpp>
#include <Buttons.hpp>
#include <ImgList.hpp>
#include <Graphics.hpp>

//---------------------------------------------------------------------------

#include "zx81config.h"

struct RomCartridgeEntry
{
public:
        RomCartridgeEntry(ZXString title, ZXString path)
        {
                Title = title;
                Path = path;
        }

        ZXString Title;
        ZXString Path;
};

class THW : public TForm
{
__published:	// IDE-managed Components
        TPageControl *Machine;
        TTabSheet *Sinclair;
        TTabSheet *Amstrad;
        TTabSheet *Timex;
        TOffice97Button *ZX80Btn;
        TOffice97Button *ZX81Btn;
        TOffice97Button *Spec48Btn;
        TOffice97Button *SpecPlusBtn;
        TOffice97Button *Spec128Btn;
        TTabSheet *Others;
        TOffice97Button *LambdaBtn;
        TOffice97Button *R470Btn;
        TOffice97Button *TK85Btn;
        TOffice97Button *AceBtn;
        TOffice97Button *TS1000Btn;
        TOffice97Button *TS1500Btn;
        TOffice97Button *SpecP2Btn;
        TOffice97Button *SpecP2aBtn;
        TButton *OK;
        TLabel *RamPackLbl;
        TComboBox *RamPackBox;
        TOffice97Button *SpecP3Btn;
        TOffice97Button *Spec16Btn;
        TOpenDialog *RomSelect;
        TOffice97Button *TC2048Btn;
        TOffice97Button *TS2068Btn;
        TTabSheet *HomeBrew;
        TOffice97Button *ZX97LEBtn;
        TPageControl *Advanced;
        TTabSheet *Interfaces;
        TTabSheet *FloppyDrives;
        TLabel *LabelA;
        TLabel *LabelB;
        TComboBox *DriveAType;
        TComboBox *DriveBType;
        TTabSheet *QLSettings;
        TLabel *SoundCardLbl;
        TComboBox *SoundCardBox;
        TLabel *ChrGenLbl;
        TComboBox *ChrGenBox;
        TLabel *HiResLbl;
        TComboBox *HiResBox;
        TCheckBox *KMouse;
        TCheckBox *TS2050;
        TCheckBox *Multiface;
        TButton *TS2050Config;
        TOffice97Button *QLBtn;
        TTabSheet *AdvSettings;
        TLabel *LabelRomFile;
        TButton *BrowseROM;
        TCheckBox *ProtectROM;
        TCheckBox *NTSC;
        TCheckBox *EnableLowRAM;
        TCheckBox *M1Not;
        TCheckBox *Issue2;
        TComboBox *QLCPU;
        TComboBox *QLMem;
        TLabel *LabelProcessor;
        TLabel *LabelMemory;
        TLabel *LabelFDC;
        TComboBox *FDC;
        TCheckBox *Autoboot;
        TButton *IF1Config;
        TComboBox *IDEBox;
        TComboBox *ZXCFRAM;
        TCheckBox *Upload;
        TLabel *LabelIDE;
        TLabel *ZXCFLabel;
        TCheckBox *ZXPrinter;
        TCheckBox *ZXpand;
        TComboBox *ColourBox;
        TLabel *ColourLabel;
        TLabel *RomCartridgeLabel;
        TComboBox *RomCartridgeBox;
        TEdit *RomCartridgeFileBox;
        TButton *BrowseRomCartridge;
        TCheckBox *FloatingPointHardwareFix;
        TButton *ButtonZXpandSDCard;
        TComboBox *ZXC1ConfigurationBox;
        TButton *ButtonAdvancedMore;
        TCheckBox *ImprovedWait;
        TComboBox *RomBox;
        TImageList *ZX80Icons;
        TComboBox *SinclairRomCartridgeFileBox;
        TLabel *LabelTotalRAM;
        TCheckBox *uSource;
        TImageList *Spec128Icons;
        TComboBox *TS1510RomCartridgeFileBox;
        TComboBox *TS2068RomCartridgeFileBox;
        TOffice97Button *TC2068Btn;
        TComboBox *TC2068RomCartridgeFileBox;
        TImage *ZXpandEmulationInfo;
        TLabel *SpeechBoxLbl;
        TComboBox *SpeechBox;
        void __fastcall OKClick(TObject *Sender);
        void __fastcall ZX80BtnClick(TObject *Sender);
        void __fastcall ZX81BtnClick(TObject *Sender);
        void __fastcall Spec48BtnClick(TObject *Sender);
        void __fastcall SpecPlusBtnClick(TObject *Sender);
        void __fastcall Spec128BtnClick(TObject *Sender);
        void __fastcall Spec16BtnClick(TObject *Sender);
        void __fastcall SpecP2BtnClick(TObject *Sender);
        void __fastcall SpecP2aBtnClick(TObject *Sender);
        void __fastcall SpecP3BtnClick(TObject *Sender);
        void __fastcall TS1000BtnClick(TObject *Sender);
        void __fastcall TS1500BtnClick(TObject *Sender);
        void __fastcall LambdaBtnClick(TObject *Sender);
        void __fastcall R470BtnClick(TObject *Sender);
        void __fastcall TK85BtnClick(TObject *Sender);
        void __fastcall AceBtnClick(TObject *Sender);
        void __fastcall TS2050Click(TObject *Sender);
        void __fastcall FormShow(TObject *Sender);
        void __fastcall TS2050ConfigClick(TObject *Sender);
        void __fastcall BrowseROMClick(TObject *Sender);
        void __fastcall TC2048BtnClick(TObject *Sender);
        void __fastcall TS2068BtnClick(TObject *Sender);
        void __fastcall ZX97LEBtnClick(TObject *Sender);
        void __fastcall RamPackBoxChange(TObject *Sender);
        void __fastcall NTSCClick(TObject *Sender);
        void __fastcall EnableLowRAMClick(TObject *Sender);
        void __fastcall M1NotClick(TObject *Sender);
        void __fastcall LambdaColourClick(TObject *Sender);
        void __fastcall SoundCardBoxChange(TObject *Sender);
        void __fastcall ChrGenBoxChange(TObject *Sender);
        void __fastcall HiResBoxChange(TObject *Sender);
        void __fastcall DriveATypeChange(TObject *Sender);
        void __fastcall DriveBTypeChange(TObject *Sender);
        void __fastcall IF1ConfigClick(TObject *Sender);
        void __fastcall MultifaceClick(TObject *Sender);
        void __fastcall IDEBoxChange(TObject *Sender);
        void __fastcall QLBtnClick(TObject *Sender);
        void __fastcall FDCChange(TObject *Sender);
        void __fastcall uSpeechClick(TObject *Sender);
        void __fastcall ZXpandClick(TObject *Sender);
        void __fastcall BrowseRomCartridgeClick(TObject *Sender);
        void __fastcall RomCartridgeBoxChange(TObject *Sender);
        void __fastcall ColourBoxChange(TObject *Sender);
        void __fastcall RomBoxChange(TObject *Sender);
        void __fastcall ZXPrinterClick(TObject *Sender);
        void __fastcall FloatingPointHardwareFixClick(TObject *Sender);
        void __fastcall ButtonZXpandSDCardClick(TObject *Sender);
        void __fastcall ButtonAdvancedMoreClick(TObject *Sender);
        void __fastcall ImprovedWaitClick(TObject *Sender);
        void __fastcall SinclairRomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall TS1510RomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall TS2068RomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall uSourceClick(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);
        void __fastcall TC2068BtnClick(TObject *Sender);
        void __fastcall TC2068RomCartridgeFileBoxChange(TObject *Sender);
        void __fastcall ZXpandEmulationInfoClick(TObject *Sender);
        void __fastcall SpeechBoxChange(TObject *Sender);
private:	// User declarations
        int RamPackHeight;
        int NewMachine, NewSpec;
        ZXString NewMachineName;
        void SetupForZX81(void);
        void SetupForSpectrum(void);
        void SetupForQL(void);
        void EnableRomCartridgeOption(bool enable);
        void ResetDisplaySize();
        void SetZX80Icon();
        void SetSpectrum128Icon();
        void PopulateRomCartridgeSinclairList();
        void PopulateRomCartridgeTS1510List();
        void PopulateRomCartridgeTS2068List();
        void PopulateRomCartridgeTC2068List();
        void AddRomCartridgeFile(TComboBox* romCartridgeFileBox, RomCartridgeEntry* romCartridgeEntry, ZXString fileExt);
        ZXString DirectSoundError(unsigned int errorCode);
        void ConfigureDefaultRamSettings();
        void DisplayTotalRam();
        int UpdateRomCartridgeControls(int machine, int spectrumModel);
        int DetermineRomCartridgeType(ZXString cartridgeText, int machine, int spectrumModel);

        void ConfigureRzxSupport();
        void ReInitialiseSound();
        void ResetDebugger();
        void ConfigureDisplayArtifacts();
        void ConfigureM1Not();
        void ConfigureModem();
        void ConfigurePrinterCentronicsPort();
        void SelectGroupboxVisibility();
        void ConfigureRamTop();
        void InitialiseSound(bool machineChanged);
        void CloseLiveMemoryWindow(bool machineChanged);
        void CloseOtherDebugWindow();
        void DetermineRamSizeLabel(ZXString newMachineName);
        void ConfigureMultifaceRom();
        void ConfigureRom();
        void ConfigureZXpand();
        void ConfigureBasicLister();
        void ConfigureColour();
        void ConfigureChroma(bool prevChromaColourSwitchOn);
        void ConfigureSpectra(bool prevSpectraColourSwitchOn);
        void ConfigureRomCartridge();
        void ConfigureCharacterGenerator();
        void ConfigureHiRes();
        void ConfigureSound();
        void ConfigureSpeech();
        void ConfigureSpectrumIDE();
        void ConfigureFDC();
        void ConfigureMachineSettings();
        ZXString DetermineRomBase();
        void ConfigureSymbolFile(ZXString romBase);
        void ConfigureCharacterBitmapFile(ZXString romBase);
        void SetUpRomCartridges();
        void Configure8K16KRam();

        vector<RomCartridgeEntry> ts1510RomCartridges;
        vector<RomCartridgeEntry> ts2068RomCartridges;
        vector<RomCartridgeEntry> tc2068RomCartridges;
        vector<RomCartridgeEntry> sinclairRomCartridges;

public:		// User declarations
        bool ResetRequired;
        __fastcall THW(TComponent* Owner);
        void UpdateHardwareSettings(bool reinitialiseStatus, bool disableReset);
        void SaveSettings(TIniFile *ini);
        void LoadSettings(TIniFile *ini);
        void SetZXpandState(bool checked, bool enabled);
};
//---------------------------------------------------------------------------
extern PACKAGE THW *HW;
//---------------------------------------------------------------------------
#endif

//---------------------------------------------------------------------------

#ifndef SetBreakpoint_H
#define SetBreakpoint_H
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include "Debug.h"

//---------------------------------------------------------------------------
class TSetBreakpoint : public TForm
{
__published:	// IDE-managed Components
        TButton *OK;
        TButton *Cancel;
        TLabel *LabelBreakAddress;
        TEdit *BreakAddress;
        TLabel *LabelConditionValue;
        TEdit *BreakValue;
        TLabel *LabelBreakValue;
        TComboBox *BreakConditionAddr;
        TLabel *LabelType;
        TComboBox *BreakType;
        TComboBox *RegisterList;
        TComboBox *FlagList;
        TComboBox *BreakConditionValue;
        TLabel *LabelConditionAddr;
        TComboBox *ComboBoxBreakEnabled;
        TLabel *LabelBreakEnabled;
        TComboBox *BreakHitCount;
        TLabel *LabelBreakHitcount;
        void __fastcall CancelClick(TObject *Sender);
        void __fastcall OKClick(TObject *Sender);
        void __fastcall FormKeyPress(TObject *Sender, char &Key);
        void __fastcall BreakConditionAddrChange(TObject *Sender);
        void __fastcall BreakValueChange(TObject *Sender);
        void __fastcall BreakAddressChange(TObject *Sender);
        void __fastcall BreakTypeChange(TObject *Sender);
        void __fastcall RegisterListChange(TObject *Sender);
        void __fastcall FormCreate(TObject *Sender);

private:	// User declarations
        bool cancelled;
        bool Register16Bit(RegisterType registerIndex);
        void EnableOkButton();
        void ValidateBreakAddress();
        void ValidateBreakValue();
        void GetBreakAddressLimits(BreakpointType type, int& lowerLimit, int& upperLimit);
        void GetBreakValueLimits(BreakpointType type, BreakpointCondition condition, RegisterType registerIndex, int& lowerLimit, int& upperLimit);
        int GetBreakAddressMaxDigits(BreakpointType type);
        int GetBreakValueMaxDigits(BreakpointType type, BreakpointCondition condition, RegisterType registerIndex);
        void ConfigureBreakpointFields(struct breakpoint& bp);
        bool GetBreakpointFields(struct breakpoint& bp);
        void BreakTypeChangeExe();
        void BreakTypeChangeRdWrInOut();
        void BreakTypeChangeInOutByte();
        void BreakTypeChangeRegister();
        void BreakTypeChangeFlag();
        void BreakTypeChangeMem();
        void BreakTypeChangeTStates();
        void SetConditionList(TComboBox* const conditionList, ZXString conditions);
        void SetEditBox(TEdit* const editBox, ZXString defaultText);
        void SetEditBoxLabels(ZXString breakAddressLabel, ZXString breakValueLabel);

public:		// User declarations
        __fastcall TSetBreakpoint(TComponent* Owner);
        void SetTitle(ZXString& title);
        bool EditBreakpoint(struct breakpoint& bp);
        void CentreOn(TForm* parent);
};
//---------------------------------------------------------------------------
extern PACKAGE TSetBreakpoint *SetBreakpoint;
//---------------------------------------------------------------------------
#endif

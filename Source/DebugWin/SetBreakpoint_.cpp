//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

#include "SetBreakpoint_.h"
#include "symbolstore.h"
#include "z80.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSetBreakpoint *SetBreakpoint;
//---------------------------------------------------------------------------
__fastcall TSetBreakpoint::TSetBreakpoint(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::CancelClick(TObject *Sender)
{
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::OKClick(TObject *Sender)
{
        cancelled = false;
        Close();
}
//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::FormKeyPress(TObject *Sender, char &Key)
{
        if (Key == VK_ESCAPE)
        {
                cancelled = true;
                Close();
        }        
}
//---------------------------------------------------------------------------

void TSetBreakpoint::CentreOn(TForm* parent)
{
        Top = parent->Top + (parent->Height - Height) / 2;
        Left = parent->Left + (parent->Width - Width) / 2;
}
//---------------------------------------------------------------------------
void TSetBreakpoint::SetTitle(ZXString& title)
{
        Text = title;
}
//---------------------------------------------------------------------------

bool TSetBreakpoint::EditBreakpoint(struct breakpoint& bp)
{
        ConfigureBreakpointFields(bp);

        // OK button resets this. Every other way of closing the dialog is a cancel
        cancelled = true;

        ShowModal();

        if (cancelled)
        {
                return false;
        }

        bool fieldsValid = GetBreakpointFields(bp);

        return fieldsValid;
}
//---------------------------------------------------------------------------

void ReplaceRegisterName(ZXString& addressValue)
{
        if (CompareText(addressValue, "PC") == 0)
        {
                addressValue = z80.pc.w;
        }
        else if (CompareText(addressValue, "SP") == 0)
        {
                addressValue = z80.sp.w;
        }
        else if (CompareText(addressValue, "AF") == 0)
        {
                addressValue = z80.af.w;
        }
        else if (CompareText(addressValue, "HL") == 0)
        {
                addressValue = z80.hl.w;
        }
        else if (CompareText(addressValue, "DE") == 0)
        {
                addressValue = z80.de.w;
        }
        else if (CompareText(addressValue, "BC") == 0)
        {
                addressValue = z80.bc.w;
        }
        else if (CompareText(addressValue, "IX") == 0)
        {
                addressValue = z80.ix.w;
        }
        else if (CompareText(addressValue, "IY") == 0)
        {
                addressValue = z80.iy.w;
        }
        else if (CompareText(addressValue, "AF'") == 0)
        {
                addressValue = z80.af_.w;
        }
        else if (CompareText(addressValue, "HL'") == 0)
        {
                addressValue = z80.hl_.w;
        }
        else if (CompareText(addressValue, "DE'") == 0)
        {
                addressValue = z80.de_.w;
        }
        else if (CompareText(addressValue, "BC'") == 0)
        {
                addressValue = z80.bc_.w;
        }
}

bool SanitiseEditBox(ZXString addressValue, int& addr, int minValue, int maxValue, bool symbolLookUp = false)
{
        int newAddr = addr;

        addressValue = addressValue.Trim();

        if (addressValue.Length() == 0)
        {
                return false;
        }

        if (symbolLookUp)
        {
                if (symbolstore::symbolToAddress(addressValue, newAddr))
                {
                        addressValue = "0x" + IntToHex(newAddr, 4);
                }
                else
                {
                        ReplaceRegisterName(addressValue);
                }
        }
        
        bool hex = (addressValue[1] == '$');
        if (hex)
        {
                if (addressValue.Length() == 1)
                {
                        return false;
                }

                // replace the zx-world hex identifier with the standard c library one.
                addressValue = "0x" + addressValue.SubString(2, addressValue.Length() - 1);
        }

        _TCHAR* endPtr;
        if (hex)
        {
				newAddr = int(_tcstol(addressValue.c_str(), &endPtr, 16));
        }
        else
        {
				newAddr = int(_tcstol(addressValue.c_str(), &endPtr, 10));
        }

        if (*endPtr != 0)
        {
                return false;
        }

        if ((newAddr < minValue) || (newAddr > maxValue))
        {
                return false;
        }

        addr = newAddr;

        return true;
}

void TSetBreakpoint::ConfigureBreakpointFields(struct breakpoint& bp)
{
        BreakType->ItemIndex = bp.Type;
        BreakTypeChange(NULL);

        BreakConditionAddr->ItemIndex = bp.ConditionAddr;
        BreakConditionAddrChange(NULL);

        if (bp.Type == BP_REGISTER)
        {
                RegisterList->ItemIndex = bp.RegisterId;
        }
        else if (bp.Type == BP_FLAG)
        {
                FlagList->ItemIndex = bp.FlagId;
        }
        else
        {
                int maxDigits = GetBreakAddressMaxDigits(bp.Type);

                if ((BreakType->ItemIndex == BP_INH) || (BreakType->ItemIndex == BP_OUTH))
                {
                        BreakAddress->Text = "$" + IntToHex(bp.Addr >> 8, maxDigits);
                }
                else
                {
                        BreakAddress->Text = "$" + IntToHex(bp.Addr, maxDigits);
                }
        }

        BreakConditionValue->ItemIndex = bp.ConditionValue;

        if ((BreakType->ItemIndex == BP_TCYCLES) || (BreakType->ItemIndex == BP_FLAG))
        {
                BreakValue->Text = bp.Value;
        }
        else
        {
                BreakValue->Text = "$" + IntToHex(bp.Value, GetBreakValueMaxDigits(bp.Type, bp.ConditionAddr, bp.RegisterId));
        }

        const int Yes = 0;
        const int No = 1;
        ComboBoxBreakEnabled->ItemIndex = bp.Enabled ? Yes : No;

        if (BreakAddress->Visible)
        {
                ActiveControl = BreakAddress;
                BreakAddress->SelectAll();
        }
        else if (RegisterList->Visible)
        {
                ActiveControl = RegisterList;
        }
        else if (FlagList->Visible)
        {
                ActiveControl = FlagList;
        }

        BreakHitCount->ItemIndex = bp.HitCount - 1;
}

bool SymbolLookUpRequired(int breakType)
{
        return ((breakType != BP_INL) && (breakType != BP_INH) &&
                (breakType != BP_OUTL) && (breakType != BP_OUTH));
}

bool TSetBreakpoint::GetBreakpointFields(struct breakpoint& bp)
{
        ZXString breakAddress;
        ZXString breakValue;
        int lowerLimit;
        int upperLimit;
        int addr;
        int value;

        if (BreakType->ItemIndex == BP_REGISTER)
        {
                addr = (RegisterType)RegisterList->ItemIndex;
        }
        else if (BreakType->ItemIndex == BP_FLAG)
        {
                addr = (FlagType)FlagList->ItemIndex;
        }
        else
        {
                breakAddress = BreakAddress->Text.Trim();
                GetBreakAddressLimits((BreakpointType)BreakType->ItemIndex, lowerLimit, upperLimit);

                bool symbolLookUp = SymbolLookUpRequired(BreakType->ItemIndex);
                if (!SanitiseEditBox(breakAddress, addr, lowerLimit, upperLimit, symbolLookUp))
                {
                        return false;
                }
        }

        breakValue = BreakValue->Text.Trim();
        GetBreakValueLimits((BreakpointType)BreakType->ItemIndex, (BreakpointCondition)BreakConditionAddr->ItemIndex, (RegisterType)RegisterList->ItemIndex, lowerLimit, upperLimit);

        bool symbolLookUp = (BreakType->ItemIndex == BP_EXE);
        if (!SanitiseEditBox(breakValue, value, lowerLimit, upperLimit, symbolLookUp))
        {
                return false;
        }

        bp.Type = (BreakpointType)BreakType->ItemIndex;
        bp.ConditionAddr = (BreakpointCondition)BreakConditionAddr->ItemIndex;

        if ((bp.Type == BP_INH) || (bp.Type == BP_OUTH))
        {
                bp.Addr = addr << 8;
        }
        else
        {
                bp.Addr = addr;
        }
        bp.ConditionValue = (BreakpointCondition)BreakConditionValue->ItemIndex;
        bp.Value = value;
        bp.Permanent = true;
        bp.Enabled = (ComboBoxBreakEnabled->Text == "Yes");
        bp.HitCount = StrToInt(BreakHitCount->Text);
        bp.Hits = 0;

        return true;
}

//---------------------------------------------------------------------------

void __fastcall TSetBreakpoint::BreakTypeChange(TObject *Sender)
{
        RegisterList->Visible = false;
        FlagList->Visible = false;
        BreakAddress->Visible = false;

        switch (BreakType->ItemIndex)
        {
                case BP_EXE:
                        BreakTypeChangeExe();
                        break;

                case BP_RD:
                case BP_WR:
                case BP_IN:
                case BP_OUT:
                        BreakTypeChangeRdWrInOut();
                        break;

                case BP_INL:
                case BP_INH:
                case BP_OUTL:
                case BP_OUTH:
                        BreakTypeChangeInOutByte();
                        break;

                case BP_MEMORY:
                        BreakTypeChangeMem();
                        break;

                case BP_REGISTER:
                        BreakTypeChangeRegister();
                        break;

                case BP_TCYCLES:
                        BreakTypeChangeTStates();
                        break;

                case BP_FLAG:
                        BreakTypeChangeFlag();
                        break;
        }

        BreakConditionAddrChange(Sender);
}

void TSetBreakpoint::SetConditionList(TComboBox* const conditionList, ZXString conditions)
{
        conditionList->Items->Clear();

        int s = 1;
        int e = 1;

        while (e < conditions.Length())
        {
                if (conditions[e] == ',')
                {
                        conditionList->Items->Add(conditions.SubString(s, e - s));
                        s = e + 1;
                }

                e++;
        }

        ZXString lastCondition = conditions.SubString(s, e + 1 - s);
        conditionList->Items->Add(lastCondition);
        conditionList->ItemIndex = 0;
        conditionList->Enabled = (conditionList->Items->Count > 1);

        if (lastCondition == "@")
        {
                conditionList->Items->Add(lastCondition);
                conditionList->Items->Add(lastCondition);
                conditionList->Items->Add(lastCondition);
                conditionList->Items->Add(lastCondition);
                conditionList->Items->Add(lastCondition);
                conditionList->ItemIndex = 5;
        }
}

void TSetBreakpoint::SetEditBox(TEdit* const editBox, ZXString defaultText)
{
        editBox->MaxLength = defaultText.Length();
        editBox->Text = defaultText.Trim();
        editBox->Visible = true;
        editBox->Enabled = true;
}

void TSetBreakpoint::BreakTypeChangeExe()
{
        SetConditionList(BreakConditionAddr, "=,<>,<=,>=,->");
        SetEditBox(BreakAddress, "$0000");
        SetConditionList(BreakConditionValue, "...");
        SetEditBox(BreakValue, "$0000");
        BreakValue->Enabled = false;
}

void TSetBreakpoint::BreakTypeChangeRdWrInOut()
{
        SetConditionList(BreakConditionAddr, "=,<>,<=,>=,->");
        SetEditBox(BreakAddress, "$0000");
        SetConditionList(BreakConditionValue, "=,<>,<=,>=");
        BreakConditionValue->ItemIndex = GreaterThanEquals;
        SetEditBox(BreakValue, "$00");
}

void TSetBreakpoint::BreakTypeChangeInOutByte()
{
        SetConditionList(BreakConditionAddr, "=,<>,<=,>=");
        SetEditBox(BreakAddress, "$00");
        SetConditionList(BreakConditionValue, "=,<>,<=,>=");
        BreakConditionValue->ItemIndex = GreaterThanEquals;
        SetEditBox(BreakValue, "$00");
}

void TSetBreakpoint::BreakTypeChangeMem()
{
        SetConditionList(BreakConditionAddr, "=");
        SetEditBox(BreakAddress, "$0000");
        SetConditionList(BreakConditionValue, "=,<>,<=,>=");
        SetEditBox(BreakValue, "$00");
}

void TSetBreakpoint::BreakTypeChangeRegister()
{
        SetConditionList(BreakConditionAddr, "=");
        RegisterList->ItemIndex = RegBC;
        RegisterList->Visible = true;
        SetConditionList(BreakConditionValue, "=,<>,<=,>=");
        SetEditBox(BreakValue, "$0000");
}

void TSetBreakpoint::BreakTypeChangeFlag()
{
        SetConditionList(BreakConditionAddr, "=");
        FlagList->ItemIndex = Carry;
        FlagList->Visible = true;
        SetConditionList(BreakConditionValue, "=,<>");
        SetEditBox(BreakValue, "0");
}

void TSetBreakpoint::BreakTypeChangeTStates()
{
        SetConditionList(BreakConditionAddr, "@");
        SetEditBox(BreakAddress, "$0000");
        SetConditionList(BreakConditionValue, "<>");
        SetEditBox(BreakValue, "0    ");
}

//---------------------------------------------------------------------------

void TSetBreakpoint::SetEditBoxLabels(ZXString breakAddressLabel, ZXString breakValueLabel)
{
        LabelBreakAddress->Caption = breakAddressLabel + ":";
        LabelBreakValue->Caption = breakValueLabel + ":";
}

//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::BreakConditionAddrChange(TObject *Sender)
{
        BreakConditionValue->ItemIndex = 0;
        BreakValue->Enabled = true;

        switch (BreakConditionAddr->ItemIndex)
        {
                case Range:
                        SetEditBoxLabels("Addr Start", "Addr End");
                        SetConditionList(BreakConditionValue, "...");
                        SetEditBox(BreakValue, "$FFFF");
                        break;

                case Equal:
                case LessThanEquals:
                case GreaterThanEquals:
                case NotEqual:
                case At:                
                        switch (BreakType->ItemIndex)
                        {
                                case BP_EXE:
                                        SetEditBoxLabels("Addr Start", "Addr End");
                                        BreakValue->Text = BreakAddress->Text;
                                        BreakValue->Enabled = false;
                                        break;

                                case BP_RD:
                                case BP_WR:
                                case BP_IN:
                                case BP_OUT:
                                        SetEditBoxLabels("Address", "Value");
                                        SetConditionList(BreakConditionValue, "=,<>,<=,>=");
                                        BreakConditionValue->ItemIndex = GreaterThanEquals;
                                        SetEditBox(BreakValue, "$00");
                                        break;

                                case BP_MEMORY:
                                        SetEditBoxLabels("Address", "Value");
                                        break;

                                case BP_INL:
                                case BP_OUTL:
                                        SetEditBoxLabels("Addr A7-0", "Value");
                                        BreakConditionValue->ItemIndex = GreaterThanEquals;
                                        break;

                                case BP_INH:
                                case BP_OUTH:
                                        SetEditBoxLabels("Addr A15-8", "Value");
                                        BreakConditionValue->ItemIndex = GreaterThanEquals;
                                        break;

                                case BP_TCYCLES:
                                        SetEditBoxLabels("Address", "T-States");
                                        break;

                                case BP_REGISTER:
                                        SetEditBoxLabels("Register", "Value");
                                        break;

                                case BP_FLAG:
                                        SetEditBoxLabels("Bit", "Value");
                                        break;

                        }
                        break;
        }

        BreakAddressChange(NULL);
}

//---------------------------------------------------------------------------

void __fastcall TSetBreakpoint::RegisterListChange(TObject *Sender)
{
        int maxDigits = GetBreakValueMaxDigits((BreakpointType)BreakType->ItemIndex, (BreakpointCondition)BreakConditionAddr->ItemIndex, (RegisterType)RegisterList->ItemIndex);
        if ((BreakValue->MaxLength - 1) > maxDigits)
        {
                SetEditBox(BreakValue, "$00");
        }
        else if ((BreakValue->MaxLength - 1) < maxDigits)
        {
                SetEditBox(BreakValue, "$0000");
        }
}

//---------------------------------------------------------------------------

void __fastcall TSetBreakpoint::BreakAddressChange(TObject *Sender)
{
        ValidateBreakAddress();

        if ((BreakType->ItemIndex == BP_EXE) && (BreakConditionAddr->ItemIndex != Range))
        {
                BreakValue->Text = BreakAddress->Text;
                BreakValue->Font->Color = BreakAddress->Font->Color;
                EnableOkButton();
        }
        else
        {
                ValidateBreakValue();
        }
}
//---------------------------------------------------------------------------

void __fastcall TSetBreakpoint::BreakValueChange(TObject *Sender)
{
        ValidateBreakValue();
}
//---------------------------------------------------------------------------

void TSetBreakpoint::ValidateBreakAddress()
{
        int lowerLimit;
        int upperLimit;
        GetBreakAddressLimits((BreakpointType)BreakType->ItemIndex, lowerLimit, upperLimit);

        int value;
        bool symbolLookUp = SymbolLookUpRequired(BreakType->ItemIndex);
        bool valid = SanitiseEditBox(BreakAddress->Text, value, lowerLimit, upperLimit, symbolLookUp);
        BreakAddress->Font->Color = valid ? clWindowText : clRed;

        EnableOkButton();
}
//---------------------------------------------------------------------------

void TSetBreakpoint::ValidateBreakValue()
{
        int lowerLimit;
        int upperLimit;
        GetBreakValueLimits((BreakpointType)BreakType->ItemIndex, (BreakpointCondition)BreakConditionAddr->ItemIndex, (RegisterType)RegisterList->ItemIndex, lowerLimit, upperLimit);

        int value;
        bool valid = SanitiseEditBox(BreakValue->Text, value, lowerLimit, upperLimit);

        if (valid)
        {
                if (BreakConditionAddr->ItemIndex == Range)
                {
                        GetBreakAddressLimits((BreakpointType)BreakType->ItemIndex, lowerLimit, upperLimit);

                        int addr;
                        bool symbolLookUp = (BreakType->ItemIndex == BP_EXE);
                        valid = SanitiseEditBox(BreakAddress->Text, addr, lowerLimit, upperLimit, symbolLookUp);

                        if (valid)
                        {
                                valid = (addr < value);
                        }
                }
        }

        BreakValue->Font->Color = valid ? clWindowText : clRed;

        EnableOkButton();
}
//---------------------------------------------------------------------------

int TSetBreakpoint::GetBreakAddressMaxDigits(BreakpointType type)
{
        int limit;

        switch (type)
        {
                case BP_INL:
                case BP_INH:
                case BP_OUTL:
                case BP_OUTH:
                        limit = 2;
                        break;

                default:
                        limit = 4;
                        break;
        }

        return limit;
}
//---------------------------------------------------------------------------

int TSetBreakpoint::GetBreakValueMaxDigits(BreakpointType type, BreakpointCondition condition, RegisterType registerIndex)
{
        int limit;

        switch (type)
        {
                case BP_EXE:
                        limit = 4;
                        break;

                case BP_RD:
                case BP_WR:
                case BP_IN:
                case BP_OUT:
                        if (condition == Range)
                        {
                                limit = 4;
                        }
                        else
                        {
                                limit = 2;
                        }
                        break;

                case BP_INL:
                case BP_INH:
                case BP_OUTL:
                case BP_OUTH:
                        limit = 2;
                        break;

                case BP_TCYCLES:
                        limit = 5;
                        break;

                case BP_REGISTER:
                        if (Register16Bit(registerIndex))
                        {
                                limit = 4;
                        }
                        else
                        {
                                limit = 2;
                        }
                        break;

                case BP_MEMORY:
                        limit = 2;
                        break;

                case BP_FLAG:
                        limit = 1;
                        break;
        }

        return limit;
}
//---------------------------------------------------------------------------

bool TSetBreakpoint::Register16Bit(RegisterType registerIndex)
{
        return (registerIndex < RegA);
}
//---------------------------------------------------------------------------

void TSetBreakpoint::GetBreakAddressLimits(BreakpointType type, int& lowerLimit, int& upperLimit)
{
        lowerLimit = 0;

        switch (type)
        {
                case BP_INL:
                case BP_INH:
                case BP_OUTL:
                case BP_OUTH:
                        upperLimit = 0xFF;
                        break;

                default:
                        upperLimit = 0xFFFF;
                        break;
        }
}

//---------------------------------------------------------------------------

void TSetBreakpoint::GetBreakValueLimits(BreakpointType type, BreakpointCondition condition, RegisterType registerIndex, int& lowerLimit, int& upperLimit)
{
        lowerLimit = 0;
        
        switch (type)
        {
                case BP_TCYCLES:
                        upperLimit = 999999;
                        break;

                case BP_REGISTER:
                        if (Register16Bit(registerIndex))
                        {
                                upperLimit = 0xFFFF;
                        }
                        else
                        {
                                upperLimit = 0xFF;
                        }
                        break;

                case BP_FLAG:
                        upperLimit = 1;
                        break;

                case BP_EXE:
                        upperLimit = 0xFFFF;
                        break;

                case BP_IN:
                case BP_OUT:
                case BP_RD:
                case BP_WR:
                        if (condition == Range)
                        {
                                upperLimit = 0xFFFF;
                        }
                        else
                        {
                                upperLimit = 0xFF;
                        }
                        break;

                case BP_INL:
                case BP_INH:
                case BP_OUTH:
                case BP_OUTL:
                case BP_MEMORY:
                        upperLimit = 0xFF;
                        break;
        }
}
//---------------------------------------------------------------------------
void TSetBreakpoint::EnableOkButton()
{
        OK->Enabled = (BreakAddress->Font->Color == clWindowText) && (BreakValue->Font->Color == clWindowText);
}

//---------------------------------------------------------------------------
void __fastcall TSetBreakpoint::FormCreate(TObject *Sender)
{
        for (int i = 1; i <= 999; i++)
        {
                BreakHitCount->Items->Add(i);
        }

        BreakHitCount->ItemIndex = 0;
}
//---------------------------------------------------------------------------


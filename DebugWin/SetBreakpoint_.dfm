object SetBreakpoint: TSetBreakpoint
  Left = 475
  Top = 261
  BorderStyle = bsToolWindow
  Caption = 'Address Breakpoint'
  ClientHeight = 105
  ClientWidth = 179
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnKeyPress = FormKeyPress
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 12
    Width = 41
    Height = 13
    Caption = 'Address:'
  end
  object Label2: TLabel
    Left = 8
    Top = 44
    Width = 47
    Height = 13
    Caption = 'Condition:'
  end
  object OK: TButton
    Left = 8
    Top = 72
    Width = 75
    Height = 25
    Caption = 'OK'
    Default = True
    TabOrder = 0
    OnClick = OKClick
  end
  object Cancel: TButton
    Left = 96
    Top = 72
    Width = 75
    Height = 25
    Caption = 'Cancel'
    TabOrder = 1
    OnClick = CancelClick
  end
  object EditAddress: TEdit
    Left = 64
    Top = 8
    Width = 105
    Height = 21
    MaxLength = 5
    TabOrder = 2
    Text = '0'
  end
  object RadioButtonEqual: TRadioButton
    Left = 104
    Top = 43
    Width = 25
    Height = 17
    Caption = '='
    Checked = True
    TabOrder = 4
    TabStop = True
  end
  object RadioButtonLessThan: TRadioButton
    Left = 64
    Top = 43
    Width = 25
    Height = 17
    Caption = '<'
    TabOrder = 3
  end
  object RadioButtonGreaterThan: TRadioButton
    Left = 144
    Top = 43
    Width = 25
    Height = 17
    Caption = '>'
    TabOrder = 5
  end
end
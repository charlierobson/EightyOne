object LoadBasicListingOptionsForm: TLoadBasicListingOptionsForm
  Left = 194
  Top = 133
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Load BASIC Listing Options'
  ClientHeight = 146
  ClientWidth = 239
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010002002020100000000000E80200002600000010101000000000002801
    00000E0300002800000020000000400000000100040000000000800200000000
    0000000000000000000000000000000000000000800000800000008080008000
    0000800080008080000080808000C0C0C0000000FF0000FF000000FFFF00FF00
    0000FF00FF00FFFF0000FFFFFF00000000000000000000000000000000007777
    7777777777777777777777777777000000000000000000000000000000000000
    0000000000000000000000000000000070070070070070070070070070000000
    7007007007007007007007007000000000000000000000000000000000000000
    0700700700700700700700700700000007007007007007007007007007000000
    0000000000000000000000000000000070070070070070070070070070000000
    7007007007007007007007007000000000000000000000000000000000000007
    0070070070070070070070070000000700700700700700700700700700000000
    0000000000000000000000000000000000000000000000000000000000000777
    7777777777777777777777777770000000000000000000000000000000000099
    9919091999090000000000000000000900019109090900000000000000000000
    9001910191190000000000000000009999190919990900000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000770
    7077777777700000000000000000077777777070777700000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000FFFFFFFF0000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000FFFFFFFFFFFFFFFF280000001000000020000000010004000000
    0000C00000000000000000000000000000000000000000000000000080000080
    00000080800080000000800080008080000080808000C0C0C0000000FF0000FF
    000000FFFF00FF000000FF00FF00FFFF0000FFFFFF0077777777777777770000
    000000000000088878878878878000F7FF7FF7FF7FF00088788788788780F7FF
    7FF7FF7FF0000000000000000000077777777777777000000000000000000999
    0999900000000090909090000000099909999000000000000000000000000000
    0000000000007777777700000000000000000000000000000000000000000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000FFFF0000}
  OldCreateOrder = False
  Position = poScreenCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 8
    Width = 222
    Height = 13
    Caption = 'Configure how a BASIC listing file is interpreted:'
    WordWrap = True
  end
  object Label2: TLabel
    Left = 8
    Top = 32
    Width = 162
    Height = 13
    Caption = 'Attempt to tokenise REM contents'
  end
  object Label3: TLabel
    Left = 8
    Top = 56
    Width = 163
    Height = 13
    Caption = 'Attempt to tokenise string contents'
  end
  object Label4: TLabel
    Left = 8
    Top = 80
    Width = 124
    Height = 13
    Caption = 'Discard redundant spaces'
  end
  object cboTokeniseRemContents: TCheckBox
    Left = 200
    Top = 32
    Width = 25
    Height = 17
    Alignment = taLeftJustify
    TabOrder = 0
  end
  object cboTokeniseStringContents: TCheckBox
    Left = 192
    Top = 56
    Width = 33
    Height = 17
    Alignment = taLeftJustify
    TabOrder = 1
  end
  object btnOK: TButton
    Left = 84
    Top = 112
    Width = 68
    Height = 25
    Caption = 'OK'
    Default = True
    ModalResult = 1
    TabOrder = 3
  end
  object cboDiscardRedundantSpaces: TCheckBox
    Left = 200
    Top = 80
    Width = 25
    Height = 17
    Alignment = taLeftJustify
    TabOrder = 2
  end
end
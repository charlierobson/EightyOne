object HistoryBox: THistoryBox
  Left = 198
  Top = 115
  Width = 471
  Height = 431
  BorderStyle = bsSizeToolWin
  Caption = 'CPU History'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object Text: TRichEdit
    Left = 0
    Top = 32
    Width = 466
    Height = 345
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -13
    Font.Name = 'Courier New'
    Font.Style = []
    HideScrollBars = False
    Lines.Strings = (
      'Text')
    ParentFont = False
    PlainText = True
    ReadOnly = True
    ScrollBars = ssVertical
    TabOrder = 0
    WordWrap = False
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 374
    Width = 455
    Height = 19
    Panels = <>
    SimplePanel = False
  end
  object ToolBar1: TToolBar
    Left = 0
    Top = 0
    Width = 455
    Height = 25
    ButtonHeight = 21
    ButtonWidth = 48
    Caption = 'ToolBar1'
    ShowCaptions = True
    TabOrder = 2
    Wrapable = False
    object ToolButtonReload: TToolButton
      Left = 0
      Top = 2
      Caption = 'Reload'
      ImageIndex = 0
      OnClick = ToolButtonReloadClick
    end
    object ToolButton4: TToolButton
      Left = 48
      Top = 2
      Width = 8
      Caption = 'ToolButton4'
      ImageIndex = 2
      Style = tbsDivider
    end
    object ToolButtonClear: TToolButton
      Left = 56
      Top = 2
      Caption = 'Clear'
      ImageIndex = 1
      OnClick = ToolButtonClearClick
    end
    object ToolButton3: TToolButton
      Left = 104
      Top = 2
      Width = 8
      Caption = 'ToolButton3'
      ImageIndex = 2
      Style = tbsDivider
    end
    object ReverseToolButtonReverse: TToolButton
      Left = 112
      Top = 2
      Caption = 'Reverse'
      ImageIndex = 3
      Style = tbsCheck
      OnClick = ReverseToolButtonReverseClick
    end
    object ToolButton2: TToolButton
      Left = 160
      Top = 2
      Width = 8
      Caption = 'ToolButton2'
      ImageIndex = 4
      Style = tbsDivider
    end
    object ToolButton0K8K: TToolButton
      Left = 168
      Top = 2
      Caption = '0K-8K'
      Down = True
      ImageIndex = 5
      Style = tbsCheck
      OnClick = ToolButton0K8KClick
    end
    object ToolButton5: TToolButton
      Left = 216
      Top = 2
      Width = 8
      Caption = 'ToolButton5'
      ImageIndex = 6
      Style = tbsDivider
    end
    object ToolButton8K16K: TToolButton
      Left = 224
      Top = 2
      Caption = '8K-16K'
      Down = True
      ImageIndex = 7
      Style = tbsCheck
      OnClick = ToolButton8K16KClick
    end
    object ToolButton7: TToolButton
      Left = 272
      Top = 2
      Width = 8
      Caption = 'ToolButton7'
      ImageIndex = 8
      Style = tbsDivider
    end
    object ToolButton16K32K: TToolButton
      Left = 280
      Top = 2
      Caption = '16K-32K'
      Down = True
      ImageIndex = 9
      Style = tbsCheck
      OnClick = ToolButton16K32KClick
    end
    object ToolButton6: TToolButton
      Left = 328
      Top = 2
      Width = 8
      Caption = 'ToolButton6'
      ImageIndex = 10
      Style = tbsDivider
    end
    object ToolButton32K48K: TToolButton
      Left = 336
      Top = 2
      Caption = '32K-48K'
      Down = True
      ImageIndex = 11
      Style = tbsCheck
      OnClick = ToolButton32K48KClick
    end
    object ToolButton9: TToolButton
      Left = 384
      Top = 2
      Width = 8
      Caption = 'ToolButton9'
      ImageIndex = 12
      Style = tbsDivider
    end
    object ToolButton48K64K: TToolButton
      Left = 392
      Top = 2
      Caption = '48K-64K'
      Down = True
      ImageIndex = 13
      Style = tbsCheck
      OnClick = ToolButton48K64KClick
    end
    object ToolButton11: TToolButton
      Left = 440
      Top = 2
      Width = 8
      Caption = 'ToolButton11'
      ImageIndex = 14
      Style = tbsDivider
    end
  end
end

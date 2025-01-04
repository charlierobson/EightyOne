object ZXPrinter: TZXPrinter
  Left = 416
  Top = 115
  Anchors = [akLeft, akTop, akBottom]
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'ZX Printer'
  ClientHeight = 453
  ClientWidth = 273
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object DisplayImage: TImage
    Left = 0
    Top = 0
    Width = 256
    Height = 265
  end
  object ScrollBar: TScrollBar
    Left = 256
    Top = 0
    Width = 17
    Height = 265
    Anchors = [akTop, akRight]
    Kind = sbVertical
    PageSize = 0
    TabOrder = 0
    OnChange = ScrollBarChange
  end
  object SaveImage: TButton
    Left = 8
    Top = 272
    Width = 60
    Height = 25
    Caption = 'Save'
    TabOrder = 1
    OnClick = SaveImageClick
  end
  object ClearImage: TButton
    Left = 74
    Top = 272
    Width = 60
    Height = 25
    Caption = 'Clear'
    TabOrder = 2
    OnClick = ClearImageClick
  end
  object OptionsBtn: TButton
    Left = 208
    Top = 272
    Width = 60
    Height = 25
    Caption = 'Options'
    TabOrder = 4
    OnClick = OptionsBtnClick
  end
  object Panel1: TPanel
    Left = 8
    Top = 304
    Width = 257
    Height = 145
    TabOrder = 5
    object BitRateTxt: TLabel
      Left = 24
      Top = 5
      Width = 73
      Height = 13
      Caption = 'Carriage Speed'
    end
    object LineRateTxt: TLabel
      Left = 24
      Top = 61
      Width = 89
      Height = 13
      Caption = 'Paper Feed Speed'
    end
    object Label1: TLabel
      Left = 16
      Top = 121
      Width = 26
      Height = 13
      Caption = 'Style:'
    end
    object BitRate: TTrackBar
      Left = 8
      Top = 21
      Width = 241
      Height = 33
      Max = 100
      Orientation = trHorizontal
      Frequency = 5
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 0
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = BitRateChange
    end
    object LineRate: TTrackBar
      Left = 8
      Top = 77
      Width = 241
      Height = 33
      Max = 100
      Orientation = trHorizontal
      Frequency = 5
      Position = 0
      SelEnd = 0
      SelStart = 0
      TabOrder = 1
      TickMarks = tmBottomRight
      TickStyle = tsAuto
      OnChange = LineRateChange
    end
    object Style: TComboBox
      Left = 48
      Top = 117
      Width = 97
      Height = 21
      Style = csDropDownList
      ItemHeight = 13
      TabOrder = 2
      OnChange = StyleChange
      Items.Strings = (
        'ZX Printer'
        'Alphacom 32'
        'Plain Paper')
    end
  end
  object Feed: TButton
    Left = 141
    Top = 272
    Width = 60
    Height = 25
    Caption = 'Feed'
    TabOrder = 3
    OnClick = FeedClick
    OnMouseDown = FeedMouseDown
    OnMouseUp = FeedMouseUp
  end
  object SaveDialog: TSaveDialog
    DefaultExt = 'bmp'
    Filter = 'Windows Bitmap (BMP)|*.bmp'
    Left = 8
    Top = 8
  end
  object FeedTimer: TTimer
    Enabled = False
    Interval = 100
    OnTimer = FeedTimerExpired
    Left = 232
    Top = 416
  end
end

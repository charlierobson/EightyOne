object ProfilePlot: TProfilePlot
  Left = 170
  Top = 162
  Width = 643
  Height = 402
  BorderIcons = [biSystemMenu, biMaximize]
  Caption = 'Profile Plot'
  Color = clBtnFace
  Constraints.MinHeight = 148
  Constraints.MinWidth = 428
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnMouseMove = FormMouseMove
  OnPaint = FormPaint
  OnResize = FormResize
  PixelsPerInch = 96
  TextHeight = 13
  object ScrollBarHorizontal: TScrollBar
    Left = 0
    Top = 328
    Width = 627
    Height = 17
    Align = alBottom
    LargeChange = 100
    Max = 0
    PageSize = 0
    TabOrder = 0
    OnChange = ScrollBarHorizontalChange
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 345
    Width = 627
    Height = 19
    Panels = <
      item
        Text = 'Min: ---- Max: ----'
        Width = 200
      end
      item
        Text = 'Sample: ----'
        Width = 100
      end
      item
        Text = 'Current: ----'
        Width = 100
      end
      item
        Width = 50
      end>
    SimplePanel = False
  end
end

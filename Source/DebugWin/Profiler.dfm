object Profiler: TProfiler
  Left = 511
  Top = 217
  Width = 458
  Height = 189
  Caption = 'Profiler'
  Color = clBtnFace
  Constraints.MinHeight = 189
  Constraints.MinWidth = 458
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object ButtonNew: TButton
    Left = 4
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'New'
    TabOrder = 0
    OnClick = ButtonNewClick
  end
  object ButtonDelete: TButton
    Left = 196
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Delete'
    TabOrder = 1
    OnClick = ButtonDeleteClick
  end
  object ListViewProfileSamples: TListView
    Left = 8
    Top = 8
    Width = 424
    Height = 107
    Anchors = [akLeft, akTop, akRight, akBottom]
    Columns = <
      item
        Caption = 'Tag'
        Width = 100
      end
      item
        Caption = 'Start'
      end
      item
        Caption = 'End'
      end
      item
        Caption = 'Min T'
        Width = 60
      end
      item
        Caption = 'Max T'
        Width = 60
      end
      item
        Caption = 'Samples'
        Width = 100
      end>
    ColumnClick = False
    GridLines = True
    RowSelect = True
    TabOrder = 2
    ViewStyle = vsReport
  end
  object ButtonEdit: TButton
    Left = 68
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Edit'
    TabOrder = 3
    OnClick = ButtonEditClick
  end
  object ButtonReset: TButton
    Left = 132
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akLeft, akBottom]
    Caption = 'Reset'
    TabOrder = 4
    OnClick = ButtonResetClick
  end
  object ButtonRefresh: TButton
    Left = 312
    Top = 122
    Width = 53
    Height = 25
    Anchors = [akRight, akBottom]
    Caption = 'Refresh'
    TabOrder = 5
    OnClick = ButtonRefreshClick
  end
  object ButtonPlot: TButton
    Left = 376
    Top = 122
    Width = 53
    Height = 25
    Caption = 'Plot'
    TabOrder = 6
    OnClick = ButtonPlotClick
  end
end
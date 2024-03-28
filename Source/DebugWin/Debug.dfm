object Dbg: TDbg
  Left = 197
  Top = 116
  BorderIcons = [biSystemMenu]
  BorderStyle = bsToolWindow
  Caption = 'Debug Window'
  ClientHeight = 468
  ClientWidth = 626
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  KeyPreview = True
  OldCreateOrder = False
  Scaled = False
  OnClose = FormClose
  OnKeyDown = FormKeyDown
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object GroupBox1: TGroupBox
    Left = 4
    Top = 3
    Width = 305
    Height = 137
    Caption = 'Z80'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Pitch = fpFixed
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    object Label1: TLabel
      Left = 11
      Top = 48
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL:'
    end
    object Label2: TLabel
      Left = 11
      Top = 16
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'BC:'
    end
    object Label3: TLabel
      Left = 11
      Top = 32
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'DE:'
    end
    object Label4: TLabel
      Left = 11
      Top = 64
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'A:'
    end
    object Label5: TLabel
      Left = 11
      Top = 80
      Width = 25
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'F:'
    end
    object HL: TLabel
      Left = 40
      Top = 48
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = HLClick
      OnMouseDown = HLMouseDown
    end
    object BC: TLabel
      Left = 40
      Top = 16
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
      ParentShowHint = False
      PopupMenu = MemDumpPopup
      ShowHint = False
      OnClick = BCClick
      OnMouseDown = BCMouseDown
    end
    object DE: TLabel
      Left = 40
      Top = 32
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = DEClick
      OnMouseDown = DEMouseDown
    end
    object A: TLabel
      Left = 40
      Top = 64
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = AClick
    end
    object F: TLabel
      Left = 40
      Top = 80
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = FClick
    end
    object Label6: TLabel
      Left = 118
      Top = 48
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'HL'#39':'
    end
    object HL_: TLabel
      Left = 152
      Top = 48
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = HL_Click
      OnMouseDown = HL_MouseDown
    end
    object BC_: TLabel
      Left = 152
      Top = 16
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = BC_Click
      OnMouseDown = BC_MouseDown
    end
    object DE_: TLabel
      Left = 152
      Top = 32
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = DE_Click
      OnMouseDown = DE_MouseDown
    end
    object A_: TLabel
      Left = 152
      Top = 64
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = A_Click
    end
    object F_: TLabel
      Left = 152
      Top = 80
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = F_Click
    end
    object Label12: TLabel
      Left = 118
      Top = 80
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'F'#39':'
    end
    object Label13: TLabel
      Left = 118
      Top = 64
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'A'#39':'
    end
    object Label14: TLabel
      Left = 118
      Top = 32
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'DE'#39':'
    end
    object Label15: TLabel
      Left = 118
      Top = 16
      Width = 30
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'BC'#39':'
    end
    object Label16: TLabel
      Left = 232
      Top = 16
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IX:'
    end
    object IX: TLabel
      Left = 256
      Top = 16
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = IXClick
      OnMouseDown = IXMouseDown
    end
    object Label18: TLabel
      Left = 232
      Top = 32
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IY:'
    end
    object IY: TLabel
      Left = 256
      Top = 32
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = IYClick
      OnMouseDown = IYMouseDown
    end
    object Label20: TLabel
      Left = 232
      Top = 48
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'IR:'
    end
    object IR: TLabel
      Left = 256
      Top = 48
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = IRClick
      OnMouseDown = IRMouseDown
    end
    object Label22: TLabel
      Left = 232
      Top = 64
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'SP:'
    end
    object SP: TLabel
      Left = 256
      Top = 64
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = SPClick
      OnMouseDown = SPMouseDown
    end
    object Label24: TLabel
      Left = 232
      Top = 80
      Width = 20
      Height = 13
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'PC:'
    end
    object PC: TLabel
      Left = 256
      Top = 80
      Width = 40
      Height = 13
      AutoSize = False
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = PCClick
      OnMouseDown = PCMouseDown
    end
    object Label7: TLabel
      Left = 258
      Top = 112
      Width = 23
      Height = 13
      AutoSize = False
      Caption = 'IM:'
    end
    object IM: TLabel
      Left = 284
      Top = 112
      Width = 9
      Height = 13
      AutoSize = False
      Caption = '0'
      OnClick = IMClick
    end
    object Label9: TLabel
      Left = 96
      Top = 112
      Width = 73
      Height = 13
      AutoSize = False
      Caption = 'Interrupts:'
    end
    object Interrupts: TLabel
      Left = 178
      Top = 112
      Width = 56
      Height = 14
      Caption = 'Disabled'
      OnClick = InterruptsClick
    end
    object Label11: TLabel
      Left = 11
      Top = 112
      Width = 37
      Height = 13
      AutoSize = False
      Caption = 'Halt:'
    end
    object Halt: TLabel
      Left = 51
      Top = 112
      Width = 25
      Height = 13
      AutoSize = False
      Caption = 'Yes'
    end
    object Label28: TLabel
      Left = 40
      Top = 94
      Width = 56
      Height = 14
      Caption = 'SZ-H-VNC'
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Label29: TLabel
      Left = 152
      Top = 94
      Width = 56
      Height = 14
      Caption = 'SZ-H-VNC'
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBox2: TGroupBox
    Left = 4
    Top = 227
    Width = 367
    Height = 238
    Caption = 'Program'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 5
    object Disass0: TLabel
      Left = 8
      Top = 37
      Width = 301
      Height = 14
      Caption = 'LABEL000    $0000  xxxxxxxx  LD HL,(IX+$zz)'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
      ShowAccelChar = False
    end
    object Disass1: TLabel
      Left = 8
      Top = 53
      Width = 70
      Height = 14
      Caption = '<history2>'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
      ShowAccelChar = False
    end
    object Disass2: TLabel
      Left = 8
      Top = 69
      Width = 70
      Height = 14
      Caption = '<history3>'
      Color = clBtnFace
      Font.Charset = ANSI_CHARSET
      Font.Color = clGrayText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
      ShowAccelChar = False
    end
    object Disass3: TLabel
      Left = 8
      Top = 85
      Width = 350
      Height = 14
      Caption = '<currentPC> 01234567890123456789012345678901234567'
      Color = clHighlightText
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentColor = False
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Disass4: TLabel
      Left = 8
      Top = 101
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Disass5: TLabel
      Left = 8
      Top = 117
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Disass6: TLabel
      Left = 8
      Top = 133
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Disass7: TLabel
      Left = 8
      Top = 149
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Disass8: TLabel
      Left = 8
      Top = 165
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Disass9: TLabel
      Left = 8
      Top = 181
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Label8: TLabel
      Left = 8
      Top = 21
      Width = 259
      Height = 14
      Caption = 'Label       Addr   Code      Mnemonic'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
    end
    object Disass10: TLabel
      Left = 8
      Top = 197
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
    object Disass11: TLabel
      Left = 8
      Top = 213
      Width = 7
      Height = 14
      Caption = '"'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      ShowAccelChar = False
      OnMouseDown = Disass3MouseDown
    end
  end
  object GroupBoxZX81: TGroupBox
    Left = 378
    Top = 346
    Width = 243
    Height = 119
    Caption = 'Display Generation'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 10
    object Label39: TLabel
      Left = 37
      Top = 18
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'NMI Generator:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label10: TLabel
      Left = 37
      Top = 34
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Sync Output:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label19: TLabel
      Left = 37
      Top = 50
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Scanline:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label21: TLabel
      Left = 37
      Top = 66
      Width = 105
      Height = 14
      Alignment = taRightJustify
      AutoSize = False
      Caption = 'Line Counter:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label23: TLabel
      Left = 37
      Top = 82
      Width = 105
      Height = 14
      Alignment = taRightJustify
      Caption = 'Shift Register:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object NMIGen: TLabel
      Left = 147
      Top = 18
      Width = 57
      Height = 14
      AutoSize = False
      Caption = 'OFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = NMIGenClick
    end
    object Scanline: TLabel
      Left = 147
      Top = 50
      Width = 57
      Height = 14
      AutoSize = False
      Caption = '299'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object RowCount: TLabel
      Left = 147
      Top = 66
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '7'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object ShiftReg: TLabel
      Left = 147
      Top = 82
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object SyncOutput: TLabel
      Left = 147
      Top = 34
      Width = 57
      Height = 14
      AutoSize = False
      Caption = 'White'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = SyncOutputClick
    end
    object Label27: TLabel
      Left = 79
      Top = 98
      Width = 63
      Height = 14
      Alignment = taRightJustify
      Caption = 'T-Cycles:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object TStates: TLabel
      Left = 147
      Top = 98
      Width = 57
      Height = 13
      AutoSize = False
      Caption = '00000000'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBox4: TGroupBox
    Left = 378
    Top = 3
    Width = 243
    Height = 222
    Caption = 'Breakpoints'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 2
    object Label31: TLabel
      Left = 9
      Top = 16
      Width = 105
      Height = 14
      Caption = 'T-Cycles Count:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object TStatesCount: TLabel
      Left = 120
      Top = 16
      Width = 57
      Height = 14
      AutoSize = False
      Caption = '0'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = HL_Click
      OnMouseDown = HL_MouseDown
    end
    object AddBrkBtn: TButton
      Left = 8
      Top = 189
      Width = 53
      Height = 25
      Caption = 'Add'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = AddBrkBtnClick
    end
    object DelBrkBtn: TButton
      Left = 66
      Top = 189
      Width = 53
      Height = 25
      Caption = 'Delete'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 1
      OnClick = DelBrkBtnClick
    end
    object BPList: TStringGrid
      Left = 7
      Top = 36
      Width = 228
      Height = 145
      ColCount = 1
      DefaultColWidth = 10
      DefaultRowHeight = 14
      FixedCols = 0
      RowCount = 1
      FixedRows = 0
      Options = [goFixedVertLine, goFixedHorzLine, goVertLine, goRowSelect]
      PopupMenu = BreakpointWindowPopup
      ScrollBars = ssVertical
      TabOrder = 4
      OnContextPopup = BPListContextPopup
      OnDblClick = BPListDblClick
      OnDrawCell = BPListDrawCell
      OnSelectCell = BPListSelectCell
    end
    object EditBrkBtn: TButton
      Left = 124
      Top = 189
      Width = 53
      Height = 25
      Caption = 'Edit'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnClick = EditBrkBtnClick
    end
    object ResetBrkBtn: TButton
      Left = 182
      Top = 189
      Width = 53
      Height = 25
      Caption = 'Reset'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 3
      OnClick = ResetBrkBtnClick
    end
  end
  object GroupBox5: TGroupBox
    Left = 314
    Top = 3
    Width = 57
    Height = 137
    Caption = 'Stack'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    object Stack0: TLabel
      Left = 11
      Top = 16
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack1: TLabel
      Left = 11
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack2: TLabel
      Left = 11
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack3: TLabel
      Left = 11
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack4: TLabel
      Left = 11
      Top = 80
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack5: TLabel
      Left = 11
      Top = 96
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
    object Stack6: TLabel
      Left = 11
      Top = 112
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      PopupMenu = MemDumpPopup
      OnClick = Stack0Click
      OnMouseDown = Stack0MouseDown
    end
  end
  object GroupBoxAce: TGroupBox
    Left = 378
    Top = 307
    Width = 243
    Height = 158
    Caption = 'Jupiter Ace Forth Stack'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 7
    object AceStk0: TLabel
      Left = 75
      Top = 20
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk1: TLabel
      Left = 75
      Top = 36
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk2: TLabel
      Left = 75
      Top = 52
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk3: TLabel
      Left = 75
      Top = 68
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk4: TLabel
      Left = 75
      Top = 84
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk5: TLabel
      Left = 75
      Top = 100
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk6: TLabel
      Left = 75
      Top = 116
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object AceStk7: TLabel
      Left = 75
      Top = 132
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStk0Click
    end
    object Label37: TLabel
      Left = 115
      Top = 20
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = Stack0Click
    end
    object Label38: TLabel
      Left = 115
      Top = 36
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label40: TLabel
      Left = 115
      Top = 52
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label41: TLabel
      Left = 115
      Top = 68
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label42: TLabel
      Left = 115
      Top = 84
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label43: TLabel
      Left = 115
      Top = 100
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label44: TLabel
      Left = 115
      Top = 116
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object Label45: TLabel
      Left = 115
      Top = 132
      Width = 7
      Height = 14
      Caption = ':'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object AceStkVal0: TLabel
      Left = 131
      Top = 20
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal1: TLabel
      Left = 131
      Top = 36
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal2: TLabel
      Left = 131
      Top = 52
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal3: TLabel
      Left = 131
      Top = 68
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal4: TLabel
      Left = 131
      Top = 84
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal5: TLabel
      Left = 131
      Top = 100
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal6: TLabel
      Left = 131
      Top = 116
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
    object AceStkVal7: TLabel
      Left = 131
      Top = 132
      Width = 35
      Height = 14
      Caption = '$FFFF'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
      OnClick = AceStkVal0Click
    end
  end
  object Execute: TGroupBox
    Left = 4
    Top = 141
    Width = 238
    Height = 84
    Caption = 'Control'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 3
    object SingleStep: TButton
      Left = 10
      Top = 21
      Width = 50
      Height = 25
      Hint = 'Shift+F5'
      Caption = 'Step'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 0
      OnClick = SingleStepClick
    end
    object StepOver: TButton
      Left = 66
      Top = 21
      Width = 50
      Height = 25
      Hint = 'Shift+F6'
      Caption = 'Over'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
      OnClick = StepOverClick
    end
    object RunStop: TButton
      Left = 178
      Top = 21
      Width = 50
      Height = 25
      Hint = 'Shift+F8'
      Caption = 'Stop'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 3
      OnClick = RunStopClick
    end
    object Continuous: TCheckBox
      Left = 155
      Top = 56
      Width = 73
      Height = 17
      Caption = 'Continuous'
      Enabled = False
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 6
      OnClick = ContinuousClick
    end
    object SkipNMIBtn: TCheckBox
      Left = 10
      Top = 56
      Width = 65
      Height = 17
      Caption = 'Skip NMI'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 4
      OnClick = SkipNMIBtnClick
    end
    object SkipINTBtn: TCheckBox
      Left = 84
      Top = 56
      Width = 65
      Height = 17
      Caption = 'Skip INT'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Pitch = fpFixed
      Font.Style = []
      ParentFont = False
      TabOrder = 5
      OnClick = SkipINTBtnClick
    end
    object StepOut: TButton
      Left = 122
      Top = 21
      Width = 50
      Height = 25
      Hint = 'Shift+F7'
      Caption = 'Out'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 2
      OnClick = StepOutClick
    end
  end
  object GroupBox3: TGroupBox
    Left = 378
    Top = 227
    Width = 243
    Height = 78
    Caption = 'View'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 6
    object SymRom: TLabel
      Left = 126
      Top = 53
      Width = 25
      Height = 13
      Hint = 'Indicates whether ROM symbols are loaded'
      Caption = 'ROM'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
    end
    object SymApp: TLabel
      Left = 155
      Top = 53
      Width = 21
      Height = 13
      Hint = 'Indicates whether application symbols are loaded'
      Caption = 'APP'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
    end
    object EnableHistory: TCheckBox
      Left = 9
      Top = 51
      Width = 57
      Height = 17
      Hint = 'Enable history capture'
      Caption = 'Enable'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 1
    end
    object History: TButton
      Left = 8
      Top = 17
      Width = 53
      Height = 25
      Caption = 'History'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 0
      OnClick = HistoryClick
    end
    object Memory: TButton
      Left = 66
      Top = 17
      Width = 53
      Height = 25
      Caption = 'Memory'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 2
      OnClick = MemoryClick
    end
    object Symbols: TButton
      Left = 124
      Top = 17
      Width = 53
      Height = 25
      Caption = 'Symbols'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 3
      OnClick = SymbolsClick
    end
    object ButtonProfiler: TButton
      Left = 182
      Top = 17
      Width = 53
      Height = 25
      Caption = 'Profiler'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      TabOrder = 4
      OnClick = ButtonProfilerClick
    end
    object AutoUpdateMemory: TCheckBox
      Left = 66
      Top = 51
      Width = 55
      Height = 17
      Hint = 'Enable Memory window refreshing'
      Caption = 'Refresh'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'MS Sans Serif'
      Font.Style = []
      ParentFont = False
      ParentShowHint = False
      ShowHint = True
      TabOrder = 5
      OnClick = AutoUpdateMemoryClick
    end
  end
  object IOPorts: TGroupBox
    Left = 249
    Top = 141
    Width = 122
    Height = 84
    Caption = 'I/O'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 4
    object IOPort0Address: TLabel
      Left = 38
      Top = 16
      Width = 35
      Height = 14
      Caption = '$FFFF'
      PopupMenu = InputContextPopup
      OnMouseDown = IOPort0AddressMouseDown
    end
    object IOPort2Address: TLabel
      Left = 39
      Top = 48
      Width = 35
      Height = 14
      Caption = '$FFFF'
      PopupMenu = InputContextPopup
      OnMouseDown = IOPort2AddressMouseDown
    end
    object IOPort1Address: TLabel
      Left = 39
      Top = 32
      Width = 35
      Height = 14
      Caption = '$FFFF'
      PopupMenu = InputContextPopup
      OnMouseDown = IOPort1AddressMouseDown
    end
    object IOPort3Address: TLabel
      Left = 39
      Top = 64
      Width = 35
      Height = 14
      Caption = '$FFFF'
      PopupMenu = InputContextPopup
      OnMouseDown = IOPort3AddressMouseDown
    end
    object IOPort0Data: TLabel
      Left = 90
      Top = 16
      Width = 21
      Height = 14
      Caption = '$FF'
    end
    object IOPort0Direction: TLabel
      Left = 10
      Top = 16
      Width = 21
      Height = 14
      Caption = 'OUT'
    end
    object IOPort2Data: TLabel
      Left = 90
      Top = 48
      Width = 21
      Height = 14
      Caption = '$FF'
    end
    object IOPort1Data: TLabel
      Left = 90
      Top = 32
      Width = 21
      Height = 14
      Caption = '$FF'
    end
    object IOPort3Data: TLabel
      Left = 90
      Top = 64
      Width = 21
      Height = 14
      Caption = '$FF'
    end
    object IOPort3Direction: TLabel
      Left = 10
      Top = 64
      Width = 21
      Height = 14
      Caption = 'OUT'
    end
    object IOPort1Direction: TLabel
      Left = 10
      Top = 32
      Width = 21
      Height = 14
      Caption = 'OUT'
    end
    object IOPort2Direction: TLabel
      Left = 10
      Top = 48
      Width = 21
      Height = 14
      Caption = 'OUT'
    end
    object Label17: TLabel
      Left = 78
      Top = 16
      Width = 7
      Height = 14
      Caption = ':'
    end
    object Label25: TLabel
      Left = 78
      Top = 32
      Width = 7
      Height = 14
      Caption = ':'
    end
    object Label26: TLabel
      Left = 78
      Top = 48
      Width = 7
      Height = 14
      Caption = ':'
    end
    object Label30: TLabel
      Left = 78
      Top = 64
      Width = 7
      Height = 14
      Caption = ':'
    end
  end
  object GroupBoxChroma: TGroupBox
    Left = 378
    Top = 307
    Width = 119
    Height = 36
    Caption = 'Chroma Colour'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 11
    object ChromaColourModeLabel: TLabel
      Left = 30
      Top = 16
      Width = 35
      Height = 14
      Alignment = taRightJustify
      Caption = 'Mode:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object ChromaColourMode: TLabel
      Left = 70
      Top = 16
      Width = 21
      Height = 14
      Caption = '$00'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
  end
  object GroupBoxZXC: TGroupBox
    Left = 502
    Top = 307
    Width = 119
    Height = 36
    Caption = 'ZXC Cartridge'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 9
    object ZXCModeLabel: TLabel
      Left = 30
      Top = 16
      Width = 35
      Height = 14
      Alignment = taRightJustify
      Caption = 'Mode:'
      Font.Charset = ANSI_CHARSET
      Font.Color = clWindowText
      Font.Height = -11
      Font.Name = 'Courier New'
      Font.Style = []
      ParentFont = False
    end
    object ZXCMode: TLabel
      Left = 70
      Top = 16
      Width = 21
      Height = 14
      Caption = '$00'
    end
  end
  object GroupBoxSpectra: TGroupBox
    Left = 378
    Top = 307
    Width = 119
    Height = 36
    Caption = 'Spectra Colour'
    Font.Charset = ANSI_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Courier New'
    Font.Style = []
    ParentFont = False
    TabOrder = 8
    object SpectraModeLabel: TLabel
      Left = 30
      Top = 16
      Width = 35
      Height = 14
      Caption = 'Mode:'
    end
    object SpectraMode: TLabel
      Left = 70
      Top = 16
      Width = 21
      Height = 14
      Caption = '$00'
    end
  end
  object MemDumpPopup: TPopupMenu
    Left = 202
    Top = 216
    object MemDumpFromHere1: TMenuItem
      Caption = 'Show Memory'
      OnClick = MemDumpFromHere1Click
    end
    object OnExecute1: TMenuItem
      Caption = 'Break On Execute'
      OnClick = AddBreak1Click
    end
    object OnRead1: TMenuItem
      Tag = 1
      Caption = 'Break On Read'
      OnClick = AddBreak1Click
    end
    object OnWrite1: TMenuItem
      Tag = 2
      Caption = 'Break On Write'
      OnClick = AddBreak1Click
    end
  end
  object InputContextPopup: TPopupMenu
    Left = 232
    Top = 216
    object BreakOnInput: TMenuItem
      Tag = 6
      Caption = 'Break on Input A15-0'
      OnClick = BreakonInputOutputClick
    end
    object BreakOnInputHigh: TMenuItem
      Tag = 7
      Caption = 'Break on Input A15-8'
      OnClick = BreakonInputOutputHighClick
    end
    object BreakOnInputLow: TMenuItem
      Tag = 8
      Caption = 'Break on Input A7-0'
      OnClick = BreakonInputOutputLowClick
    end
  end
  object OutputContextPopup: TPopupMenu
    Left = 264
    Top = 216
    object BreakOnOutput: TMenuItem
      Tag = 3
      Caption = 'Break on Output A15-0'
      OnClick = BreakonInputOutputClick
    end
    object BreakOnOutputHigh: TMenuItem
      Tag = 4
      Caption = 'Break on Output A15-8'
      OnClick = BreakonInputOutputHighClick
    end
    object BreakOnOutputLow: TMenuItem
      Tag = 5
      Caption = 'Break on Output A7-0'
      OnClick = BreakonInputOutputLowClick
    end
  end
  object BreakpointWindowPopup: TPopupMenu
    Left = 295
    Top = 216
    object Disable: TMenuItem
      Caption = 'Disable'
      OnClick = DisableClick
    end
    object Enable: TMenuItem
      Caption = 'Enable'
      OnClick = EnableClick
    end
    object ResetHitCount: TMenuItem
      Caption = 'Reset Hit Count'
      OnClick = ResetHitCountClick
    end
    object N1: TMenuItem
      Caption = '-'
    end
    object CurrentHitCount: TMenuItem
      Caption = 'Hit Count = x'
    end
  end
  object MemoryWindowTimer: TTimer
    Enabled = False
    Interval = 50
    OnTimer = MemoryWindowTimerExpired
    Left = 326
    Top = 216
  end
end

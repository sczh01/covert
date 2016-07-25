object MainForm: TMainForm
  Left = 0
  Top = 0
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = #26684#24335#36716#25442#22120
  ClientHeight = 693
  ClientWidth = 1244
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  Position = poDesigned
  OnClose = FormClose
  PixelsPerInch = 96
  TextHeight = 13
  object rdoInputType: TRadioGroup
    Left = 8
    Top = 386
    Width = 473
    Height = 299
    Caption = #25351#20196#31867#22411'(input)'
    Columns = 2
    ItemIndex = 0
    Items.Strings = (
      'COM(a); DATA(b);'
      '{setc, a}'
      'A1(CMD,P0,P1,..Pn)'
      'A2 CMD P0 P1..Pn'
      'A3 ... CMD ... P0..Pn'
      'A4..DT..cnt..CMD..P0..Pn'
      'CMD 0x70xx DATA 0x72yy'
      'GSWAP ==>  GSWAP'
      'GSWAP ==> GSWAP0  GSWAP1  '
      'A5..DT..CMD..cnt..P0..Pn'
      'Execel array'
      'CIE_color_generate'
      'Line generate'
      '0xmm ..'
      'Correct Gamma'
      'A6..DT..CMD..cnt..P0..Pn'
      'DSO File-->Test Log'
      'DSO File Analzye'
      'A7..DT..CMD..cnt..P0..Pn'
      'Excel Format')
    TabOrder = 1
    OnClick = rdoInputTypeClick
  end
  object rdoOutputType: TRadioGroup
    Left = 960
    Top = 391
    Width = 274
    Height = 294
    Caption = #25351#20196#31867#22411'(output)'
    ItemIndex = 0
    Items.Strings = (
      'COM(a); DATA(b);'
      '{setc, a}'
      'CMD Array1(CMD,P0,P1,..Pn)'
      'CMD Array2 CMD P0 P1..Pn'
      'CMD Array3 ... CMD ... P0..Pn'
      'CMD Array4 ..DT.. cnt.. CMD..P0..Pn'
      'GSWAP'
      'GSWAP0 GSWAP1 GSWAP2..')
    TabOrder = 10
  end
  object LeCMD1: TLabeledEdit
    Left = 523
    Top = 390
    Width = 76
    Height = 21
    EditLabel.Width = 22
    EditLabel.Height = 13
    EditLabel.Caption = 'CMD'
    LabelPosition = lpLeft
    TabOrder = 2
    Text = 'W_COM'
  end
  object LeDATA1: TLabeledEdit
    Left = 523
    Top = 411
    Width = 76
    Height = 21
    EditLabel.Width = 27
    EditLabel.Height = 13
    EditLabel.Caption = 'DATA'
    LabelPosition = lpLeft
    TabOrder = 3
    Text = 'W_DATA'
  end
  object LeDELAY1: TLabeledEdit
    Left = 523
    Top = 431
    Width = 76
    Height = 21
    EditLabel.Width = 31
    EditLabel.Height = 13
    EditLabel.Caption = 'DELAY'
    LabelPosition = lpLeft
    TabOrder = 4
    Text = 'Delayms'
  end
  object LeCMD2: TLabeledEdit
    Left = 648
    Top = 391
    Width = 80
    Height = 21
    EditLabel.Width = 22
    EditLabel.Height = 13
    EditLabel.Caption = 'CMD'
    LabelPosition = lpRight
    TabOrder = 7
    Text = 'W_COM'
  end
  object LeDATA2: TLabeledEdit
    Left = 648
    Top = 412
    Width = 80
    Height = 21
    EditLabel.Width = 27
    EditLabel.Height = 13
    EditLabel.Caption = 'DATA'
    LabelPosition = lpRight
    TabOrder = 8
    Text = 'W_DATA'
  end
  object LeDELAY2: TLabeledEdit
    Left = 648
    Top = 432
    Width = 80
    Height = 21
    EditLabel.Width = 31
    EditLabel.Height = 13
    EditLabel.Caption = 'DELAY'
    LabelPosition = lpRight
    TabOrder = 9
    Text = 'Delayms'
  end
  object GroupBox1: TGroupBox
    Left = 8
    Top = 8
    Width = 591
    Height = 377
    Caption = 'Input'
    TabOrder = 0
    object Memo1: TMemo
      Left = 2
      Top = 15
      Width = 587
      Height = 360
      Align = alClient
      ScrollBars = ssBoth
      TabOrder = 0
      OnClick = Memo1Click
    end
  end
  object BtnConvert1: TButton
    Left = 599
    Top = 390
    Width = 49
    Height = 21
    Caption = '-->>'
    TabOrder = 5
    OnClick = BtnConvert1Click
  end
  object GroupBox2: TGroupBox
    Left = 603
    Top = 8
    Width = 633
    Height = 377
    Caption = 'Output'
    TabOrder = 12
    object Memo2: TMemo
      Left = 2
      Top = 15
      Width = 629
      Height = 360
      Align = alClient
      ScrollBars = ssBoth
      TabOrder = 0
    end
  end
  object BtnCopy2: TButton
    Left = 599
    Top = 411
    Width = 49
    Height = 20
    Caption = #22797#21046
    TabOrder = 6
    OnClick = BtnCopy2Click
  end
  object LinkLabel1: TLinkLabel
    Left = 429
    Top = 637
    Width = 96
    Height = 17
    Caption = '<a>'#28857#27492#26597#30475'DCS'#21629#20196'</a>'
    TabOrder = 11
    Visible = False
    OnClick = LinkLabel1Click
  end
  object if_out: TRadioGroup
    Left = 863
    Top = 390
    Width = 91
    Height = 113
    Caption = 'Output bit'
    ItemIndex = 0
    Items.Strings = (
      '8(Dec)'
      '16(Hex)'
      '24(Hex)'
      '32(Hex)')
    TabOrder = 13
  end
  object if_in: TRadioGroup
    Left = 783
    Top = 390
    Width = 82
    Height = 114
    Caption = 'input  bit'
    ItemIndex = 0
    Items.Strings = (
      '8(Dec)'
      '16(Hex)'
      '24(Hex)'
      '32(Hex)')
    TabOrder = 14
  end
  object if_type: TRadioGroup
    Left = 487
    Top = 460
    Width = 130
    Height = 206
    Caption = 'Convert Type'
    ItemIndex = 0
    Items.Strings = (
      'Normal Convert'
      'Self Convert'
      'GSWAP=>'
      'GSWAP<='
      '0xmm...')
    TabOrder = 15
  end
  object LeArrayHead: TLabeledEdit
    Left = 760
    Top = 508
    Width = 23
    Height = 21
    EditLabel.Width = 58
    EditLabel.Height = 13
    EditLabel.Caption = 'Array_Head'
    LabelPosition = lpLeft
    TabOrder = 16
    Text = ' '
  end
  object LeArrayTrail: TLabeledEdit
    Left = 760
    Top = 530
    Width = 23
    Height = 21
    EditLabel.Width = 53
    EditLabel.Height = 13
    EditLabel.Caption = 'Array_Trail'
    LabelPosition = lpLeft
    TabOrder = 17
    Text = ' '
  end
  object LeArrayGap: TLabeledEdit
    Left = 760
    Top = 552
    Width = 23
    Height = 21
    EditLabel.Width = 52
    EditLabel.Height = 13
    EditLabel.Caption = 'Array_Gap'
    LabelPosition = lpLeft
    TabOrder = 18
    Text = ' '
  end
  object LeCmdPos: TLabeledEdit
    Left = 672
    Top = 508
    Width = 25
    Height = 21
    EditLabel.Width = 45
    EditLabel.Height = 13
    EditLabel.Caption = 'CMD_Pos'
    LabelPosition = lpLeft
    TabOrder = 19
    Text = ' '
  end
  object LeDataPos: TLabeledEdit
    Left = 672
    Top = 530
    Width = 25
    Height = 21
    EditLabel.Width = 47
    EditLabel.Height = 13
    EditLabel.Caption = 'DATA Pos'
    LabelPosition = lpLeft
    TabOrder = 20
    Text = ' '
  end
  object LeDelayPos: TLabeledEdit
    Left = 672
    Top = 552
    Width = 25
    Height = 21
    EditLabel.Width = 47
    EditLabel.Height = 13
    EditLabel.Caption = 'Delay Pos'
    LabelPosition = lpLeft
    TabOrder = 21
    Text = ' '
  end
  object LeDataCntPos: TLabeledEdit
    Left = 760
    Top = 596
    Width = 23
    Height = 21
    EditLabel.Width = 50
    EditLabel.Height = 13
    EditLabel.Caption = 'D_Cnt Pos'
    LabelPosition = lpLeft
    TabOrder = 22
    Text = ' '
  end
  object LeDataTypePos: TLabeledEdit
    Left = 760
    Top = 618
    Width = 24
    Height = 21
    EditLabel.Width = 57
    EditLabel.Height = 13
    EditLabel.Caption = 'D_Type Pos'
    LabelPosition = lpLeft
    TabOrder = 23
    Text = ' '
  end
  object LeAnnotate: TLabeledEdit
    Left = 672
    Top = 574
    Width = 25
    Height = 21
    EditLabel.Width = 44
    EditLabel.Height = 13
    EditLabel.Caption = 'annotate'
    LabelPosition = lpLeft
    TabOrder = 24
    Text = ' '
  end
  object LeExFunc: TLabeledEdit
    Left = 672
    Top = 596
    Width = 25
    Height = 21
    EditLabel.Width = 26
    EditLabel.Height = 13
    EditLabel.Caption = 'other'
    LabelPosition = lpLeft
    TabOrder = 25
    Text = ' 0'
  end
  object btnSave: TButton
    Left = 599
    Top = 432
    Width = 49
    Height = 22
    Caption = 'Save'
    TabOrder = 26
    OnClick = btnSaveClick
  end
  object MIPI_Data_Type: TCheckBox
    Left = 648
    Top = 459
    Width = 97
    Height = 17
    Caption = 'DCS_GCS_Switch'
    TabOrder = 27
  end
  object Edit1: TEdit
    Left = 785
    Top = 507
    Width = 50
    Height = 21
    TabOrder = 28
    Text = '0.7347'
  end
  object Edit2: TEdit
    Left = 838
    Top = 507
    Width = 50
    Height = 21
    TabOrder = 29
    Text = '0.2653'
  end
  object Edit3: TEdit
    Left = 785
    Top = 531
    Width = 50
    Height = 21
    TabOrder = 30
    Text = '0.1741'
  end
  object Edit4: TEdit
    Left = 838
    Top = 531
    Width = 50
    Height = 21
    TabOrder = 31
    Text = '0.005'
  end
  object Edit5: TEdit
    Left = 785
    Top = 553
    Width = 50
    Height = 21
    TabOrder = 32
    Text = '40'
  end
  object Edit6: TEdit
    Left = 838
    Top = 553
    Width = 50
    Height = 21
    TabOrder = 33
    Text = '41'
  end
  object AllGCS: TCheckBox
    Left = 648
    Top = 477
    Width = 58
    Height = 17
    Caption = 'All GCS'
    TabOrder = 34
  end
  object LinkLabel_Other: TLinkLabel
    Left = 519
    Top = 672
    Width = 104
    Height = 17
    Caption = '<a>'#28857#27492#26597#30475'Other'#20869#23481'</a>'
    TabOrder = 35
    OnClick = LinkLabel1Click
  end
  object LeArrayGapOut: TLabeledEdit
    Left = 760
    Top = 575
    Width = 23
    Height = 21
    EditLabel.Width = 52
    EditLabel.Height = 13
    EditLabel.Caption = 'Array_Gap'
    LabelPosition = lpLeft
    TabOrder = 36
    Text = ' '
  end
  object LabeledEdit1: TLabeledEdit
    Left = 669
    Top = 664
    Width = 252
    Height = 21
    EditLabel.Width = 31
    EditLabel.Height = 13
    EditLabel.Caption = 'DELAY'
    LabelPosition = lpLeft
    TabOrder = 37
    Text = 'Delayms'
  end
  object btnOpenLogfile: TButton
    Left = 921
    Top = 663
    Width = 35
    Height = 22
    Caption = '...'
    TabOrder = 38
    OnClick = btnSaveClick
  end
  object btnSavePrj: TButton
    Left = 888
    Top = 509
    Width = 66
    Height = 22
    Caption = 'Save Prj'
    TabOrder = 39
    OnClick = btnSaveClick
  end
  object btnOpenPrj: TButton
    Left = 888
    Top = 534
    Width = 66
    Height = 22
    Caption = 'Open Prj'
    TabOrder = 40
    OnClick = btnSaveClick
  end
  object MainAppEvents: TApplicationEvents
    OnMessage = MainAppEventsMessage
    Left = 304
    Top = 48
  end
  object saveDlgInitial: TSaveDialog
    Left = 312
    Top = 136
  end
  object openDlgLogDir: TOpenDialog
    Left = 312
    Top = 192
  end
end

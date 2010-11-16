object Form1: TForm1
  Left = 298
  Top = 235
  Width = 627
  Height = 366
  BorderIcons = [biSystemMenu, biMinimize]
  Caption = 'MFPNT - Vs-'
  Color = clBtnFace
  Constraints.MinHeight = 366
  Constraints.MinWidth = 627
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Menu = MainMenu1
  OldCreateOrder = False
  Position = poDesktopCenter
  OnActivate = FormActivate
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  DesignSize = (
    619
    312)
  PixelsPerInch = 96
  TextHeight = 13
  object Label3: TLabel
    Left = 6
    Top = 287
    Width = 171
    Height = 17
    Anchors = [akLeft, akBottom]
    AutoSize = False
    Transparent = True
    WordWrap = True
  end
  object Label7: TLabel
    Left = 6
    Top = 98
    Width = 4
    Height = 15
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 33023
    Font.Height = -11
    Font.Name = 'Arial Black'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label2: TLabel
    Left = 610
    Top = 97
    Width = 4
    Height = 15
    Alignment = taRightJustify
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clRed
    Font.Height = -11
    Font.Name = 'Arial Black'
    Font.Style = []
    ParentFont = False
    Transparent = True
  end
  object Label4: TLabel
    Left = 85
    Top = 7
    Width = 449
    Height = 76
    Alignment = taCenter
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = 16744576
    Font.Height = -27
    Font.Name = 'Comic Sans MS'
    Font.Style = []
    ParentFont = False
    Transparent = True
    WordWrap = True
  end
  object Label5: TLabel
    Left = 85
    Top = 8
    Width = 449
    Height = 76
    Alignment = taCenter
    AutoSize = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -27
    Font.Name = 'Comic Sans MS'
    Font.Style = []
    ParentFont = False
    Transparent = True
    WordWrap = True
  end
  object Button1: TButton
    Left = 541
    Top = 282
    Width = 75
    Height = 25
    Anchors = [akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 0
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 318
    Top = 282
    Width = 135
    Height = 25
    Anchors = [akLeft, akBottom]
    Enabled = False
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = []
    ParentFont = False
    TabOrder = 1
    OnClick = Button2Click
  end
  object ListBox1: TListBox
    Left = 6
    Top = 114
    Width = 609
    Height = 161
    Style = lbOwnerDrawFixed
    Anchors = [akLeft, akTop, akRight, akBottom]
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clBlack
    Font.Height = -12
    Font.Name = 'Courier'
    Font.Style = []
    ItemHeight = 13
    ParentFont = False
    TabOrder = 2
    OnClick = ListBox1Click
    OnDrawItem = ListBox1DrawItem
  end
  object RadioButton1: TRadioButton
    Left = 176
    Top = 286
    Width = 33
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = 'A:'
    Checked = True
    TabOrder = 3
    TabStop = True
  end
  object RadioButton2: TRadioButton
    Left = 216
    Top = 286
    Width = 41
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = 'B:'
    TabOrder = 4
  end
  object RadioButton3: TRadioButton
    Left = 256
    Top = 286
    Width = 57
    Height = 17
    Anchors = [akLeft, akBottom]
    Caption = 'bs.bin'
    TabOrder = 5
  end
  object XPManifest1: TXPManifest
    Left = 424
    Top = 8
  end
  object MainMenu1: TMainMenu
    Left = 391
    Top = 8
    object Langue1: TMenuItem
      OnClick = Langue1Click
    end
    object Info1: TMenuItem
      OnClick = Info1Click
    end
  end
end

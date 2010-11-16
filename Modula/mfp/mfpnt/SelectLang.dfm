object Form3: TForm3
  Left = 352
  Top = 304
  BorderIcons = []
  BorderStyle = bsToolWindow
  Caption = 'PM3DCDNT '
  ClientHeight = 82
  ClientWidth = 166
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  FormStyle = fsStayOnTop
  OldCreateOrder = False
  Position = poDesktopCenter
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 11
    Top = 5
    Width = 111
    Height = 19
    Caption = 'Select language :'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clNavy
    Font.Height = -13
    Font.Name = 'Comic Sans MS'
    Font.Style = [fsBold]
    ParentFont = False
  end
  object ComboBox1: TComboBox
    Left = 11
    Top = 29
    Width = 145
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    ItemIndex = 0
    TabOrder = 0
    Text = 'Fran'#231'ais'
    Items.Strings = (
      'Fran'#231'ais'
      'English')
  end
  object Button1: TButton
    Left = 91
    Top = 56
    Width = 65
    Height = 20
    Caption = 'Ok'
    ModalResult = 1
    TabOrder = 1
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 11
    Top = 56
    Width = 65
    Height = 20
    ModalResult = 2
    TabOrder = 2
    Visible = False
  end
end

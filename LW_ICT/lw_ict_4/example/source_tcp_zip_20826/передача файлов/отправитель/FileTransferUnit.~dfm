object TransferForm: TTransferForm
  Left = 192
  Top = 107
  Width = 112
  Height = 120
  Caption = #1055#1077#1088#1077#1076#1072#1095#1072' '#1092#1072#1081#1083#1086#1074
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  PixelsPerInch = 96
  TextHeight = 13
  object StartButton: TButton
    Left = 8
    Top = 32
    Width = 75
    Height = 25
    Caption = #1047#1072#1087#1091#1089#1090#1080#1090#1100
    TabOrder = 0
    OnClick = StartButtonClick
  end
  object SendButton: TButton
    Left = 8
    Top = 64
    Width = 75
    Height = 25
    Caption = #1054#1090#1087#1088#1072#1074#1080#1090#1100
    TabOrder = 1
    OnClick = SendButtonClick
  end
  object OpenDialog: TOpenDialog
    Left = 8
  end
  object Server: TServerSocket
    Active = False
    Port = 1010
    ServerType = stNonBlocking
    OnClientRead = ServerClientRead
    Left = 48
  end
end

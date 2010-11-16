unit SelectLang;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, Traduction, registry;

type
  TForm3 = class(TForm)
    Label1: TLabel;
    ComboBox1: TComboBox;
    Button1: TButton;
    Button2: TButton;
    procedure Button1Click(Sender: TObject);
  private
    { Déclarations privées }
  public
    { Déclarations publiques }
  end;

implementation

{$R *.dfm}

procedure TForm3.Button1Click(Sender: TObject);
var Reg: TRegistry;
begin
SetLang(ComboBox1.ItemIndex);

Reg := TRegistry.Create(KEY_WRITE);
If Reg <> nil then
With Reg do
  begin
    RootKey := HKEY_CURRENT_USER;
    if OpenKey('Software\Modula\MFPDNT',true) then
                          WriteInteger('Language',ComboBox1.ItemIndex);
    Free;
  end;
end;

end.

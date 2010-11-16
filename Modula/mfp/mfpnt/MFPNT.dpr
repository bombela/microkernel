program MFPNT;

uses
  Forms,
  Main in 'Main.pas' {Form1},
  Splash in 'Splash.pas' {Form2},
  Traduction in 'Traduction.pas',
  SelectLang in 'SelectLang.pas' {Form3},
  Registry,
  Windows;

{$E .bin}

{$R *.res}

var Form3: TForm3;
    Reg: TRegistry;

begin
  Application.Initialize;
  Application.Title := 'PM3DCDNT';

Reg := TRegistry.Create(KEY_READ);
If Reg <> nil then
With Reg do
  begin
    RootKey := HKEY_CURRENT_USER;
    if OpenKey('Software\Modula\MFPDNT', false) then
      begin
        SetLang(ReadInteger('Language'));
      end else
      begin
        Form3 := TForm3.Create(Application);
        Form3.Caption := Form3.Caption + VersionProg;
        Form3.ShowModal;
        Form3.Release;
      end;
    Free;
  end;

  Application.CreateForm(TForm1, Form1);
  Application.Run;
end.

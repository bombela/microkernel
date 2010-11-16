unit Main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, XPMan, ExtCtrls, jpeg, Splash, Traduction, Menus;

type
  TForm1 = class(TForm)
    Button1: TButton;
    XPManifest1: TXPManifest;
    Button2: TButton;
    Label3: TLabel;
    ListBox1: TListBox;
    MainMenu1: TMainMenu;
    Langue1: TMenuItem;
    Info1: TMenuItem;
    Label7: TLabel;
    Label2: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    RadioButton1: TRadioButton;
    RadioButton2: TRadioButton;
    RadioButton3: TRadioButton;
    procedure Button1Click(Sender: TObject);
    procedure ListBox1DrawItem(Control: TWinControl; Index: Integer;
      Rect: TRect; State: TOwnerDrawState);
    procedure ListBox1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure FormActivate(Sender: TObject);
    procedure Langue1Click(Sender: TObject);
    procedure Info1Click(Sender: TObject);
    procedure AddList(Text: string; Color: TColor);
    procedure FormCreate(Sender: TObject);
  private
    { Déclarations privées }
  public
    { Déclarations publiques }
  end;

const VersionProg = '3.2';
      Created = '01/02/2004';
      LastUpdate = '12/09/2004';
      ColorPartPrin = clGreen;
      ColorPartLogi = clOlive;
      ColorPartPrinError = clgray;
      ColorPartLogiError = clsilver;
      OffBootVs = $01E5;
      BootVsSize = $13;

type TFAT32_INFO = packed record
Jmp                :array[0..2] of byte;
OemName            :array[0..7] of char;

BytePerSector      :word;
SectorPerCluster   :byte;
ReservedSector     :word;
NumberOfFat        :byte;
RootEntries        :word;
TotalSector        :word;
Media              :byte;
FatSize            :word;
SectorPerTrack     :word;
HeadPerCylinder    :word;
NbHideSector       :cardinal;
NbSectorBig        :cardinal;
FatSizeBig         :cardinal;
Flags              :word;
Version            :word;
RootCluster        :cardinal;
InfoSector         :word;
RootBackupStart    :word;
Reserved           :array[0..11] of byte;

DriveNumber        :byte;
Unused             :byte;
ExtBootSignature   :byte;
SerialNumber       :cardinal;
LabelVolume        :array[0..10] of char;
FileSystem         :array[0..7] of char;
end;

type TPartitionItem = packed record
state  : byte; { 80h = bootable, 0 = statique.}
head   : byte;
CylSect: word;
Fs     : byte; { 04h = Fat16, 11d = Fat32, 05=Etendue}
headf  : byte;
CylSectf : word;
posi   : cardinal;
length : cardinal;
end;
type TPartitionTab = packed array[0..3] of TPartitionItem;

type TFAT32_ENTRY = packed record
Name               :array[0..7] of char;
Ext                :array[0..2] of char;
Attr               :byte;
Reserved_NT        :byte;
CreateTimeTenth    :byte;
CreateTime         :word;
CreateDate         :word;
LastAccessDate     :word;
FirstCluster_hi    :word;
TimeModif          :word;
DateModif          :word;
FirstCluster       :word;
Size               :cardinal;
end;

type TPartType = (Principale,Secondaire,Logique);

var
  Form1: TForm1;
  PartitionList: array of TPartitionItem;
  Splash: TForm2;
  Activated: boolean = false;

implementation

uses SelectLang;

{$R *.dfm}

function ReadSector(Disk: byte;LBA: cardinal;Buf: pointer;Count: cardinal):boolean;
var
  Read: dword;
  dwpointer: dword;
  hdevice: thandle;
  ldistancelow, ldistancehigh: dword;
begin
  Result := false;
  hDevice:=CreateFile(pchar('\\.\PhysicalDrive'+inttostr(Disk)), GENERIC_READ, FILE_SHARE_READ or FILE_SHARE_WRITE,
    nil, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH, 0);
  if hDevice <> INVALID_HANDLE_VALUE then
  begin
    ldistanceLow:=dword(LBA SHL 9);
    ldistanceHigh:=dword(LBA SHR (32-9));
    dwpointer:=SetFilePointer(hdevice, ldistancelow,@ldistancehigh, FILE_BEGIN);
    if dwPointer <> $FFFFFFFF then
    begin
      Result := ReadFile(hDevice, buf^, Count*512, Read, nil);
    end;
    CloseHandle(hDevice);
  end;
  Result:= Result and (Read=Count*512);
end;

function ReadPartitionTab(Disk: byte;LBA: cardinal;var PartitionTab: TPartitionTab):boolean;
var Buf: Pointer;
begin
Buf := AllocMem(512);
Result := false;
If ReadSector(Disk,LBA,Buf,1) then
  begin
   asm
    pushfd
    pushad
    cld
    mov esi, [Buf]
    add esi, 446
    mov edi, [PartitionTab]
    mov ecx, 64
    REP MovsB
    popad
    popfd
   end;
   Result := true;
  end;
FreeMem(Buf);
end;

function ReadFAT32VolumeName(Disk: byte; BootSect: Cardinal): string;
var Buf: Pointer;
    BufClus: pointer;
    FAT32: ^TFAT32_INFO;
    Sect, i: Cardinal;
    Clus: Cardinal;
    StartData: Cardinal;
    StartFat: Cardinal;
    Entry: ^TFAT32_ENTRY;
    Trouve, ReadOK: boolean;
begin
Trouve := false;
ReadFAT32VolumeName := '?!'+SepareLang(S11)+'!?';
GetMem(Buf,512);
If ReadSector(Disk,BootSect,Buf,1) then
  begin
    FAT32 := Buf;

    StartFat := BootSect + FAT32^.ReservedSector;
    StartData := StartFat+ (FAT32^.NumberOfFat*FAT32^.FatSizeBig);

    GetMem(BufClus,FAT32^.SectorPerCluster*512);

    Clus := FAT32^.RootCluster;

  repeat
    i := 0;
    Sect := StartData+((Clus-2)*FAT32^.SectorPerCluster);
    ReadOK := false;
    If ReadSector(Disk,Sect,BufClus,FAT32^.SectorPerCluster) then
     begin
      ReadOk := true;
      repeat
        Entry := pointer(cardinal(bufclus)+i);
        if Entry^.Attr = 8 then
          begin
            ReadFAT32VolumeName := Entry^.Name+Entry^.Ext;
            Trouve := true;
            break;
          end;
        inc(i,32);
      until i >= (FAT32^.SectorPerCluster*512);
     end else break;
     if ReadSector(Disk,StartFat+(Clus div 128),BufClus,1) then
        Clus := longint(pointer(cardinal(BufClus)+(Clus mod 128 * 4))^) else break;
  until Clus >= $0FFFFFF8;  

    FreeMem(BufClus,FAT32^.SectorPerCluster*512);

    If (ReadOK) and (not trouve) then  ReadFAT32VolumeName := FAT32^.LabelVolume;

  end;
FreeMem(Buf,512);
end;

procedure TForm1.Button1Click(Sender: TObject);
begin
close;
end;

procedure TForm1.ListBox1DrawItem(Control: TWinControl; Index: Integer;
  Rect: TRect; State: TOwnerDrawState);
begin
with ListBox1.Canvas do
  begin
    if odSelected	in State then
    begin
      Brush.Color := $00C1D3FF;
      Pen.Color := clGray;
      FillRect(rect);
    end else
    begin
      Brush.Color := clWhite;
      Pen.Color := clWhite;
      FillRect(rect);
    end;
    Font.Color := integer(ListBox1.Items.Objects[Index]);
    Brush.Style := bsClear;
    TextRect(Rect,Rect.Left,Rect.Top,ListBox1.Items[Index]);
  end;
end;

procedure TForm1.ListBox1Click(Sender: TObject);
begin
if ListBox1.ItemIndex >= 0 then
case integer(ListBox1.Items.Objects[ListBox1.ItemIndex]) of
    ColorPartPrin: Button2.Enabled := true;
    ColorPartLogi: Button2.Enabled := true;
   else Button2.Enabled := false; end;
end;

procedure TForm1.Button2Click(Sender: TObject);
const Disk = 512-2-4-1;
      LBA  = 512-2-4;

var hFloppy: hfile;
    Buf: pointer;
    Write: cardinal;
    F: TFileStream;
    PPartitionItem: ^TPartitionItem;
    LF: string;
begin
  Buf := AllocMem(512);
  F := TFileStream.Create(Application.ExeName,fmOpenRead or fmShareDenyNone);
  F.Seek(-512,soFromEnd);
  F.ReadBuffer(buf^,512);
  F.Free;
  PPartitionItem := @PartitionList[ListBox1.Itemindex];
  asm
    pushfd
    pushad
    mov esi, dword [PPartitionItem]
    mov edi, [Buf]
    mov al, [esi]
    add al, 80h
    mov [edi+Disk], al
    mov eax, [esi+8]
    mov [edi+LBA], eax
    popad
    popfd
  end;

  if RadioButton3.Checked then
  begin
    F := TFileStream.Create(RadioButton3.Caption,fmCreate or fmShareExclusive);
    F.WriteBuffer(Buf^,512);
    F.Free;
    MessageDlg(SepareLang(S12),mtInformation,[mbOK],0)
  end else
  begin
    if RadioButton1.Checked then LF := '\\.\A:' else LF := '\\.\B:';
    hFloppy := CreateFile(pchar(LF),GENERIC_WRITE,0,nil,OPEN_ALWAYS,0,0);
    If hFloppy <> INVALID_HANDLE_VALUE then
      begin
        If WriteFile(hFloppy,buf^,512,Write,nil)
          then MessageDlg(SepareLang(S15),mtInformation,[mbOK],0)
          else MessageDlg(SepareLang(S16),mtError,[mbOK],0);
        CloseHandle(hFloppy);
      end else MessageDlg(SepareLang(S17),mtError,[mbOK],0);
  end;
  FreeMem(Buf);
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
Finalize(PartitionList);
end;

procedure TForm1.FormActivate(Sender: TObject);
var PartitionTab: TPartitionTab;
    PartitionLogical: TPartitionTAb;
    i, ii, iii: integer;
    PartType: array of TPartType;
    ActuPosi: cardinal;

begin
If Activated then exit;
Activated := true;

ListBox1.Clear;

Splash := TForm2.Create(Application);
Splash.Label1.Caption := SepareLang(S01);
Splash.Show;
Splash.Update;

// Mise en place des traductions

Label4.Caption := SepareLang(S03);
Label5.Caption := Label4.caption;
Label7.Caption := SepareLang(S04);
Label2.Caption := SepareLang(S05);
Label3.Caption := SepareLang(S06);
Button2.Caption := SepareLang(S07);
Button1.Caption := SepareLang(S08);
Langue1.Caption := SepareLang(S18);
Info1.Caption := SepareLang(S19);

// ----------------------------

For i := 0 to 3 do
If ReadPartitionTab(i,0,PartitionTab) then
begin

for ii := 0 to 3 do
  begin
    case PartitionTab[ii].fs of
       11,12: begin
              setlength(PartType,length(PartType)+1);
              If PartitionTab[ii].state = 0 then PartType[length(PartType)-1] := Secondaire else PartType[length(PartType)-1] := Principale;
              AddList(ReadFAT32VolumeName(i,PartitionTab[ii].posi)+' [FAT32] '+SepareLang(S09)+'('+IntToStr(i)+'), '+SepareLang(S10)+'('+IntToStr(ii)+')',ColorPartPrin);
              setlength(PartitionList,length(PartitionList)+1);
              PartitionList[length(PartitionList)-1] := PartitionTab[ii];
              PartitionList[length(PartitionList)-1].state := i;
            end;
       5,15: begin
           PartitionLogical[1].posi := 0;
           iii := 0;
              repeat
              ActuPosi := PartitionTab[ii].posi+PartitionLogical[1].Posi;
              If ReadPartitionTab(i,ActuPosi,PartitionLogical) then
                 begin
                 case PartitionLogical[0].fs of
                  11,12: begin
                         setlength(PartType,length(PartType)+1);
                         PartType[length(PartType)-1] := Logique;
                         AddList(ReadFAT32VolumeName(i,ActuPosi+PartitionLogical[0].posi)+' [FAT32] '+SepareLang(S09)+'('+IntToStr(i)+'), '+SepareLang(S10)+'('+IntToStr(ii)+') > '
                         +SepareLang(S13)+'('+IntToStr(iii)+')',ColorPartLogi);
                         setlength(PartitionList,length(PartitionList)+1);
                         PartitionList[length(PartitionList)-1].posi := ActuPosi+PartitionLogical[0].posi;
                         PartitionList[length(PartitionList)-1].state := i;
                       end;
                   else
                     setlength(PartType,length(PartType)+1);
                     PartType[length(PartType)-1] := Logique;
                     AddList('            [     ] '+SepareLang(S09)+'('+IntToStr(i)+'); '+SepareLang(S10)+'('+IntToStr(ii)+') > '+SepareLang(S13)+'('+IntToStr(iii)+') : '
                      +SepareLang(S14)+' ('+inttostr(PartitionLogical[0].Fs)+').',ColorPartLogiError);
                     setlength(PartitionList,length(PartitionList)+1);
                   end;
                 end;
               inc(iii);
              until (PartitionLogical[1].fs <> 5) and (PartitionLogical[1].fs <> 15);
            end;
      else
        if PartitionTab[ii].Fs > 0 then
        begin
          setlength(PartType,length(PartType)+1);
          If PartitionTab[ii].state = 0 then PartType[length(PartType)-1] := Secondaire else PartType[length(PartType)-1] := Principale;
          AddList('            [     ] '+SepareLang(S09)+'('+IntToStr(i)+'), '+SepareLang(S10)+'('+IntToStr(ii)+') : '+SepareLang(S14)+' ('+inttostr(PartitionTab[ii].Fs)+').',ColorPartPrinError);
          setlength(PartitionList,length(PartitionList)+1);
        end;
      end;
  end;
end;

Splash.Label1.Caption := SepareLang(S02);
If length(PartitionList) = 0 then ListBox1.AddItem(SepareLang(S21),tobject(clRED));

  // Calcul du scroll horizontal.
  if ListBox1.Count > 0 then
  begin
    ii := 0;
    for i := 0 to ListBox1.Count-1 do
    begin
      iii := ListBox1.Canvas.TextWidth(ListBox1.Items[i]);
      if iii > ii then ii := iii;
    end;
    ListBox1.ScrollWidth := ii*2;
  end;
end;

procedure TForm1.Langue1Click(Sender: TObject);
var Form3: TForm3;
begin
Form3 := TForm3.Create(Application);
Form3.Caption := Form3.Caption + VersionProg;
Form3.Button2.Caption := SepareLang(S20);
Form3.Button2.Show;
Form3.ComboBox1.ItemIndex := GetLang;
Form3.ComboBox1.Tag := GetLang;
If (Form3.ShowModal = 1) and  (Form3.ComboBox1.ItemIndex <> Form3.ComboBox1.Tag) then
  begin
   Activated := false;
   Button2.Enabled  := false;
   FormActivate(nil);
  end;
Form3.Release;
end;

procedure TForm1.Info1Click(Sender: TObject);
begin
MessageDlg(Created+#10+#13+LastUpdate+#10+#13+'Bombela'+#10+#13+'supercar@tiscali.fr',mtInformation,[mbOK],0);
end;

procedure TForm1.AddList(Text: string; Color: TColor);
begin
ListBox1.AddItem(Text,tobject(Color));
end;

procedure TForm1.FormCreate(Sender: TObject);
var F: TFileStream;
    BootVs : array[0..BootVsSize-1] of char;
begin
    F := TFileStream.Create(Application.ExeName,fmOpenRead or fmShareDenyNone);
    F.Seek(OffBootVs-512,soFromEnd);
    F.ReadBuffer(BootVs,BootVsSize);
    F.Free;
    Form1.Caption := Form1.Caption+VersionProg+' - '+BootVs;
end;

end.

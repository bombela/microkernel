program MFP16;

uses CRT, DOS, Img, Trad, ScrolBox;

{DEFINE DEBUG}

const Version = '2.6';
      Date_Create  = '03/02/2004';
      Date_Update    = '25/10/2004';
      Author  = 'Coded by Bombela - Thank at Ludolpif for english traduction.';

      {$IFDEF DEBUG}
      {ImgSize = 0;}
      {$ELSE}
      ImgSize = 921656;
      {$ENDIF}

      OffBootVs = $01E5;
      BootVsSize = $13;
      ColorPartPrin = 10;
      ColorPartLogi = 11;
      ColorPartPrinError = 8;
      ColorPartLogiError = 8;

type TPartitionItem = packed record
state  : byte; { 80h = bootable, 0 = statique.}
head   : byte;
CylSect: word;
Fs     : byte; { 04h = Fat16, 11d = Fat32, 05=Etendue ...}
headf  : byte;
CylSectf : word;
posi   : longint;
length : longint;
end;
type TPartitionTab = packed array[0..3] of TPartitionItem;

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
NbHideSector       :longint;
NbSectorBig        :longint;
FatSizeBig         :longint;
Flags              :word;
Version            :word;
RootCluster        :longint;
InfoSector         :word;
RootBackupStart    :word;
Reserved           :array[0..11] of byte;

DriveNumber        :byte;
Unused             :byte;
ExtBootSignature   :byte;
SerialNumber       :longint;
LabelVolume        :array[0..10] of char;
FileSystem         :array[0..7] of char;
end;

type TDAP = packed record
Size: byte;
Reserved1: byte;
NumSectorRead: byte;
Reserved2: byte;
BlockPtr: pointer;
LBA_lo: LongInt;
LBA_hi: Longint;
end;

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
Size               :longint;
end;

function IntToStr(I: Longint): String;
var
 S: string[11];
begin
 Str(I, S);
 IntToStr := S;
end;

function IntToStr00(I: Longint): String;
var
 S: string[11]; Num0: byte;
begin
 Str(I, S);
 Num0 := 2 - length(s);
 If Num0 > 0 then for Num0 := 1 to Num0 do S := '0'+S;
 IntToStr00 := S;
end;

Function ReadSector(NumDisk: byte;LBA: LongInt;Buf:pointer;NumSector: byte): boolean;
var DAP: TDAP; Regs: Registers;
begin
with DAP do
     begin
          Size := 16;
          Reserved1 := 0;
          NumSectorRead := NumSector;
          Reserved2 := 0;
          BlockPtr := Buf;
          LBA_lo := LBA;
          LBA_hi := 0;
     end;
with REGS do
     begin
          ah := $42;
          dl := NumDisk+$80;
          ds := seg(Dap);
          si := ofs(Dap);
     end;
     intr($13,Regs);
ReadSector := Regs.ah = 0;
end;

function ReadPartitionTab(NumDisk: byte;LBA: longint;var PartitionTab: TPartitionTab): boolean;
var Buf: pointer; TabSize: word;
begin
ReadPartitionTab := false;
GetMem(Buf,512);
If ReadSector(NumDisk,LBA,Buf,1) then
   begin
     TabSize := sizeof(TPartitionTab);
     asm
      push ds
      lds si, Buf
      les di, [PartitionTab]
      add si, 446
      mov cx, [TabSize]
      rep movsb
      pop ds
     end;
     ReadPartitionTab := true;
   end;
FreeMem(Buf,512);
end;

function ReadFAT32VolumeName(Disk: byte; LbaMbr, LbaRelativeFat: longint): string;
var Buf: Pointer;
    BufClus: pointer;
    FAT32: ^TFAT32_INFO;
    Sect, i: longint;
    Clus: longint;
    StartData: longint;
    StartFat: longint;
    Entry: ^TFAT32_ENTRY;
    Trouve, ReadOK: boolean;
begin
Trouve := false;
ReadFAT32VolumeName := SepareLang(S08);
GetMem(Buf,512);
If ReadSector(Disk,LbaMbr+LbaRelativeFat,Buf,1) then
  begin
    FAT32 := Buf;

    StartFat := LbaMbr+FAT32^.ReservedSector+LbaRelativeFat;
    StartData := LbaMbr+FAT32^.ReservedSector+LbaRelativeFat+
                  (FAT32^.NumberOfFat*FAT32^.FatSizeBig);

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
        Entry := pointer(longint(bufclus)+i);
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
       begin
         Clus := longint(pointer(longint(BufClus)+(Clus mod 128 * 4))^);
       end else break;

  until Clus >= $0FFFFFF8;

    FreeMem(BufClus,FAT32^.SectorPerCluster*512);

If (ReadOK) and (not trouve) then ReadFAT32VolumeName := FAT32^.LabelVolume;

  end;
FreeMem(Buf,512);
end;

function StrToInt(Chaine: string): integer;
var
 V, Code: integer;
begin
 Val(Chaine,v,Code);
 StrToInt := V;
end;

var ScrollBox: TScrollBox;

procedure AddList(Text: string; Color: byte);
begin
  If (Color =ColorPartPrinError) or (Color = ColorPartLogiError)
  	then ScrollBox.Strings.Add(Text,Color,false) else ScrollBox.Strings.Add(Text,Color,true);
end;


type TPartType = (Principale,Secondaire,Logique,PrincipaleErr,SecondaireErr,LogiqueErr);
var PartitionTab: TPartitionTab;
    PartitionLogical: TPartitionTab;
    PartitionList: array[0..25] of ^TPartitionItem;
    ListSize: byte;
    i,ii,iii: byte;
    Num: char;
    BufNum : string[2];
    F: file;
    BootSector: pointer;
    BmpPosi: longint;
    BootVs : array[0..BootVsSize-1] of char;
    Lettre: char;
    X,Y: byte;
    PartType: array[0..25] of TPartType;
    NumPart: byte;
    ActuPosi: longint;
    Text: string;
    Selectable: boolean;
begin
clrscr;
writeln('PM3DCD16 ',Version);
writeln;
writeln('Select language :');
writeln(' 1: Fran‡ais');
writeln(' 2: English');
writeln;

{$IFNDEF DEBUG}

repeat
num := readkey;
until (ord(num) = 27) or ((ord(num) >= 48) and (ord(num) <= 48+NumLang));
if ord(num) = 27 then
   begin
     writeln('Ended...');
     writeln;
     exit;
   end;

{$ELSE}
num := #49;
{$ENDIF}

SetLang(ord(num)-48-1);

{$IFNDEF DEBUG}

if VESA_Test then
begin
	VGA_SaveMode;

	assign(f,paramstr(0));
	reset(f,1);
	BmpPosi := filesize(f)-ImgSize-512;
	close(f);

	VESA_SetMode($112);
	VESA_PaintBMPfile(paramstr(0),BmpPosi,0,0,640);

	readkey;

	VGA_RestoreMode;
end else
begin
	CLRSCR;
	WriteLn(SepareLang(S10));
	WriteLn(SepareLang(S15));
	readkey;
end;

{$ENDIF}

CLRSCR;
textcolor(13);
writeln(SepareLang(S01));
textcolor(cyan);

      assign(f,paramstr(0));
      reset(f,1);
      Seek(f,FileSize(f)-512+OffBootVs);
      BlockRead(f,BootVs,BootVsSize);
      close(f);

writeln('Version-'+Version+' - '+BootVs);
writeln(Author);
writeln(Date_Create+'..'+Date_Update);
textcolor(4);
writeln;
writeln(SepareLang(S02));
textcolor(6);
writeln(SepareLang(S03));
writeln;

ListSize := 0;
NumPart := 0;

ScrollBox.Init;
ScrollBox.Top := WhereY;
ScrollBox.Left := 2;
ScrollBox.Width := 78;
ScrollBox.Heigth := 17-WhereY;

TextColor(14);
Write(SepareLang(S09));

For i := 0 to 3 do
If ReadPartitionTab(i,0,PartitionTab) then
begin

for ii := 0 to 3 do
  begin
    case PartitionTab[ii].fs of
       11,12: begin
              If PartitionTab[ii].State = 0 then PartType[NumPart] := Secondaire else PartType[NumPart] := Principale;
              Inc(NumPart);
              AddList(ReadFAT32VolumeName(i,0,PartitionTab[ii].Posi)+' [FAT32] '+SepareLang(S05)+'('+inttostr(i)+'), '
                 +SepareLang(S06)+'('+inttostr(ii)+')',ColorPartPrin);
              new(PartitionList[ListSize]);
              PartitionList[ListSize]^ := PartitionTab[ii];
              PartitionList[ListSize]^.state := i;
              Inc(ListSize);
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
                         PartType[NumPart] := Logique;
                         Inc(NumPart);
                         AddList(ReadFAT32VolumeName(i,ActuPosi,PartitionLogical[0].posi)+' [FAT32] '
                           +SepareLang(S05)+'('+inttostr(i)+'), '
                           +SepareLang(S06)+'('+inttostr(ii)+') > '+SepareLang(S07)+'('+inttostr(iii)+')',ColorPartLogi);
                         new(PartitionList[ListSize]);
                         PartitionList[ListSize]^ := PartitionLogical[0];
                         PartitionList[ListSize]^.state := i;
                         PartitionList[ListSize]^.Posi := ActuPosi+PartitionLogical[0].posi;
                         Inc(ListSize);
                       end;
                   else
                     PartType[NumPart] := LogiqueErr;
                     Inc(NumPart);
                     AddList('            [     ] '+SepareLang(S05)+'('+inttostr(i)+'), '+SepareLang(S06)+'('+inttostr(ii)+
                     	') > '+SepareLang(S07)+'('+inttostr(iii)+')',ColorPartLogiError);
                   end;
                 end;
               inc(iii);
              until (PartitionLogical[1].fs <> 5) and (PartitionLogical[1].fs <> 15);
            end;
      else
        if PartitionTab[ii].fs > 0 then
          begin
            If PartitionTab[ii].State = 0 then PartType[NumPart] := SecondaireErr else PartType[NumPart] := PrincipaleErr;
            Inc(NumPart);
            AddList('            [     ] '+SepareLang(S05)+'('+inttostr(i)+'), '
            	+SepareLang(S06)+'('+inttostr(ii)+')',ColorPartPrinError);
         end;
      end;
  end;

end;

gotoXY(1,whereY);
for i := 1 to 80 do write(' ');
GotoXY(1,WhereY-1);

{-------------------------}

If NumPart > 0 then
begin
	ScrollBox.Paint(False);
  ScrollBox.SetCursorBotom;
  writeln;
  textcolor(7);
  write(' '+SepareLang(S17));
   
  ii := 0;
  while ii=0 do
  begin
    if ScrollBox.Select(i) then
    begin
  		if (byte(PartType[i]) < 3) then ii := 1;
  	end else ii := 2;
  end;
   
   
  ScrollBox.Paint(True);
  gotoXY(1,whereY);
  for iii := 1 to 80 do write(' '); 
  GotoXY(1,WhereY-ScrollBox.Heigth-2);
  
  if ii = 2 then
  begin
  	textcolor(7);
    clrscr;
  	writeln;
		writeln(SepareLang(S11));
  	exit;
  end;
  
  ScrollBox.Strings.Get(i,Text,ii,Selectable);
  textcolor(14);
  writeln(separelang(S04)+' :');
  textcolor(9);
  write('[');
  textcolor(ii);
  write(Text);
  textcolor(9);
  writeln(']');
  writeln;
    
	TextColor(13);
	repeat
  	write(#13,SepareLang(S12)+' (A,B): [  ]',#8,#8,#8);
    ii := 3;
    repeat
    	Num := readkey;
    	If ord(Num) = 27 then
    	begin
      	TextColor(7);
      	clrscr;
				writeln;
				writeln(SepareLang(S11));
				exit;
			end;
			Num := UpCase(Num);
			if GetCurrentLang = AN then
			begin
				Case Num of
				 'Q': begin ii := 0; write('A:]',#8,#8,#8); end;
				 'B': begin ii := 1; write('B:]',#8,#8,#8); end;
				end;
			end else
			begin
				Case Num of
				 'A': begin ii := 0; write('A:]',#8,#8,#8); end;
				 'B': begin ii := 1; write('B:]',#8,#8,#8); end;
				end;
			end;
		until ord(Num)=13;
	until ii < 3;
      
	writeln;
	writeln;
	textcolor(14);
	write(SepareLang(S13)+' ');
	assign(f,paramstr(0));
	reset(f,1);
	Seek(f,FileSize(f)-512);
	GetMem(BootSector,512);
	BlockRead(f,BootSector^,512);
	close(f);
	PartitionTab[0] := PartitionList[i]^;
	
	asm
		les di, [BootSector]
		mov al, byte [PartitionTab]
		add al, 80h
		mov byte [es:di+512-7], al
		mov ax, word [PartitionTab+8]
		mov word [es:di+512-6], ax
		mov ax, word [PartitionTab+10]
		mov word [es:di+512-4], ax
		mov cx, 3
		@Write:
		 cmp cx, 0
		 je @Err
		 push cx
		 les bx, [BootSector]
		 mov ax, 0301h
		 mov dl, [ii]
		 xor dh, dh
		 mov cx, 1
		 int 13h
		 pop cx
		 dec cx
		jc @Write
		 @Err:
		 mov byte [ii], ah
	 end;
	 
	FreeMem(BootSector,512);
	If ii = 0 then
	 begin
		textcolor(10);
		writeln(SepareLang(S11));
	 end else
	 begin
		textcolor(12);
		writeln(SepareLang(S14));
	 end;
	readkey;
	textcolor(7);
	clrscr;
	writeln;
	writeln(SepareLang(S11));
end else
begin
	TextColor(12);
	WriteLn(SepareLang(S16));
	readkey;
	textcolor(7);
	clrscr;
	writeln;
	writeln(SepareLang(S11));	
end;

If ListSize > 0 then For i := 0 to ListSize-1 do dispose(PartitionList[i]);
end.
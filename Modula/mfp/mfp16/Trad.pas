unit Trad;

interface

const
      NumLang = 2;
      FR = 0; AN = 1;
      S01 = 'Cr‚ation d''une disquette de d‚marrage ModulaõModula''s boot floppy creatorõ';
      S02 = 'Pour DOS et Windows 95/98/MEõFor DOS and Windows 95/98/MEõ';
      S03 = 'Seulement pour les partitions FAT32õOnly for FAT32 partitionõ';
      S04 = 'Partition choisieõPartition selectedõ';
      S05 = 'DisqueõDiskõ';
      S06 = 'PartitionõPartitionõ';
      S07 = 'Partition logiqueõLogical partitionõ';
      S08 = '? Erreur ?õ? Error ?õ';
      S09 = 'Analyse des disques...õAnalysing disks...õ';
      S10 = 'Pas de mode VESA, pas de logo Modula !õNo VESA mode, no Modula''s logo !õ';
      S11 = 'Termin‚...õFinished...õ';
      S12 = 'Lecteur disquette de destinationõFloppy drive destinationõ';
      S13 = 'Ecriture sur la disquette...õWriting on floppy...õ';
      S14 = 'Erreur d''‚criture !õWrite error !õ';
			S15 = 'Pressez une touche pour continuer...õPress any key to continue...õ';
      S16 = 'Aucune partition reconnue...õNoone partition was recognized...õ';
      S17 = 'Selectionnez une partition valide et pressez sur [Entr‚e]õSelect a valid partition and press [Return]õ';

procedure SetLang(NEWIDLANG: integer);
function SepareLang(StringLang: string): string;
function GetCurrentLang: integer;

implementation

var IDLANG: integer;

procedure SetLang(NEWIDLANG: integer);
begin
  IDLANG := NEWIDLANG;
end;

function SepareLang(StringLang: string): string;
var TMP: string; i: integer;
begin
TMP := StringLang;
For i := 0 to IDLANG do
  begin
    SepareLang := copy(TMP,0,pos('õ',TMP)-1);
    TMP := copy(TMP,pos('õ',TMP)+1,length(TMP));
  end;
end;

function GetCurrentLang: integer;
begin
  GetCurrentLang := IDLANG;
end;

end.
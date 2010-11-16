unit Trad;

interface

const
      NumLang = 2;
      FR = 0; AN = 1;
      S01 = 'Cr�ation d''une disquette de d�marrage Modula�Modula''s boot floppy creator�';
      S02 = 'Pour DOS et Windows 95/98/ME�For DOS and Windows 95/98/ME�';
      S03 = 'Seulement pour les partitions FAT32�Only for FAT32 partition�';
      S04 = 'Partition choisie�Partition selected�';
      S05 = 'Disque�Disk�';
      S06 = 'Partition�Partition�';
      S07 = 'Partition logique�Logical partition�';
      S08 = '? Erreur ?�? Error ?�';
      S09 = 'Analyse des disques...�Analysing disks...�';
      S10 = 'Pas de mode VESA, pas de logo Modula !�No VESA mode, no Modula''s logo !�';
      S11 = 'Termin�...�Finished...�';
      S12 = 'Lecteur disquette de destination�Floppy drive destination�';
      S13 = 'Ecriture sur la disquette...�Writing on floppy...�';
      S14 = 'Erreur d''�criture !�Write error !�';
			S15 = 'Pressez une touche pour continuer...�Press any key to continue...�';
      S16 = 'Aucune partition reconnue...�Noone partition was recognized...�';
      S17 = 'Selectionnez une partition valide et pressez sur [Entr�e]�Select a valid partition and press [Return]�';

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
    SepareLang := copy(TMP,0,pos('�',TMP)-1);
    TMP := copy(TMP,pos('�',TMP)+1,length(TMP));
  end;
end;

function GetCurrentLang: integer;
begin
  GetCurrentLang := IDLANG;
end;

end.
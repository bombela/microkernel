unit Traduction;

interface

const
      FR = 0; AN = 1; DU = 2;
      S01 = 'Analyse des disques...§Analysing disks...§Analyse scheibe§';
      S02 = 'Cliquez pour continuer...§Clic for continue...§';
      S03 = 'Création d''une disquette de démarrage pour Modula§Create boot floppy for Modula§';
      S04 = 'Pour Windows NT/2000/XP§For Windows NT/2000/XP§';
      S05 = 'Seulement pour les partitions FAT32§Only for partition FAT32§';
      S06 = 'Lecteur disquette de destination :§Floppy drive out :§';
      S07 = '&Copier sur la disquette !§Copy to floppy !§';
      S08 = '&Quitter§&Quit§';
      S09 = 'Disque§Disk§';
      S10 = 'Partition§Partition§';
      S11 = 'Erreur§Error§';
      S12 = 'Fichier bs.bin créé !§File bs.bin created !';
      S13 = 'Partition logique§Partition logical§';
      S14 = 'Système de fichier non supporté§File system not supported§';
      S15 = 'Ecriture réussie !§Write ok !§';
      S16 = 'Impossible d''écrire sur la disquette !§Write floppy error !§';
      S17 = 'Impossible d''accéder au lecteur disquette !§Access floppy drive error !§';
      S18 = 'Langue§Language§';
      S19 = 'Infos§About§';
      S20 = 'Annuler§Cancel§';
      S21 = 'Aucune partition reconnue§?Aucune partition reconnue?>Traduire§';

procedure SetLang(NEWIDLANG: integer);
function GetLang: integer;
function SepareLang(StringLang: string): string;

implementation

var IDLANG: integer = FR;

procedure SetLang(NEWIDLANG: integer);
begin
IDLANG := NEWIDLANG;
end;

function GetLang: integer; begin result := IDLANG; end;

function SepareLang(StringLang: string): string;
var TMP: string; i: integer;
begin
TMP := StringLang;
For i := 0 to IDLANG do
  begin
    Result := copy(TMP,0,pos('§',TMP)-1);
    TMP := copy(TMP,pos('§',TMP)+1,length(TMP));
  end;
end;


end.

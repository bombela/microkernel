/*
 *  Pilote d'affichage de texte.
 *	PMode 32 bits.
 *
 *	Retour à la ligne UNIX.
 *
 *	Peut écrire dans un fichier HTML ce qui se passe.
 *
 *	Bombela.
 *	27/06/2004
 *	22/04/2005
 *	29/01/2006 - log possible.
 *	06/02/2006 - Quelques vérifications dans le log pour éviter les pertes...
 */

//////////////////////////////////////////////

#include <Debug_Start.h>
#ifdef DEBUG_OUTTEXT
#define DEBUG_NAME "OutText "
#endif
#include <Debug_End.h>

#include <OutText.h>
#include <Io.h>
#include <Arg.h>
#include <Types.h>
#include <String.h>
#include <Date.h>
#include <Fat12.h>
#include <Timer.h>

#define VideoMem	0xB8000
#define LineSize	160
#define NumChar		80
#define NumLine		25

extern boolean Fat12_Enable; // Déclaré dans Loader32.c

// Il faut une taille assez grande, pour ne pas prendre de risque.
// Il doit avoir de quoi stoquer asser de log avant d'avoir assez
// de module chargé pour stoquer le log...
#define LogBufSize	(1024*64)

u8 Attribut = 7;
u8 X, Y;

boolean WriteToLog;
char LogBuf[LogBufSize];
u32 LogBufCount; // Emplacement dans le fichier de log.
TFat12_FileInfo LogFile; // Fichier de log.
boolean LogFileCreated;
u8 BufLogLastColor;
char LastLogedChar;

char TMC2HC[][8] =
{
	"#000000",
	"#0000A8",
	"#00A800",
	"#00A8A8",
	"#A80000",
	"#A800A8",
	"#A85700",
	"#A8A8A8",
	"#575757",
	"#5757FF",
	"#57FF57",
	"#57FFFF",
	"#FF5757",
	"#FF57FF",
	"#FFFF57",
	"#FFFFFF"
};

// Fonction définie plus bas.

void PutString2Log(char* Str);

// Debut du module.

// Définit le fébut d'un log html.
void AddHtmlStart()
{
	TDate_Hour Hour;
	TDate_Date Date;
	char date[20];
	
	PutString2Log("<html>\n<head>\n<meta http-equiv=\"content-type\"\
			content=\"text/html;charset=iso-8859-15\" />\n<title>");

	Date_GetCurrentDate(&Date);
	Date_GetCurrentHour(&Hour);
	
	Str_Fmt(date,"Log Modula - Le %U2 à %U2:%U2:%U2",Date.Day,Hour.Hour,Hour.Min,Hour.Sec);
	
	PutString2Log(date);
	
	PutString2Log("</title></head>\n\
			<body bgcolor=\"#000000\">\n<font face=\"Courier New\">\n");
}

// Créer le fichier de log.
u8 CreateLogFile()
{
	TDate_Hour Hour;
	TDate_Date Date;
	char LogName[12];
	
	if (LogFileCreated) return 0;
	
	// Création du fichier de log dans le dossier courant.
	// Syntaxe du nom : JOURS HEURES MINUTES SECONDES
	
	Date_GetCurrentDate(&Date);
	Date_GetCurrentHour(&Hour);
		
	Str_Fmt(LogName,"%U2%U2%U2%U2HTM",Date.Day,Hour.Hour,Hour.Min,Hour.Sec);
	
	if (Fat12_CreateFile(LogName,&LogFile))
	{
		WriteToLog = false;
		OutDbgColor(12,"Impossible de créer le fichier de log... ");
		WriteToLog = true;
		return 1;
	}
	LogFileCreated = true;
	
	OutDbg("Fichier de log créé : %s.",LogName);
	
	return 0;
}

void PutChar2Log(char Char)
{
	if (LogBufCount == LogBufSize)
	{
		if (!CreateLogFile())
		{		
			// Écriture du buffer de log.
			if (Fat12_WriteFile(&LogFile,LogBufCount,LogBuf))
			{	
				// ohh nooo...
				LogBufCount = 0; // Vide le buffer du log...
				AddHtmlStart();
				OutDbgColor(12,"\n\nErreur d'écriture du log !\n\n");
				
			} else
			{
				LogBufCount = 0; // Ok, on peux vider, c'est écris.
				OutDbgColor(11,"Buffer de log vidé.");
				LogBuf[LogBufCount++] = Char;
			}
		} else
		{
			// ohhh nooo...
			LogBufCount = 0; // Ben tant pis, on vide pour récup la suite...
			AddHtmlStart();
			OutDbgColor(12,"\n\nLog non sauvé car impossible de créer le fichier de log !\n\n");	
		}
	} else LogBuf[LogBufCount++] = Char;
}

void PutString2Log(char* Str)
{
	while(*Str != 0) PutChar2Log( *(Str++) );
}

// Ajoute un caratère dans le log.
// Si Couleur = 0xAA -> Couleur à ne pas prendre en compte.
void PutChar2HtmlLog(char Char)
{
	char HtmlColor[50];
	u8 Color = Attribut & 0x0F; // On retire au passage les attributs étendus.
	
	if (BufLogLastColor != Color)
	{
		if (BufLogLastColor != 0xFF) PutString2Log("</font>");
		BufLogLastColor = Color;
			
		Str_Fmt(HtmlColor,"<font color=\"%s\">",TMC2HC[Color]);
		PutString2Log(HtmlColor);
	}
	
	switch (Char)
	{
		case 10 :
			PutString2Log("<br>\n");
			break;
		
		case 32 :
			if (LastLogedChar == 32)
			{
				PutString2Log("&nbsp;");
				break;
			}
			
		default :
			PutChar2Log(Char);
		
	}
	
	LastLogedChar = Char;
}

// Active le logging.
u8 OutT_LogStart()
{
	u32 i;
	int c;
	
	if (WriteToLog) return 0;
	
	OutDbg("Activation du log...");
	
	LogBufCount = 0;
	BufLogLastColor = 0xFF;
	LastLogedChar = 0xFF;
	WriteToLog = true;
	LogFileCreated = false;
	
	// Ajout du début html.
	AddHtmlStart();
	
	c = 1;
	for (i = VideoMem; i < (VideoMem+(Y*LineSize)+(X*2)); i+=2)
	{
		Attribut = *((u8*) i+1);
		PutChar2HtmlLog( *((u8*) i) );
		
		if (c++ == NumChar)
		{
			c = 1;
			PutChar2HtmlLog(10);
		}
	}
	
	OutDbg("Écran récupéré.");
	
	return 0;
}

void OutT_LogStop()
{
	if (!WriteToLog)
	{
		OutDbg("Log non actif");
		return;
	}
	OutDbg("Arrêt du log...");
	
	if (LogBufCount)
	{
		if (!CreateLogFile())
		{
			PutString2Log("</font>\n</font>\n</body>\n</html>");
			WriteToLog = false;
			
			if (Fat12_WriteFile(&LogFile,LogBufCount,&LogBuf[0]))
			{	
				OutDbgColor(12,"Ah zut buffer du log écrit, erreur d'écriture...");
			}
			else
			{
				OutDbg("Buffer du log écrit.");
			}
			Fat12_CloseFile(&LogFile);
		}
	}
	
	WriteToLog = false;
}

// Récupère la ligne actuelle dans les datas BIOS.
void OutT_Init()
{
	Y = *((unsigned char*) (0x0451)); // Le curseur laissé par le bios.
	X = 0;
	
	WriteToLog = false;
	
	OutDbg("-- OutText Compilation DEBUG --");
	#ifdef DEBUG_OUTTEXT_OUTBOCHS
	OutDbg("-- OutText Compilation DEBUG_OUTTEXT_OUTBOCHS --");
	#endif
}

// Retourne la ligne actuelle dans les datas BIOS.
void OutT_Release()
{
	OutDbg("-- Release --");
	// On remet le curseur au niveau du bios.
	// On est des gens propres nous ! C'est juste utile pour les tests !
	*((unsigned char*) (0x0451)) = Y;
}

void OutT_SetAttr(unsigned char NewAttribut)
{
	Attribut = NewAttribut;
}

unsigned char OutT_GetAttr()
{
	return Attribut;
}

// Scroll une ligne.
static void
OutT_NextLn()
{
	short i;

	for (i = 0; i < (LineSize*NumLine); i = i+4)
	{
		*((unsigned int*) (VideoMem+i)) = *((unsigned int*) (VideoMem+i+LineSize));
	}

}

// Actualisation du curseur vidéo.
static void
OutT_UpdateVideoCursor()
{
	#define	CrtPort		0x03D4
	#define	RegCursorLow	0x0F
	#define	RegCursorHigh	0x0E

	u16 CurOffset = (Y*NumChar)+X;
	u16 Value;
	
	Value = (CurOffset << 8) + RegCursorLow;
	Outw(CrtPort, Value);
	
	Value = (CurOffset & 0xFF00) + RegCursorHigh;
	Outw(CrtPort, Value);
}

// Sort un caractère avec attributs.
void OutT_PutChar(unsigned char Char)
{
	#ifdef DEBUG_OUTTEXT_OUTBOCHS
	Outb(0xE9,Char); // Sortie dans Bochs.
	#endif
	if (WriteToLog) PutChar2HtmlLog(Char);

	switch (Char)
	{
		case 10 :
				X = 0;
				Y++;
				break;

		case 9	:
				{
					int Tab = X / 8;
					Tab++;
					Tab *= 8;
					if (Tab < NumChar)
					{
						int i;
						for (i = X; i < Tab; i++) OutT_PutChar(' ');
					}
				}
				break;

		default	:
				*((unsigned char*) (VideoMem+0+(Y*LineSize)+(X*2))) = Char;
				*((unsigned char*) (VideoMem+1+(Y*LineSize)+(X*2))) = Attribut;

				X++;
				if (X == NumChar)
				{
					X = 0;
					Y++;
				}
	}

	if (Y == NumLine)
	{
		OutT_NextLn();
		Y--;
	}
	
	OutT_UpdateVideoCursor();
}

// Sort un unsigned int en décimal.
void OutT_uInt(unsigned int Nb, u8 Zero)
{
	char str[10];
	int  posi = 0;

	do
	{
		str[posi] = ((Nb % 10) | 48);
		posi++;
	}
	while ((Nb = Nb / 10) > 0);
	
	for (;posi < Zero; Zero--) OutT_PutChar('0');
	for (posi--; posi >= 0; OutT_PutChar(str[posi--]) );
}

// Sort un int en décimal.
void OutT_Int(int Nb, u8 Zero)
{
	char str[10];
	int  posi = 0;
	char neg = (Nb < 0);

	if (neg) Nb = -Nb;
	do
	{
		str[posi] = ((Nb % 10) | 48);
		posi++;
	}
	while ((Nb = Nb / 10) > 0);
	
	if (neg) OutT_PutChar('-');
	for (;posi < Zero; Zero--) OutT_PutChar('0');
	for (posi--; posi >= 0; OutT_PutChar(str[posi--]) );
}

// Sort un int en hexadécimal.
void OutT_Hexa(unsigned int Nb)
{
	char str[9];
	int posi = 0;
	unsigned char demiu8;

	do
	{
		demiu8 = (Nb & 0x0000000F);
		Nb = Nb >> 4;
		if (demiu8 < 0xA) demiu8 |= 48; else demiu8 += (65-10);
		str[posi++] = demiu8;
	} while (Nb > 0);

	OutT_Format("0x",0);
	for (posi-- ; posi >= 0; posi--) OutT_PutChar(str[posi]);
}

void OutT_Bin(unsigned int Nb, char NbOctets)
{
	int i, ii = 0;

	NbOctets *= 8;
	Nb = Nb << (32 - NbOctets);
	for (i = 0; i < NbOctets; i++)
	{
		if (ii == 4)
		{
			OutT_PutChar(' ');
			ii = 1;
		}
		else ii++;
		OutT_PutChar( ((Nb & 0x80000000) > 0) ? '1' : '0');
		Nb = Nb << 1;
	}
}

// Sort une chaîne formattée à l'écran avec les params dans une chaîne.
// %u  : unsigned int.
// %i  : int.
// %U? : unsigned int. ? = Le nombre de zero.
// %I? : int.		   ? = Le nombre de zero.
// %s  : ptrString.
// %h  : hexa.
// %b? : binaire. ? = 1 pour un octets, 2 pour deux octets etc. Max = 4.
// %c  : char.
// *params est la liste des paramètres %?.
void OutT_Format(unsigned char *str, void *params)
{
#define NextParam(ParamsPtr)  (((void*) ParamsPtr) += 4, *((int*) (ParamsPtr-4)))

	while (*str != 0)
	{
		if (*str == '%')
		{
			switch(str[1])
			{
				char *string;
				
				case 'u' :
					OutT_uInt(NextParam(params),0);
					break;
				
				case 'i' :
					OutT_Int(NextParam(params),0);
					break;
				
				case 'U' :
					OutT_uInt(NextParam(params),str[2]-48);
					str++;
					break;
				
				case 'I' :
					OutT_Int(NextParam(params),str[2]-48);
					str++;
					break;					
										
				case 's' :
					string = (char*) NextParam(params);
					while (*string != 0) OutT_PutChar(*string++);
					break;
				
				case 'h' :
					OutT_Hexa(NextParam(params));
					break;
				
				case 'b' :
					OutT_Bin(NextParam(params),str[2]-48);
					str++;
					break;
				
				case 'c' :
					OutT_PutChar(NextParam(params));
					break;
				
				default :
				{
					OutT_PutChar(*str);
					str--;
				}
			}
			str++;

		} else OutT_PutChar(*str);

		str++;
	}
}

// Sort une chaîne formattée à l'écran avec paramètres infini.
// Utilise OutT_Format pour afficher la chaîne formatée.
void OutT(unsigned char *str, ...)
{
	Arg_List Args;
	Arg_Init(Args, str);
	OutT_Format(str,Args);
}

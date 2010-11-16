/*
	Ficher include copié automatiquement.
	Seul le fichier original doit être modifié.
*/

/*
 *	Pilote de l'horloge.
 *
 *	Bombela.
 *	01/02/2005
 *	02/02/2005
 */

typedef struct
{
	u32	Year;
	u8	Month,Day;
} TDate_Date;

typedef struct
{
	u8	Hour,Min,Sec;	
} TDate_Hour;

void Date_GetCurrentDate(TDate_Date *Date);
void Date_GetCurrentHour(TDate_Hour *Hour);

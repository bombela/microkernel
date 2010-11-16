/*
 *	Pilote de l'horloge.
 *
 *	Bombela.
 *	01/02/2005
 *	02/02/2005
 *	04/10/2005 - Correction d'un vieux bug tout moisi et ajout d'un double boucle
				pour plus de sureté.
 */

#include <Debug_Start.h>
#ifdef DEBUG_FAT12
#define DEBUG_NAME "FAT12   "
#endif
#include <Debug_End.h>

#include <Types.h>
#include <Date.h>
#include <Io.h>

#include "Date_Define.h"
// -----------------------------------------

// Les valeurs RTC sont codé avec un chiffre pour 4 bits.

u8 GetValue(Index)
{
	u8 Value;

	Outb(RTC_Index,Index);
	Value = Inb(RTC_Data);
	return (((Value >> 4) * 10) + (Value & 0x0F));
}

void WaitAvailable()
{
	Outb(RTC_Index,RTC_status_register_A);
	while (Inb(RTC_Data) & 0x80); // Deux boucle pour le cas où
	while (Inb(RTC_Data) & 0x80); // on serait tombé au bord de la première !
}

void Date_GetCurrentDate(TDate_Date *Date)
{
	WaitAvailable();
	Date->Year	= GetValue(RTC_year) + 2000;
	Date->Month	= GetValue(RTC_month);
	Date->Day	= GetValue(RTC_day_of_month);
}

void Date_GetCurrentHour(TDate_Hour *Hour)
{
	WaitAvailable();
	Hour->Hour	= GetValue(RTC_hours);
	Hour->Min	= GetValue(RTC_minutes);
	Hour->Sec	= GetValue(RTC_seconds);
}

/*
	Ficher include copié automatiquement.
	Seul le fichier original doit être modifié.
*/

/*
 *	Pilote timer.
 *
 *	Bombela.
 *	03/07/2004
 *	24/10/2004
 */


char Timer_Init();
void Timer_SleepU(u32 MicroSecondes);
void Timer_SleepM(u32 MilliSecondes);
void Timer_SleepM55(u32 Multiple); // 18,5 ~= une seconde.

/*
 *	Pilote timer.
 *
 *	Bombela.
 *	03/07/2004
 *	24/10/2004
 *	29/01/2006
 */

#include <Types.h>

char Timer_Init();
void Timer_SleepU(u32 MicroSecondes);
void Timer_SleepM(u32 MilliSecondes);
void Timer_SleepM55(u32 Multiple); // 18,5 ~= une seconde.
u32 Timer_CalcCpuSpeed();
u32 Timer_GetCpuSpeed();

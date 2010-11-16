/*
	Ficher include copié automatiquement.
	Seul le fichier original doit être modifié.
*/

/*
 *	Pilote Pic.
 *
 *	Bombela.
 *	30/09/2004
 *	30/09/2004
 */

char Pic_Init();
char Pic_IrqMask(u8 NumIrq, boolean Active);
char Pic_IrqToInt(u8 Irq);
char Pic_EndIrq(u8 Irq);
char Pic_Release();

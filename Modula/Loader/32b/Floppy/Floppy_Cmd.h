/*
 *	Petit pilote floppy assurant seulement l'ecriture.
 * 	Définitions locale.
 *
 *	Bombela.
 *	29/09/2004
 *	24/10/2004
 */

#ifndef include_floppy_cmd
#define include_floppy_cmd

#define FDC_Dor					0x3F2
#define FDC_MainStatus			0x3F4
#define FDC_Data					0x3F5
#define FDC_Dir					0x3F7
#define FDC_Config				0x3F7

// Main status
#define FDC_MS_Mrq				0x80
#define FDC_MS_Dio				0x40
#define FDC_MS_Ndma				0x20
#define FDC_MS_Busy				0x10
#define FDC_MS_ActD				0x08
#define FDC_MS_ActC				0x04
#define FDC_MS_ActB				0x02
#define FDC_MS_ActA				0x01

// Main status ST0

// Main status ST1

// Main status ST2

// Main status ST3

// Commands

#define CMD_SPECIFY (0x03)  /* specify drive timings */
#define CMD_WRITE   (0xc5)  /* write data (+ MT,MFM) */
#define CMD_READ    (0xe6)  /* read data (+ MT,MFM,SK) */
#define CMD_RECAL   (0x07)  /* recalibrate */
#define CMD_CHECKINT (0x08)  /* sense interrupt status */
#define CMD_FORMAT  (0x4d)  /* format track (+ MFM) */
#define CMD_SEEK    (0x0f)  /* seek track */
#define CMD_VERSION (0x10)  /* FDC version */

#endif

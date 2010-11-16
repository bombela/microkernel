/*
 *  Déclarations ATA.
 *
 *	Bombela.
 *	31/08/2004
 *	31/08/2004
 */

// Registres des controleurs ATA.
#define ATA_DATA                        0x00    /* (R/W) Data register */

#define ATA_ERROR                       0x01    /* (R) error register */

#define ATA_PRECOMP                     0x01    /* (W) precompensation */

#define ATA_SECTOR_COUNT                0x02    /* (W) sector count register */

#define ATA_LBA_0					0x03    /* (W) sector number register */
#define ATA_LBA_1                    	0x04    /* (W) cylinder# LSB */
#define ATA_LBA_2                     	0x05    /* (W) cylinder# MSB */
#define ATA_LBA_3                     	0x06    /* (W) cylinder# MSB */

#define ATA_DRIVE                       0x06    /* (W) drive/head/lba */
#define         ATA_D_IBM               0xa0    /* bits that must be set */
#define         ATA_D_LBA               0x40 | ATA_D_IBM    /* use LBA ? */
#define         ATA_D_MASTER            0x00 | ATA_D_IBM    /* select master */
#define         ATA_D_SLAVE             0x10 | ATA_D_IBM    /* select slave */


#define ATA_STATUS                      0x07    /* (R) status register */
#define         ATA_S_ERROR             0x01    /* error */
#define         ATA_S_INDEX             0x02    /* index */
#define         ATA_S_CORR              0x04    /* data corrected */
#define         ATA_S_DRQ               0x08    /* data request */
#define         ATA_S_DSC               0x10    /* drive Seek Completed */
#define         ATA_S_DWF               0x20    /* drive write fault */
#define         ATA_S_DF				0x20	// Device fault.
#define         ATA_S_DRDY              0x40    /* drive ready */
#define         ATA_S_BSY               0x80    /* busy */

#define ATA_CMD                         0x07    /* (W) command register */
#define         ATA_C_ATA_IDENTIFY      0xec    /* get ATA params */
#define         ATA_C_ATAPI_IDENTIFY    0xa1    /* get ATAPI params*/
#define         ATA_C_READ              0x20    /* read command */
#define         ATA_C_WRITE             0x30    /* write command */
#define         ATA_C_READ_MULTI        0xc4    /* read multi command */
#define         ATA_C_WRITE_MULTI       0xc5    /* write multi command */
#define         ATA_C_SET_MULTI         0xc6    /* set multi size command */
#define         ATA_C_PACKET_CMD        0xa0    /* set multi size command */
#define			ATA_C_DEVDIAG			0x90	// Device diagnostic.

#define ATA_ALTPORT                     0x206   /* (R) alternate
                                                   Status register */

#define ATA_DEVICE_CONTROL              0x206   /* (W) device control
                                                   register */
#define         ATA_A_nIEN              0x02    /* disable interrupts */
#define         ATA_A_RESET             0x04    /* RESET controller */
#define         ATA_A_4BIT              0x08    /* 4 head bits */

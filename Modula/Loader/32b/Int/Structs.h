/*
 *	Pilote Int.
 *
 *	Bombela.
 *	30/07/2004
 *	01/10/2004
 */

#pragma pack(1) // Stop l'alignement.

// Element d' IDT.
typedef struct
{
	u16 Offset_Low;
	u16 Selecteur;
	u16 Type;
	u16 Offset_High;
} __attribute__((__packed__)) IDT_Item;


// PtrIDT, pour le reg IDTR.
typedef struct
{
	u16 Limite;
	void* Base;
} __attribute__ ((__packed__)) IDT_ptrIDTR;

#pragma pack() // Rétablit l'alignement par défaut.

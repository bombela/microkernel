/*
	Ficher include copié automatiquement.
	Seul le fichier original doit être modifié.
*/


// Macros

#define MEM_ALIGN_INF(val) \
	(((unsigned)(val)) & (~((4096)-1)))

#define MEM_ALIGN_SUP(val) \
	({ unsigned int __bnd=(4096); \
	(((((unsigned)(val))-1) & (~(__bnd - 1))) + __bnd); })

#define MEM_IS_ALIGNED(val) \
	( 0 == (((unsigned)(val)) & ((4096)-1)) )

// Fonctions

u8 Mem_Init();
u8 Mem_Alloc_Adress(unsigned int Adress, unsigned int MaxAllocation);
void* Mem_Alloc();
u8 Mem_Free_Adress(unsigned int Adress);


// Gestion m�moire.

// Bombela - 18/03/2006
// Bombela - 19/03/2006

#include <Debug_Start.h>
#ifdef DEBUG_MEM
#define DEBUG_NAME "MEM     "
#endif
#include <Debug_End.h>

// Includes

#include <macro_list.h>
#include <Mem.h>

// Structures

struct Physical_Memory_Page
{
	void *Adress;
	unsigned int RefCount;
	struct Physical_Memory_Page *prev, *next;
};

// Variables

static unsigned int MemorySize;
static struct Physical_Memory_Page *Pages, *FreePages, *UsedPages;
static unsigned int PageCount;

// #####################################################################
// Calcul de la taille de la ram.
// #####################################################################
static
unsigned int CalcMemorySize()
{
	unsigned int i;
	volatile u32 *u32mem;
	u32 max = 0xFFFFFFFF; // 32 bits
	
	for (i = (1024*1024); i < max-(1024*1024); i+=(1024*1024/4))
	{
		u32mem = (u32*) i;
		*u32mem = 0xAAFF55FF;
		if (*u32mem != 0xAAFF55FF) break;
	}
	
	return i;
}

// #####################################################################
// Alloue une page.
// Renvoie une adresse si l'allocation � r�ussie, NULL si erreur.
// #####################################################################
void* Mem_Alloc()
{	
	if (list_is_empty(FreePages))
	{
		OutDbg("Mem_Alloc. Heu... y a plus de m�moire !");
		return NULL;
	}
	
	struct Physical_Memory_Page *Page = list_pop_head(FreePages);

	Page->RefCount++;

	list_add_tail(UsedPages, Page);

	OutDbg("Mem_Alloc -> Et une page � l'adress %h !",Page->Adress);
	
	return (void*) Page->Adress;
}

// #####################################################################
// D�cr�mente le compteur d'une page acc�d�e par son adresse,
// et la lib�re si il atteint 0.
// Renvoie 0 si d�salou�e ou d�cr�ment�e.
// Renvoie 1 si l'adresse correspond � une page libre.
// Renvoie 2 si l'adresse n'est pas align�e.
// Renvoie 3 si l'adresse est NULL.
// #####################################################################
u8 Mem_Free_Adress(unsigned int Adress)
{	
	if (Adress == 0)
	{
		OutDbg("Mem_Free_Adress : Adresse NULL !");
		return 0;
	}
	
	if (!MEM_IS_ALIGNED(Adress))
	{
		OutDbg("Mem_Free_Adress : Adresse %h non align�e sur 4096 !",Adress);
		return 2; // Adresse pas sur 4096.
	}
	
	// Le -1, c'est pour la premi�re page physique � zero.
	struct Physical_Memory_Page *Page = &Pages[(Adress / 4096) -1];

	if (Page->RefCount == 0)
	{
		OutDbg("Mem_Free_Adress : La page %u qui correspond � l'adresse %h n'est pas allou�e.",
				(Adress / 4096) -1,Adress,Page->RefCount);
		return 1;
	}

	// La page est lib�r�e.
	if (--Page->RefCount == 0)
	{
		OutDbg("Mem_Free_Adress : La page d'adresse %h est lib�r�e.",Adress);
		list_delete(UsedPages,Page);
		list_add_head(FreePages,Page);
	} else OutDbg("Mem_Free_Adress : La page d'adresse %h devient r�f�renc�e %u fois.",Adress,Page->RefCount);
	return 0;
}

// #####################################################################
// Alloue une page par son adresse.
// Retourne 0 si ok.
// Retourne 1 si la page est d�j� allou�e plus que demandé par MaxAllocation.
// Retourne 2 si l'adresse n'est pas align�e.
// Retourne 3 si l'adresse est NULL.
// #####################################################################
u8 Mem_Alloc_Adress(unsigned int Adress, unsigned int MaxAllocation)
{
	if (Adress == 0)
	{
		OutDbg("Mem_Alloc_Adress : Adresse NULL !");
		return 0;
	}
	
	if (!MEM_IS_ALIGNED(Adress))
	{
		OutDbg("Mem_Alloc_Adress : Adresse %h non align�e sur 4096 !",Adress);
		return 2; // Adresse pas sur 4096.
	}
	
	// Le -1, c'est pour la premi�re page physique � zero.
	struct Physical_Memory_Page *Page = &Pages[(Adress / 4096) -1];

	if (Page->RefCount > MaxAllocation)
	{
		OutDbg("Mem_Alloc_Adress : La page %u qui correspond � l'adresse %h est d�j� allou�e %u fois \
				(%u demand� max).",(Adress / 4096) -1,Adress,Page->RefCount,MaxAllocation);
		return 1; // La page est d�j� allou�e plus que demand�.
	}

	// La page est allou�e.
	list_delete(FreePages,Page);
	list_add_tail(UsedPages,Page);
	Page->RefCount++; // � ne pas oublier ;)
	return 0;
}

// #####################################################################
// Initialisation !
// #####################################################################
u8 Mem_Init()
{
	unsigned int i;
	
	OutDbg("Initialisation...");
	
	OutDbg("Premi�re �tape, on d�tecte la taille de la ram...");

	MemorySize = CalcMemorySize();
	OutDbg("Taille de la ram : %u Mo.",MemorySize / 1024 / 1024);

	PageCount = (MemorySize / 4096) - 4096; // Le nombres de pages.
	OutDbg("Ce qui nous fait %u pages de 4ko (On utilise pas la page 0).",PageCount);

	OutDbg("Cr�ation du tableau des pages.");
	Pages = (struct Physical_Memory_Page*) (1024*1024); // On place donc � 1 mo ce tableau.
	
	OutDbg("Cr�ation des liste doublement cha�n�e circulaire...");
	list_init(FreePages);
	for (i=0; i < PageCount; i++)
	{
		list_add_tail(FreePages,&Pages[i]);
		Pages[i].Adress = (void*) ((i+1) * 4096);
		Pages[i].RefCount = 0;
	}
	list_init(UsedPages);
	
	OutDbg("Le tableau de page prend %u pages.",
			(MEM_ALIGN_SUP(PageCount * sizeof(struct Physical_Memory_Page)) / 4096) );
	
	OutDbg("Allocation du tableau de page lui m�me.");
	for (i = 0; i < (MEM_ALIGN_SUP(PageCount * sizeof(struct Physical_Memory_Page)) / 4096); i++)
	{
		u8 res;
		unsigned int Adress;

		Adress = ((unsigned int) Pages) + (i * 4096);
		
		res = Mem_Alloc_Adress(Adress,0); // Allocation !
			
		if (res > 0)
		{
			OutDbg("Erreur n� %u d'allocation : %h.",res,Adress);
			return 1;
		}
	}
	
	return 0;
}

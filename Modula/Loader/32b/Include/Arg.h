// Arg, définitions pour la récupération des arguments dans la pile.

/* Exemple:
// Fonction qui fais le cumul des ints.
// Num indique le nombre d'int.
int CumulInt(int Num, ...)
{
	Arg_List Args:
	int i, ret = 0;

	Arg_Init(Arg_List, Num);
	for ( ; i < Num; i++) ret += Arg_Get(Args,int);

	return ret;
}
*/

// Bombela
// 29/06/2004

#ifndef include_arg
#define include_arg

typedef void* Arg_List;

#define __va_rounded_size(TYPE) (((sizeof (TYPE) + sizeof (int) - 1) / sizeof (int)) * sizeof (int))

#define Arg_Init(AP, LASTARG) (AP = ((Arg_List) __builtin_next_arg (LASTARG)))
#define Arg_Get(AP, TYPE)  (AP = (Arg_List) ((char *) (AP) + __va_rounded_size (TYPE)),	*((TYPE *) (void *) ((char *) (AP) - __va_rounded_size (TYPE))))

#endif

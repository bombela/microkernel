/*
 *	Define DEBUG.
 *
 *	Bombela.
 *	20/11/2004
 *	20/11/2004
 */

#include <OutText.h>

// Macro qui sort une cha�ne de debug.
// Si pas de debug, le code est ignor�.
// OutDbg sort avec l'ent�te DEBUG_NAME.
// OutDbgColor fait pareil avec de la couleur.
// -------------------------------------------
#define OutDbg(str, args...)
#define OutDbgColor(couleur, str, args...)
#ifdef DEBUG_NAME
#undef OutDbg
#undef OutDbgColor
#define OutDbg(str, args...) \
	{ \
		char SauvAttr = OutT_GetAttr(); \
		OutT_SetAttr(7); \
		OutT("\n ["DEBUG_NAME"] : "str, ## args); \
		OutT_SetAttr(SauvAttr); \
	}
#define OutDbgColor(couleur, str, args...) \
	{ \
		char SauvAttr = OutT_GetAttr(); \
		OutT_SetAttr(couleur); \
		OutT("\n ["DEBUG_NAME"] : "str, ## args); \
		OutT_SetAttr(SauvAttr); \
	}
#endif

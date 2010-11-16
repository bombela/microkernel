// Fonctions utiles dans le tra�tement des cha�nes de caract�res.

// Bombela
// Created: 15/04/2005
// 19/04/2005
// 03/10/2005 - Renommage des fonctions de magni�re plus logique.

#include <Types.h>
#include <Arg.h>

// ------------------------------------------------------------------------------------------------
// Comparaison de deux cha�nes.
// MaxChar indique la limite de recherche.
// Ne s'arr�te pas � un 0 !
boolean Str_nzCmp(char *A, char *B,u32 MaxChar)
{
	while (MaxChar > 0)
		if (A[--MaxChar] != B[MaxChar]) return false;
	return true;
}

// ------------------------------------------------------------------------------------------------
// Comparaison de deux cha�nes de texte ASCIIZ.
// MaxChar permet de limiter la recherche.
// La comparaison s'arr�te � la rencontre d'une fin de cha�ne.
boolean Str_Cmp(char *A, char *B,u32 MaxChar)
{
	int i = 0;
	while ( !((A[i] == 0) || (B[i] == 0)) && (i < MaxChar) ) if (A[i] != B[i++]) return false;
	if ((i == MaxChar) || ((A[i] == 0) && (B[i] == 0))) return true;
	return false;
}

// ------------------------------------------------------------------------------------------------
// Copie une cha�ne jusqu'au carat�re choisi.
// Renvoi le nombre d'octets copi�.
int Str_nzCpCmp(char *stout, char *stin, char comparator)
{
	int i = 0;
	while ((*stout++ = *stin++) != comparator) i++;
	return i;
}

// ------------------------------------------------------------------------------------------------
// Copie une cha�ne jusqu'au carat�re choisi.
// S'arr�te � la rencontre d'une fin de cha�ne.
// Renvoi le nombre d'octets copi�.
int Str_CpCmp(char *stout, char *stin, char comparator)
{
	int i = 0;
	while ((*stout++ = *stin++) != comparator)
	{
		i++;
		if ((stin == 0) || (stout == 0)) return i;
	}
	return i;
}

// ------------------------------------------------------------------------------------------------
// Copie une cha�ne rapidement.
// S'arr�te � la rencontre d'une fin de cha�ne.
void Str_Cp(char *stout, char *stin)
{
	while ((*stout++ = *stin++) != 0);
}

// ------------------------------------------------------------------------------------------------
// Copie une cha�ne.
// S'arr�te � la rencontre d'une fin de cha�ne.
// Renvoit la taille copi�e.
int Str_sCp(char *stout, char *stin)
{
	int i = 0;
	while ((*stout++ = *stin++) != 0) i++;
	return i;
}

// ------------------------------------------------------------------------------------------------
// Sort un unsigned int en d�cimal.
// Renvoi le nombre d'octets copi�.
int Str_uInt(unsigned char *stout,unsigned int Nb, u8 Zero)
{
	char str[10];
	int  posi = 0;
	int i;

	do
	{
		str[posi] = ((Nb % 10) | 48);
		posi++;
	}
	while ((Nb = Nb / 10) > 0);
	
	i = posi;
	for (;posi < Zero; Zero--, i++) *(stout++) = '0';
	for (posi--; posi >= 0; *(stout++) = str[posi--]);
	return i;
}

// ------------------------------------------------------------------------------------------------
// Sort un int en d�cimal.
// Renvoi le nombre d'octets copi�.
int Str_Int(unsigned char *stout,int Nb, u8 Zero)
{
	char str[10];
	int  posi = 0;
	char neg = (Nb < 0);
	int i;

	if (neg) Nb = -Nb;
	do
	{
		str[posi] = ((Nb % 10) | 48);
		posi++;
	}
	while ((Nb = Nb / 10) > 0);
	
	i = posi;	
	if (neg)
	{
		 *(stout++) = '-';
		 i++;
	}
	for (;posi < Zero; Zero--, i++) *(stout++) = '0';
	for (posi--; posi >= 0; *(stout++) = str[posi--]);
	return i;
}

// ------------------------------------------------------------------------------------------------
// Sort un int en hexad�cimal.
// Renvoi le nombre d'octets copi�.
int Str_Hexa(unsigned char *stout,unsigned int Nb)
{
	char str[9];
	int posi = 0;
	unsigned char demiu8;
	int i;

	do
	{
		demiu8 = (Nb & 0x0000000F);
		Nb = Nb >> 4;
		if (demiu8 < 0xA) demiu8 |= 48; else demiu8 += (65-10);
		str[posi++] = demiu8;
	} while (Nb > 0);

	i = posi + 2;
	*(stout++) = '0';
	*(stout++) = 'x';
	for (posi-- ; posi >= 0; *(stout++) = str[posi--]);
	return i;
}

// ------------------------------------------------------------------------------------------------
// Sort un unsigned int en binaire.
// Renvoi le nombre d'octets copi�.
int Str_Bin(unsigned char *stout,unsigned int Nb, char NbOctets)
{
	int i, ii = 0;

	NbOctets *= 8;
	Nb = Nb << (32 - NbOctets);
	for (i = 0; i < NbOctets; i++)
	{
		if (ii == 4)
		{
			*(stout++) = ' ';
			ii = 1;
		}
		else ii++;
		*(stout++) = (((Nb & 0x80000000) > 0) ? '1' : '0');
		Nb = Nb << 1;
	}
	return NbOctets+1;
}

// ------------------------------------------------------------------------------------------------
// Sort une cha�ne formatt�e dans une cha�ne.
// %u  : unsigned int.
// %i  : int.
// %U? : unsigned int. ? = Le nombre de zero.
// %I? : int.		   ? = Le nombre de zero.
// %s  : ptrString.
// %h  : hexa.
// %b? : binaire. ? = 1 pour un octets, 2 pour deux octets etc. Max = 4.
// %c  : char.
// *params est un tableau non align� des param�tres correspondant � chaque %.
// Renvoi le nombre d'octets copi�.
int Str_tFmt(char *stout, char *str, void *params)
{
#define NextParam(ParamsPtr)  (((void*) ParamsPtr) += 4, *((int*) (ParamsPtr-4)))

	char *origin = stout;

	while (*str != 0)
	{
		if (*str == '%')
		{
			switch(str[1])
			{
				case 'u' :
					stout += Str_uInt(stout,NextParam(params),0);
					break;
					
				case 'i' :
					stout += Str_Int(stout,NextParam(params),0);
					break;
					
				case 'U' :
					stout += Str_uInt(stout,NextParam(params),str[2]-48);
					str++;
					break;
					
				case 'I' :
					stout += Str_Int(stout,NextParam(params),str[2]-48);
					str++;
					break;					
					
				case 's' :
					stout += Str_sCp(stout,(char*) NextParam(params));					
					break;
					
				case 'h' :
					stout += Str_Hexa(stout,NextParam(params));
					break;
					
				case 'b' :
					stout += Str_Bin(stout,NextParam(params),str[2]-48);
					str++;
					break;
					
				case 'c' :
					*stout++ = NextParam(params);
					break;
					
				default :
				{
					*stout++ = --*str;
				}
			}
			str++;
			
		}
		else *stout++ = *str;
		
		str++;
	}
	*stout++ = 0;
	return ((int) stout - (int) origin);
}

// ------------------------------------------------------------------------------------------------
// G�n�re une cha�ne format�e.
int Str_Fmt(char *stout, char *str, ...)
{
	Arg_List Args;
	Arg_Init(Args, str);
	return Str_tFmt(stout,str,Args);
}

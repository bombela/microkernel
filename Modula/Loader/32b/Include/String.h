/*
	Ficher include copié automatiquement.
	Seul le fichier original doit être modifié.
*/

// Fonctions utiles dans le traîtement des chaînes de caractères.

// Bombela
// Created: 15/04/2005
// Updated: 19/04/2005

#ifndef include_string
#define include_string

boolean Str_nzCmp(char *A, char *B,u32 MaxChar);
boolean Str_Cmp(char *A, char *B,u32 MaxChar);
int Str_nzCpCmp(char *stout, char *stin, char comparator);
int Str_CpCmp(char *stout, char *stin, char comparator);
void Str_Cp(char *stout, char *stin);
int Str_sCp(char *stout, char *stin);

int Str_uInt(unsigned char *stout,unsigned int Nb, u8 Zero);
int Str_Int(unsigned char *stout,int Nb, u8 Zero);
int Str_Hexa(unsigned char *stout,unsigned int Nb);
int Str_Bin(unsigned char *stout,unsigned int Nb, char NbOctets);

int Str_tFmt(char *stout, char *str, void *params);
int Str_Fmt(char *stout, char *str, ...);

#endif

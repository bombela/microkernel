/*
	Ficher include copi� automatiquement.
	Seul le fichier original doit �tre modifi�.
*/

// Fonctions utiles dans le tra�tement des cha�nes de caract�res.

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

int Str_tFmt(unsigned char *stout, unsigned char *str, void *params);
int Str_Fmt(unsigned char *stout, unsigned char *str, ...);

#endif

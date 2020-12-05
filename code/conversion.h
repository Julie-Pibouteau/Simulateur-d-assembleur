#ifndef CONVERSION_H
#define CONVERSION_H
#include <stdio.h>


/* PARTIE 1 : assembleur --> hexa */
/* assemble et convertit le code opératoire et les différentes opérandes en une instruction en hexa
à l'appel de la fonction */
void generateur_hexa(int code_op, int rd, int rs, int imm, int src2, char* hexa);



/* PARTIE 2 : hexa --> execution */
/* convertit une case mémoire en binaire et inscrit le resultat (les 8 bits correspondants) dans un tableau de int passé en argument */
void conv_octet_memoire_vers_tab_bin(char* p_mem, int bin[]);

/* convertit une instruction en mémoire (4 cases mémoire) en binaire et inscrit le resultat (les 32 bits correspondants) dans un tableau de int passé en argument */
void conv_instr_memoire_vers_tab_bin(char* PCMem, int bin[]);

/* renvoit l'entier correspondant aux nb_bits premiers bits stockés dans bin[] */
int conv_bin_vers_int(int nb_bits, int bin[]);

/* remplit le code opératoire et les opérandes passées en arguments selon l'instruction binaire également passée en argument */
void conv_bin_vers_operandes(int bin[], int* code_op, int* dest, int* src, int* imm, int* scr2);



#endif

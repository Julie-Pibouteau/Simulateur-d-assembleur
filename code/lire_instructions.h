#ifndef LIRE_INSTRUCTIONS_H
#define LIRE_INSTRUCTIONS_H
#include "etiquette.h"
typedef struct {
  int code_op;
  int dest;
  int src1;
  int imm;
  int src2_valeur;
  int code_erreur;
  char* message_erreur;
} instruction;

/*
signification des codes d’erreurs :
0 : pas d’erreur
1 : mot_cle_op non reconnu
2 : nb d’opérandes insuffisant
3 : nb d’opérandes trop important
4 : opérande avec une syntaxe incorrecte
*/

// vérifier que la syntaxe du registre de destination est correcte
instruction* registre_incorrect(instruction* structure_instruction, char* dest);

// vérifier que la syntaxe de src1 est correcte
instruction* src1_incorrecte(instruction* structure_instruction, char* src1);

// vérifier que la syntaxe de src2 est correcte
instruction* src2_incorrecte(instruction* structure_instruction, char* src2);

instruction* lire_instructions(char* ligne, char Tab_operations[32][4], etiquette** Tab_Etiquette, instruction* structure_instruction);

// traiter les instructions arithmétiques et logiques et aléatoires et transferts
instruction* lire_IAL_IT(char* ligne, instruction* structure_instruction );

//lire les instructions de sauts
instruction* lire_IS(char* ligne, instruction* structure_instruction, etiquette** Tab_Etiquette);

// lire les entrées/sorties
instruction* lire_IES(char* ligne, instruction* structure_instruction);

#endif

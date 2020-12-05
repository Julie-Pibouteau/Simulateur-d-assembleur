#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "lire_instructions.h"
#include "etiquette.h" // E = 100 max et I = 200

// définition des fonctions de gestion d'erreurs de lecture

instruction* registre_incorrect(instruction* structure_instruction, char* dest){
  // gestion des erreurs : syntaxe de dest
  if (dest[0]!='r'){
    strcpy(structure_instruction->message_erreur,"Le registre de destination est de la forme rn.");
    structure_instruction->code_erreur = 4;
    return structure_instruction;
  }

  // on utilise la fonction src2 pour vérifier la syntaxe de dest pour éviter de dupliquer du code car ce sont les mêmes vérifications
  //s'il y a des erreurs
  if (src2_incorrecte(structure_instruction, dest) != NULL){
    //on change le message d'erreur
    strcpy(structure_instruction->message_erreur,"Le registre de destination va de r0 a r31.");
    return structure_instruction;
  }
  else{
    // si pas d'erreurs, on stocke dest et on retourne NULL
    sscanf(dest, "r %d", &structure_instruction->dest);
    return NULL;
  }
}

instruction* src1_incorrecte(instruction* structure_instruction, char* src1){
  // gestion des erreurs : syntaxe de src1
  if (src1[0]!='r'){
    strcpy(structure_instruction->message_erreur,"Le registre de src1 est de la forme rn.");
    structure_instruction->code_erreur = 4;
    return structure_instruction;
  }

  // on utilise la fonction src2 pour vérifier la syntaxe de src1 pour éviter de dupliquer du code car ce sont les mêmes vérifications
  //s'il y a des erreurs
  if (src2_incorrecte(structure_instruction, src1) != NULL){
    //on change le message d'erreur
    strcpy(structure_instruction->message_erreur,"Le registre de src1 va de r0 a r31.");
    return structure_instruction;
  }
  else{
    // si pas d'erreurs, on stocke src1 et on retourne NULL
    sscanf(src1, "r %d", &structure_instruction->src1);
    return NULL;
  }
}


instruction* src2_incorrecte(instruction* structure_instruction, char* src2){
  // gestion des erreurs : syntaxe de src2
  if (src2[0] == 'r'){
    // si src2 est un registre
    structure_instruction->imm = 0;

    //gestion des erreurs : le numéro du registre est-il bien un entier
    for(int i = 1; i < strlen(src2); i++){
      // les codes ASCII des nombres de 0 à 9 sont les nombres de 48 à 57
      if (src2[i] < 48 || src2[i] > 57){
        strcpy(structure_instruction->message_erreur,"Le registre source 2 attend un numero entier positif.");
        structure_instruction->code_erreur = 4;
        return structure_instruction;
      }
    }

    sscanf(src2, "r%d", &(structure_instruction->src2_valeur));
    //gestion des erreurs : syntaxe du registre source 2 (valeur de num de registre entre 0 et 31)
    if((structure_instruction->src2_valeur < 0) || (structure_instruction->src2_valeur > 31)) {
      strcpy(structure_instruction->message_erreur,"Le registre source 2 va de r0 a r31.");
      structure_instruction->code_erreur = 4;
      return structure_instruction;
    }

    strcpy(structure_instruction->message_erreur,"Pas d'erreurs");
    return NULL;

  }

  else if (src2[0] == '#'){
    // si src2 est une valeur immédiate
    structure_instruction->imm = 1;
    if (src2[1] == 'h'){
      //si src2 est une valeur hexadécimale
      // gestion des erreurs : syntaxe de la valeur hexadécimale source 2
      for(int i = 2; i < strlen(src2); i++){
        // les valeurs acceptées sont, en ASCII, dans [48,57] ou [65,70] ou [97,102]
        if (src2[i] < 48 || (src2[i] > 57 && src2[i] < 65) || (src2[i] > 70 && src2[i] < 97) || (src2[i] > 102)){
          strcpy(structure_instruction->message_erreur,"La valeur source 2 attend un nombre compose d'entiers de 0 a 9 et de lettres de A a F.");
          structure_instruction->code_erreur = 4;
          return structure_instruction;
        }
      }
      sscanf(src2, "#h%x", &(structure_instruction->src2_valeur));
      // gestion des erreurs : si la valeur hexadécimale est trop grande ou trop petite

      // si c'est un saut, la valeur acceptée va de 0 à 65535
      if ((structure_instruction->code_op >= 16) && (structure_instruction->code_op <= 22)){
        if ((structure_instruction->src2_valeur > 65535) || (structure_instruction->src2_valeur < 0)){
          strcpy(structure_instruction->message_erreur,"La valeur source 2 pour un saut attend un nombre dans [0, 65535]");
          structure_instruction->code_erreur = 4;
          return structure_instruction;
        }
      }
      // dans les autres cas, src 2 est dans [−32768, 32767]
      else {
        if ((structure_instruction->src2_valeur > 32767) || (structure_instruction->src2_valeur < -32768)){
          strcpy(structure_instruction->message_erreur,"La valeur source 2 attend un nombre dans [−32768, 32767]");
          structure_instruction->code_erreur = 4;
          return structure_instruction;
        }
      }

      // si pas d'erreurs
      strcpy(structure_instruction->message_erreur,"Pas d'erreurs");
      return NULL;
    }
    else {
      // si src2 est une valeur décimale
      // gestion des erreurs : syntaxe de la valeur décimale source 2
      // attention : gérer le "-" potentiel !
      if ((src2[1] != '-') && (src2[1] < 48 || src2[1] > 57)) {
        strcpy(structure_instruction->message_erreur,"La source 2 attend un numero entier.");
        structure_instruction->code_erreur = 4;
        return structure_instruction;
      }
      for(int i = 2; i < strlen(src2); i++){
        if (src2[i] < 48 || src2[i] > 57) {
          strcpy(structure_instruction->message_erreur,"La source 2 attend un numero entier.");
          structure_instruction->code_erreur = 4;
          return structure_instruction;
        }
      }
      sscanf(src2, "#%d", &(structure_instruction->src2_valeur));
      // gestion des erreurs : si la valeur hexadécimale est trop grande ou trop petite

      // si c'est un saut, la valeur acceptée va de 0 à 65535
      if ((structure_instruction->code_op >= 16) && (structure_instruction->code_op <= 22)){
        if ((structure_instruction->src2_valeur > 65535) || (structure_instruction->src2_valeur < 0)){
          strcpy(structure_instruction->message_erreur,"La valeur source 2 pour un saut attend un nombre dans [0, 65535]");
          structure_instruction->code_erreur = 4;
          return structure_instruction;
        }
      }
      // dans les autres cas, src 2 est dans [−32768, 32767]
      else {
        if ((structure_instruction->src2_valeur > 32767) || (structure_instruction->src2_valeur < -32768)){
          strcpy(structure_instruction->message_erreur,"La valeur source 2 attend un nombre dans [−32768, 32767]");
          structure_instruction->code_erreur = 4;
          return structure_instruction;
        }
      }
      //si pas d'erreurs
      strcpy(structure_instruction->message_erreur,"Pas d'erreurs");
      return NULL;
    }
  }

  else {
    // si la src2 ne commence ni par r ni par #, c'est une erreur de syntaxe
    strcpy(structure_instruction->message_erreur,"La src2 n'est ni un registre ni une valeur immediate.");
    structure_instruction->code_erreur = 4;
    return structure_instruction;
  }
}


instruction* lire_IAL_IT(char* ligne, instruction* structure_instruction){
  char dest[25], src1[25], src2[25] = {'\0'}, val_surnumeraire[25] = {'\0'};
  // la chaine mot_cle_op est créée pour être utilisée dans le format mais on ne se servira pas de la valeur car on a déjà récupéré le code_op
  char mot_cle_op[4];
  int valeur_retour_lecture;

  // si l'instruction est arith/log ou un load ou un rnd
  if (((structure_instruction->code_op >= 0) && (structure_instruction->code_op <= 9)) || (structure_instruction->code_op ==30)){
    valeur_retour_lecture = sscanf(ligne, " %s %s %s %s %s ", mot_cle_op, dest, src1, src2, val_surnumeraire);
  }
  // si l'instruction est un store
  else {
    valeur_retour_lecture = sscanf(ligne, " %s %s %s %s  %s ", mot_cle_op, dest, src2, src1, val_surnumeraire);
  }

  // gestion des erreurs : trop peu d'opérandes
  if(valeur_retour_lecture < 4){
    strcpy(structure_instruction->message_erreur,"Nombre d'operandes insuffisant, l'operation necessite 3 operandes.");
    structure_instruction->code_erreur = 2;
    return structure_instruction;
  }

  //gestion des erreurs : trop d'opérandes
  if(val_surnumeraire[0] != '\0'){
    strcpy(structure_instruction->message_erreur,"Nombre d'operandes trop important, l'operation necessite 3 operandes.");
    structure_instruction->code_erreur = 3;
    return structure_instruction;

  }

  // gestion des erreurs : syntaxe

  // si la fonction retourne autre chose que NULL, alors on retourne immédiatement la structure car il y a eu une erreur de syntaxe
  if (registre_incorrect(structure_instruction, dest) != NULL) return structure_instruction;
  if (src1_incorrecte(structure_instruction, src1) != NULL) return structure_instruction;
  if (src2_incorrecte(structure_instruction, src2) != NULL) return structure_instruction;

  // si on arrive ici, c'est qu'il n'y a pas eu d'erreurs de syntaxe donc on retourne une structure correctement remplie
  return structure_instruction;
}




instruction* lire_IS(char* ligne, instruction* structure_instruction, etiquette** Tab_Etiquette){
  // les bits des champs Dest et Src1 ne sont pas significatifs pour les sauts
  char src2[25] = {'\0'}, val_surnumeraire[25] = {'\0'};
  // la chaine mot_cle_op est créée pour être utilisée dans le format mais on ne se servira pas de la valeur car on a déjà récupéré le code_op
  char mot_cle_op[4];

  int valeur_retour_lecture = sscanf(ligne, " %s %s %s ", mot_cle_op, src2, val_surnumeraire);

  // gestion des erreurs : trop peu d'opérandes
  if(valeur_retour_lecture < 2){
    strcpy(structure_instruction->message_erreur,"Nombre d'operandes insuffisant, l'operation necessite 1 operande.");
    structure_instruction->code_erreur = 2;
    return structure_instruction;
  }

  //gestion des erreurs : trop d'opérandes
  if(val_surnumeraire[0] != '\0'){
    strcpy(structure_instruction->message_erreur,"Nombre d'operandes trop important, l'operation necessite 1 operande.");
    structure_instruction->code_erreur = 3;
    return structure_instruction;

  }

  // gestion des erreurs : syntaxe
  src2_incorrecte(structure_instruction, src2);

  // si S n'est ni un registre ni une valeur immédiate, c'est soit une erreur soit un nom d'étiquette
  if (strcmp("La src2 n'est ni un registre ni une valeur immediate.",structure_instruction->message_erreur) == 0){
    int i;
    for (i = 0; Tab_Etiquette[i] != NULL; i++){
      // si on trouve l'étiquette dans Tab_Etiquette
      if (strcmp(src2, Tab_Etiquette[i]->nom) == 0){
        structure_instruction->imm = 1;
        structure_instruction->src2_valeur = 4 * (Tab_Etiquette[i]->num_ligne);
        strcpy(structure_instruction->message_erreur,"Pas d'erreurs.");
        structure_instruction->code_erreur = 0;
        // les champs dest et src1 ne sont pas significatifs
        structure_instruction->src1 = 0;
        structure_instruction->dest = 0;
        return structure_instruction;
      }
    }
  }
  structure_instruction->src1 = 0;
  structure_instruction->dest = 0;
  return structure_instruction;
}


instruction* lire_IES(char* ligne, instruction* structure_instruction){

  char val_surnumeraire[25] = {'\0'};
  // la chaine mot_cle_op est créée pour être utilisée dans le format mais on ne se servira pas de la valeur car on a déjà récupéré le code_op
  char mot_cle_op[4];
  int valeur_retour_lecture;
  char dest[25];

  // on remplit dest avec la valeur lue
  valeur_retour_lecture = sscanf(ligne, " %s %s %s ", mot_cle_op, dest,val_surnumeraire);

  // gestion des erreurs : trop peu d'opérandes
  if(valeur_retour_lecture < 2){
    strcpy(structure_instruction->message_erreur,"Nombre d'operandes insuffisant, l'operation necessite 1 operande.");
    structure_instruction->code_erreur = 2;
    return structure_instruction;
  }

  //gestion des erreurs : trop d'opérandes
  if(val_surnumeraire[0] != '\0'){
    strcpy(structure_instruction->message_erreur,"Nombre d'operandes trop important, l'operation necessite 1 operande.");
    structure_instruction->code_erreur = 3;
    return structure_instruction;

  }

  // gestion des erreurs : syntaxe
  registre_incorrect(structure_instruction, dest);
  // les bits des champs Src1, Src2 et Imm ne sont pas significatifs pour les entrées/sorties
  structure_instruction->src1 = 0;
  structure_instruction->imm = 0;
  structure_instruction->src2_valeur = 0;
  return structure_instruction;

}




// on récupère ligne dans le main après le gerer_etiquette
instruction* lire_instructions(char* ligne, char Tab_operations[32][4] , etiquette** Tab_Etiquette, instruction* structure_instruction){
  char mot_cle_op[I];

  // retirer les virgules et les parenthèses dans la ligne d'instructions
  for(int i = 0; ligne[i] != '\0'; i++){
    if((ligne[i] == ',') || (ligne[i] == '(') || (ligne[i] == ')')){
      ligne[i] = ' ';
    }
  }
  int code_op;
  // récupérer l'opération au début de l'instruction
  if(sscanf(ligne,"%s", mot_cle_op) != 1){
    strcpy(structure_instruction->message_erreur,"La ligne est vide.");
    structure_instruction->code_erreur = 2;
    return structure_instruction;
  }

  // identifier l'opération : l'indice de la case où est stockée l'instruction correspond au code opération
  for(code_op = 0; (strcmp(Tab_operations[code_op], mot_cle_op) != 0) && (code_op < 32); code_op++);
  structure_instruction->code_op = code_op;

  // gestion d'erreur : mot-clé non reconnu
  if ((code_op >= 32) || (strcmp(Tab_operations[code_op], "none") == 0)) {
    strcpy(structure_instruction->message_erreur,"L'operation que vous voulez effectuer n'existe pas.");
    structure_instruction->code_erreur = 1;
    return structure_instruction;
  }

  else {
    if (((structure_instruction->code_op >= 0) && (structure_instruction->code_op <= 11)) || (structure_instruction->code_op ==30))
      return lire_IAL_IT(ligne, structure_instruction);
    if ((structure_instruction->code_op >= 16) && (structure_instruction->code_op <= 22))
      return lire_IS(ligne, structure_instruction, Tab_Etiquette);
    if ((structure_instruction->code_op == 28) || (structure_instruction->code_op == 29))
      return lire_IES(ligne, structure_instruction);
    else{
      //gérer le cas d'erreur du hlt
      char val_surnumeraire[25] = {'\0'}, mot_cle_op[4];
      sscanf(ligne, " %s %s ", mot_cle_op, val_surnumeraire);

      //gestion des erreurs : trop d'opérandes
      if(val_surnumeraire[0] != '\0'){
        strcpy(structure_instruction->message_erreur,"Nombre d'operandes trop important, l'operation ne prend pas d'operande.");
        structure_instruction->code_erreur = 3;
        return structure_instruction;
      }
      strcpy(structure_instruction->message_erreur,"Pas d'erreurs");
      structure_instruction->dest = 0;
      structure_instruction->src1 = 0;
      structure_instruction->imm = 0;
      structure_instruction->src2_valeur = 0;
      return structure_instruction;
    }

  }
}

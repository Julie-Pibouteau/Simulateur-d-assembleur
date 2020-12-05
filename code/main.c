#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "etiquette.h"
#include "conversion.h"
#include "lire_instructions.h"
#define L 1000



void usage(char *prog) {
    fprintf(stderr, "Usage: %s fichier_assembleur.txt \n\n", prog);
}
void registres_etat(int* Z, int* N, int* C, int result, short valeur_finale){
/* Z : zero */
  if (valeur_finale == 0) *Z = 1;
  else *Z = 0;
/* N : bit de poids fort */
  if(valeur_finale <0) *N = 1;
  else *N = 0;
/* C : retenue */
  if (result<-32768 || result>32767) *C = 1;
  else *C = 0;
}

/* fonction qui met à jour seulement les registres d'état Z et N (pour pouvoir gérer C à part dans les cas particuliers) */
void registres_etat_ZN(int* Z, int* N, int valeur_finale){
  *Z = *N = 0;
  if(valeur_finale==0) *Z=1;
  if(valeur_finale<0) *N=1;
}

// indique si la ligne passée en argument est une ligne vide
int est_ligne_vide(char* ligne){
  int i, nb_vides = 0;
  for (i=0; i< strlen(ligne)-1; i++){
    if (ligne[i] == '\t' || ligne[i] == ' ' || ligne[i] == '\r' || ligne[i] == '\f' || ligne[i] == '\v') nb_vides++;
  }
  if(ligne[i] == '\n') nb_vides++;
  //si la chaîne est vide
  if(nb_vides == strlen(ligne)) return 0;
  else return 1;
}

int main(int argc, char** argv) {
   char* f_input;
/* vérifie qu'il y a 2 arguments --> sinon affiche un message d'erreur dans le terminal et arrête le programme*/
   if(argc != 2) {
      usage(argv[0]);
      return 0;
   }
/* affecte les fichiers passés en arguments à f_input */
   f_input=argv[1];


/* PARTIE 1 : assembleur --> hexadécimal */
/* LECTURE FICHIER */
  FILE* f_assembleur = fopen(f_input, "r");
  if(f_assembleur == NULL){
    fprintf(stderr, "%s : erreur fichier d'entrée (%s)\n", argv[0], f_input);
    return 0;
  }

/* GESTION DES ETIQUETTES : on parcourt le fichier en entier pour repérer et stocker les étiquette */
  etiquette* Tab_Etiquette[L];
  int nb_etiquette = 0, cpt_ligne = 0;
  //cpt_ligne compte le nombre de lignes NON VIDES
  char ligne[I];
  while(fgets(ligne, I, f_assembleur)!=NULL){
    //si la ligne n'est pas vide
    if(est_ligne_vide(ligne) != 0) {
      gerer_etiquette(ligne, cpt_ligne, Tab_Etiquette, &nb_etiquette);
      cpt_ligne++;
    }
  }
  Tab_Etiquette[nb_etiquette] = NULL;

/* LECTURE DES INSTRUCTIONS ET ECRITURE DU FICHIER EN HEXADECIMAL*/
  rewind(f_assembleur);
  FILE* f_output = fopen("hexa.txt", "w");
  if(f_output == NULL){
    fprintf(stderr, "%s : erreur fichier de sortie (hexa.txt)\n", argv[0]);
    return 0;
  }
  char Tab_operations[32][4] = {"add","sub", "mul", "div", "or", "xor", "and", "shl", "ldb", "ldw", "stb", "stw", "none", "none", "none", "none", "jmp", "jzs", "jzc", "jcs", "jcc", "jns", "jnc", "none", "none", "none", "none", "none", "in", "out", "rnd", "hlt"};
  instruction* instr = malloc(sizeof(instruction));
  instr->dest = 0;
  instr->src1 = 0;
  instr->imm = 0;
  instr->src2_valeur = 0;
  instr->code_erreur = 0;
  instr->message_erreur = malloc(sizeof(char)*100);
  char hexa[9];

  cpt_ligne = 0; //cpt_ligne compte le nombre de lignes NON VIDES
  int nb_lignes_vides = 0;
  while(fgets(ligne, I, f_assembleur)!=NULL){
    //si la ligne n'est pas vide
    if(est_ligne_vide(ligne) != 0) {
      cpt_ligne++;
      /* on enlève l'étiquette s'il y en a une */
      supp_etiquette(ligne);
      /* on lit l'instruction, on vérifie qu'il n'y a pas d'erreur de syntaxe et on récupère les opérandes */
      instr = lire_instructions(ligne, Tab_operations, Tab_Etiquette, instr);
      /* si il y a une erreur de syntaxe, on renvoie un message d'erreur et on arrête le programme */
      if(instr->code_erreur != 0){
        fprintf(stderr, "%s : Erreur ligne %d : %s \n%s \n\n", argv[0], cpt_ligne + nb_lignes_vides, ligne, instr->message_erreur);
        free(instr->message_erreur);
        free(instr);
        fclose(f_assembleur);
        fclose(f_output);
        // s'il y a une erreur, on ne doit pas générer le fichier hexa.txt
        remove("hexa.txt");
        //libérer le tableau d'étiquettes
        int i = 0;
        while(Tab_Etiquette[i] != NULL){
          free(Tab_Etiquette[i]);
          i++;
        }
        return 0;
      }
      /* on génère l'instruction en hexadécimal et on l'écrit dans le fichier */
      generateur_hexa(instr->code_op, instr->dest, instr->src1, instr->imm, instr->src2_valeur, hexa);

      fprintf(f_output,"%s\n", hexa);
    }
    else nb_lignes_vides++;
  }
  free(instr->message_erreur);
  free(instr);
  fclose(f_assembleur);
  fclose(f_output);
  //libérer le tableau d'étiquettes
  int i = 0;
  while(Tab_Etiquette[i] != NULL){
    free(Tab_Etiquette[i]);
    i++;
  }


  /* PARTIE 2 : éxecution du fichier en hexadécimal */
/* INITIALISATION DES ELEMENTS DU PROCESSEUR */
    char Mem[65536];
    short R[32] = {0}; // on initialise ici les registres pour éviter les erreurs mais dans la réalité, ces registres ne sont pas initialisés
    R[0] = 0;



    int PC = 0; // car si c'est un unsigned short (0 à 65536), si il dépasse 65536 il va revenir à 0 et le programme va lire l'instruction en 0 alors qu'on veut qu'il s'arrête. De plus, PC=-1 nous sert de condition d'arrêt
    int Z = 0, N = 0, C = 0;
    srand(time(NULL));

/* IMPLEMENTATION DU PROGRAMME EN MEMOIRE : lecture du fichier en hexadecimal */
    FILE* f_hexa = fopen("hexa.txt","r");
    if(f_hexa == NULL){
      fprintf(stderr, "%s : erreur lecture fichier (hexa.txt)\n", argv[0]);
      return 0;
    }

    int cpt_memoire = 0;
    int tmp;
    char hex2[3];   // notation : hex2 car représente 2 caractères en hexa
    while(fscanf(f_hexa, "%2s", hex2) == 1 && cpt_memoire<65536){  //%2s pour récupérer une chaine de maximum 2 caractères
      sscanf(hex2,"%x", &tmp);  /* on utilise une variable int tmp car sinon Warning à la compilation :"Mem+cpt_memoire de type char* alors qu'on attend un int*" */
      Mem[cpt_memoire] = tmp;   /* ici : convertit en char pour le mettre dans le tableau */
      cpt_memoire++;
    }

    fclose(f_hexa);

  /* EXECUTION DU PROGRAMME */
    int instr_binaire[32] = {0};
    int code_op, dest, src, imm, src2;

    int result; /* pour stocker le resultat normalement attendu (sans débordement), justement pour voir s'il y a eu débordement */



    while(PC>=0 && PC < cpt_memoire){
      conv_instr_memoire_vers_tab_bin(Mem+PC, instr_binaire);
      conv_bin_vers_operandes(instr_binaire, &code_op, &dest, &src, &imm, &src2);
      PC += 4;

      if (imm == 0){
        src2 = R[src2];
      }

      switch(code_op){
        case 0 :
          result = R[src] + src2;
          R[dest] = result; /* normalement fait la conversion de int à short */
          registres_etat(&Z, &N, &C, result, R[dest]); /* il faut garder result pour voir si il y a eu débordement */
          break;

        case 1 :
          result = R[src] - src2 ;
          R[dest] = result;
          registres_etat(&Z, &N, &C, result, R[dest]);
          break;

        case 2 :
          /* Multiplication sur les 8 bits de poids faible */
          result = ((char)R[src]) * ((char)src2);
          R[dest]=result;
          registres_etat(&Z, &N, &C, result, R[dest]);
          break;

        case 3 :
          if(src2 == 0){
            fprintf(stderr, "%s : Erreur ligne %d : division par 0\n", argv[0], (PC-4)/4 +1);
            PC = -1;
            break;
          }
          R[dest] = R[src] / src2;
          registres_etat(&Z, &N, &C, 0 /* ne peut jamais déborder ici (donc on peut mettre 0 si c'est sûr que ça déborde jamais --> C sera alors toujours mis à 0)*/, R[dest]);
          break;

        case 4 :
          R[dest] = R[src] | src2;
          registres_etat(&Z, &N, &C, 0, R[dest]);
          break;

        case 5 :
          R[dest] = R[src] ^ src2;
          registres_etat(&Z, &N, &C, 0, R[dest]);
          break;

        case 6 :
          R[dest] = R[src] & src2;
          registres_etat(&Z, &N, &C, 0, R[dest]);
          break;

        case 7 :
          if(src2 == 0){
            R[dest] = R[src];
            registres_etat(&Z, &N, &C, 0, R[dest]);
            break;
          }
          /* si on décale à gauche */
          if (src2>0){
            R[dest] = R[src]<<(src2-1);
            /* c'est le bit de poids fort qui va disparaitre : on le stocke dans le registre C */
            if(R[dest]>=0)
              C = 0;
            else
              C = 1;
            R[dest] = R[dest]<<1;
          }
          /* si on décale à droite */
          else {
            R[dest] = R[src]>>((-src2)-1);
            /* c'est le bit de poids faible qui va disparaitre : on le stocke dans le registre C */
            C = R[dest] & 1;  /* agit comme un masque pour récuperer le bit de poids faible*/
            R[dest] = R[dest]>>1 ;
          }
          /* puis on met à jour les autres registres d'état */
          registres_etat_ZN(&Z, &N, R[dest]);
          break;

        case 8 :
          R[dest] = Mem[R[src] + src2];
          registres_etat(&Z, &N, &C, 0, R[dest]);
          break;

        case 9 :
          R[dest] = Mem[R[src]+src2] + (Mem[R[src]+src2+1]<<8);
          registres_etat(&Z, &N, &C, 0, R[dest]);
          break;

        case 10 :
          result = R[src];
          Mem[R[dest]+src2] = result;
          /*test si il y a débordement pour mettre à jour C correctement (car on ne considère pas le même intervalle que dans la fonction registre_d'etat() car on est sur 8 bits ici)*/
          if(result<-128 || result>127)
            C = 1;
          /* on met à jour les autres registres d'état */
          registres_etat_ZN(&Z, &N, Mem[R[dest]+src2]);
          break;

        case 11 :
          Mem[R[dest]+src2] = R[src]&255; /* &255 agit comme un masque pour récupérer les bits de poids faible */
          Mem[R[dest]+src2+1] = (R[src]&65280)>>8;
          registres_etat(&Z, &N, &C, 0, R[src]);
          break;

        case 16 :
          if(src2<0)
            src2 = src2 + 65536 ;
          PC = src2;
          break;

        case 17 :
          if(Z==1){
            if(src2<0)
              src2 = src2 + 65536 ;
            PC = src2;
          }
          break;

        case 18 :
          if(Z==0){
            if(src2<0)
              src2 = src2 + 65536 ;
            PC = src2;
          }
          break;

        case 19 :
          if(C==1){
            if(src2<0)
              src2 = src2 + 65536 ;
            PC = src2;
          }
          break;

        case 20 :
          if(C==0){
            if(src2<0)
              src2 = src2 + 65536 ;
            PC = src2;
          }
          break;

        case 21 :
          if(N==1){
            if(src2<0)
              src2 = src2 + 65536 ;
            PC = src2;
          }
          break;

        case 22 :
          if(N==0){
            if(src2<0)
              src2 = src2 + 65536 ;
            PC = src2;
          }
          break;

        case 28 :
          printf("Entrer un nombre : ");
          if (scanf("%d",&result) == 1){
            R[dest]=result;
            registres_etat(&Z, &N, &C, result, R[dest]);
            break;
          }
          else {
            printf("Erreur : entree invalide.\n");
            return 0;
          }


        case 29 :
          printf("%d\n",R[dest]);
          registres_etat(&Z, &N, &C, 0, R[dest]);
          break;

        case 30 :
          if (R[src]>=src2){
            fprintf(stderr, "Erreur ligne %d : generation nombre aleatoire impossible (on n'a pas %d < %d)\n", (PC-4)/4 +1, R[src], src2);
            PC=-1;
            break;
          }
          R[dest] = ( rand()%(src2-R[src]) ) + R[src];
          registres_etat(&Z, &N, &C, 0, R[dest]);
          break;

        case 31:
          PC = -1;
          break;
      }

      if(dest==0)
        R[0]=0;

    }
  return 0;
}

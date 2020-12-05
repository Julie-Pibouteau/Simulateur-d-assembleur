#include "etiquette.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void gerer_etiquette(char* ligne, int cpt_ligne, etiquette* Tab_Etiquette[], int* nb_etiquette){  
/* recherche de l'étiquette */
  char chaine_etiq[E] = {' '};
  sscanf(ligne, "%s", chaine_etiq);

  /* si on a trouvé une étiquette : */
  if(chaine_etiq[strlen(chaine_etiq)-1] == ':'){
    /* on crée l'étiquette */
    chaine_etiq[strlen(chaine_etiq)-1] = '\0';

    etiquette* etiq = malloc(sizeof(etiquette));
    strcpy(etiq->nom, chaine_etiq);
    etiq->num_ligne = cpt_ligne;
    Tab_Etiquette[*nb_etiquette] = etiq;
    (*nb_etiquette)++;
  }
}

void supp_etiquette(char ligne[]){
  char chaine_etiq[E] = {' '};
  sscanf(ligne, "%s", chaine_etiq);
  /* si on trouve une étiquette : */
  if(chaine_etiq[strlen(chaine_etiq)-1] == ':'){
    int decalage = strlen(chaine_etiq)+1;
    int j;
    for(j=0; ligne[j+decalage] != '\0'; j++){
      ligne[j]=ligne[j+decalage];
    }
    ligne[j] = '\0';
  }
}

#ifndef ETIQUETTE_H
#define ETIQUETTE_H
#define E 100
#define I 200


typedef struct{
  char nom[E];
  int num_ligne;
} etiquette;


/* cherche dans une chaîne s'il y a une étiquette, si oui : la crée, la stocke dans Tab_Etiquette[] et incrémente le nombre d'étiquette */
void gerer_etiquette(char* ligne, int cpt_ligne, etiquette* Tab_Etiquette[], int* nb_etiquette);

/* cherche s'il y a une étiquette dans une chaine de caractère et si oui la supprime */
void supp_etiquette(char ligne[]);


#endif

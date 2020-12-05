#include "conversion.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* PARTIE 1 : assembleur --> hexa */

void generateur_hexa(int code_op, int rd, int rs, int imm, int src2, char* hexa){
  /*convertir scr2 en son C2 (pour obtenir sa valeur en représentation non signée)*/
  src2 = (src2+ 65536)% 65536;

  unsigned int decimal = src2 + imm*pow(2,16) + rs*pow(2,17) + rd*pow(2,22) + code_op*pow(2,27);
  sprintf(hexa,"%.8X", decimal);
}




/* PARTIE 2 : hexa --> execution */

void conv_octet_memoire_vers_tab_bin(char* p_mem, int bin[]){
  int i;
  int x = *p_mem;
  x = (x+256)%256; // on remet la valeur en représentation non signée car elle était signée pour pouvoir la stockée dans la mémoire
  for(i=0; i<8 ; i++){
    if(x>0){
      bin[7-i] = x%2;
      x = x/2;
    }
    else{
      bin[7-i] = 0;
    }
  }
}

void conv_instr_memoire_vers_tab_bin(char* PCMem, int bin[]){
  for(int i=0; i<4; i++){
    conv_octet_memoire_vers_tab_bin(PCMem+i, bin+(8*i));
  }
}

int conv_bin_vers_int(int nb_bits, int bin[]){
  int x = 0;
  for(int i=0; i<nb_bits; i++){
    x += bin[i]*pow(2,nb_bits-1-i);
  }
  return x;
}

void conv_bin_vers_operandes(int bin[], int* code_op, int* dest, int* src, int* imm, int* src2){
  *code_op = conv_bin_vers_int(5,bin);
  *dest = conv_bin_vers_int(5,bin+5);
  *src = conv_bin_vers_int(5,bin+10);
  *imm = bin[15];
  *src2 = conv_bin_vers_int(16, bin+16);
  /* gérer les différents cas (C2 ou non) */
  if((*code_op<16 || *code_op>22) && *src2>32767){  // on ne met pas en C2 [-32768,32767] si c'est des jumps car les adresses vont de [0,65535]
    *src2 = *src2 - 65536 ;
  }
}

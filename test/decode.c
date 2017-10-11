#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// If DE contains a decode bundle:
// If RN is not empty (cannot accept a new rename bundle), then do
// nothing. If RN is empty (can accept a new rename bundle), then
// advance the decode bundle from DE to RN.

void decode(){

  //update decode cycle
  if(miss_timer == 0){
    for(int i = 0; i < width; i++){
      if(de[i].empty && (de[i].de_beg == 0)){
        de[i].fe_beg = cycle_count - 1; 
        de[i].de_beg = cycle_count;
        de[i].fe_dur = de[i].de_beg - de[i].fe_beg;
      }
    }

    for(int i = 0; i < width; i ++){
      if(!rn[i].empty && de[i].empty){
        rn[i] = de[i];
        rn[i].empty = 1;
        //  printf("decode: %d %d %d %d %d de_beg: %d\n" , de[i].instr_num, de[i].op_type, de[i].dest_reg, de[i].src1_reg, de[i].src2_reg, de[i].de_beg);
        memset(&de[i], 0, sizeof(struct instruction));
      }
    }
  }
}

int deIsEmpty(){
  for(int i = 0; i < width; i++){
    if(de[i].empty){
      return 0;
    }
  }
  return 1;
}

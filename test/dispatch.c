#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// If DI contains a dispatch bundle:
// If the number of free IQ entries is less than the size of the
// dispatch bundle in DI, then do nothing. If the number of free IQ
// entries is greater than or equal to the size of the dispatch bundle
// in DI, then dispatch all instructions from DI to the IQ.
void dispatch(){

  //update dispatch stage
  if(di[0].empty && (di[0].di_beg == 0)){
    for(int i = 0; i < width; i ++){
      //add rename cycle begin
      di[i].di_beg = cycle_count;
      di[i].rr_dur = di[i].di_beg - di[i].rr_beg;
    }
  }

  if(di[0].empty){
    int iq_count = 0;
    //count free entries in iq
    for(int i = 0; i < iq_size; i++){
      if(!iq[i].empty){
        iq_count++;
      }
    }
    //if iq count is greater than width, dispatch instructions to the iq
    if(iq_count >= width){
      int j = 0;
      for(int i = 0; i < iq_size; i ++){
        if(j == width){
          break;
        }
        if(!iq[i].empty && di[j].empty){
          iq[i] = di[j];
          iq[i].empty = 1;
            if(di[j].pc == 0x2b6f64){
        //  printf("dispatch: %x %d %d %d %d %d di_beg: %d\n" ,di[j].pc, di[j].instr_num, di[j].op_type, di[j].dest_reg, di[j].src1_reg, di[j].src2_reg, di[j].di_beg);
        }
          memset(&di[j], 0, sizeof(struct instruction));
          j++;
        }
      }
    }
  }
}

int diIsEmpty(){
  for(int i = 0; i < width; i++){
    if(di[i].empty){
      return 0;
    }
  }
  return 1;
}

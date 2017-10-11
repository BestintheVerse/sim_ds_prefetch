#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// If RR contains a register-read bundle:
// If DI is not empty (cannot accept a new dispatch bundle), then do
// nothing. If DI is empty (can accept a new dispatch bundle), then
// process (see below) the register-read bundle and advance it from RR
// to DI.
// How to process the register-read bundle:
// Since values are not explicitly modeled, the sole purpose of the
// Register Read stage is to ascertain the readiness of the renamed
// source operands. Apply your learning from the class lectures/notes
// on this topic.
// Also take care that producers in their last cycle of execution
// wakeup dependent operands not just in the IQ, but also in two other
// stages including RegRead()(this is required to avoid deadlock). See
// Execute() description above.
void regRead(){

  //update regread stage
  if(rr[0].empty && (rr[0].rr_beg == 0)){
    for(int i = 0; i < width; i ++){
      //add rename cycle begin
      rr[i].rr_beg = cycle_count;
      rr[i].rn_dur = rr[i].rr_beg - rr[i].rn_beg;
    }

  }

  if(rr[0].empty){
    if(!di[0].empty){
      for(int i = 0; i < width; i++){
        if(rr[i].empty){
          //set ready bits for sources 1 and 2
          //if src is less than 100 (a ARF value)
          if(rr[i].src1_reg < 100){
            rr[i].iq_src1_ready = 1;
          }
          if(rr[i].src2_reg < 100){
            rr[i].iq_src2_ready = 1;
          }
          //if src is greater than 100 (a rob value)
          if(rr[i].src1_reg > 99){
            //set ready bits
            if(rob[rr[i].src1_reg - 100].rob_ready){
              rr[i].iq_src1_ready = 1;
            }
          }
          if(rr[i].src2_reg > 99){
            //set ready bits
            if(rob[rr[i].src2_reg - 100].rob_ready){
              rr[i].iq_src2_ready = 1;
            }
          }
          
          //advance rr bundle to di bundle
          di[i] = rr[i];
          di[i].empty = 1;

          //    printf("regRead: %d %d %d %d %d rr_beg: %d\n" , rr[i].instr_num, rr[i].op_type, rr[i].dest_reg, rr[i].src1_reg, rr[i].src2_reg, rr[i].rr_beg);
        }
        memset(&rr[i], 0, sizeof(struct instruction));
      }
    }
  }
}

int rrIsEmpty(){
  for(int i = 0; i < width; i++){
    if(rr[i].empty){
      return 0;
    }
  }
  return 1;
}

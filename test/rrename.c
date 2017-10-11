#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// If RN contains a rename bundle:
// If either RR is not empty (cannot accept a new register-read bundle)
// or the ROB does not have enough free entries to accept the entire
// rename bundle, then do nothing.
// If RR is empty (can accept a new register-read bundle) and the ROB
// has enough free entries to accept the entire rename bundle, then
// process (see below) the rename bundle and advance it from RN to RR.
// How to process the rename bundle:
// Apply your learning from the class lectures/notes on the steps for
// renaming:
// (1) Allocate an entry in the ROB for the instruction,
// (2) Rename its source registers, and
// (3) Rename its destination register (if it has one).
// Note that the rename bundle must be renamed in program order.
// Fortunately, the instructions in the rename bundle are in program
// order).

void rrename(){

  //update rename stage

  for(int i = 0; i < width; i ++){
    if(rn[i].empty && (rn[i].rn_beg == 0)){
      //add rename cycle begin
      rn[i].rn_beg = cycle_count;
      rn[i].de_dur = rn[i].rn_beg - rn[i].de_beg;
    }
  }

  //if rename bundle exists
  if(rn[0].empty){
    //printf("rename bundle exists on cycle %d\n", cycle_count);
    //  printf("rr empty: %d\n" ,rr[0].empty );
    //if rr is empty and rob has WIDTH free space
    //count free spaces in rob
    int rob_free_count = 0;
    for(int i = 0; i < rob_size; i++){
      if(!rob[i].empty){
        rob_free_count++;
      }
    }

    //  printf("rr empty on cycle %d\n" , cycle_count);
    for(int i = 0; i < width; i ++){
      if(!rr[i].empty && (rob_free_count > width) && rn[i].empty){

        //rename the source registers if an entry in the rmt exists
        int temp_reg = rn[i].src1_reg;
        if(rmt[temp_reg]){
          rn[i].src1_reg = rmt[temp_reg];
        }
        temp_reg = rn[i].src2_reg;
        if(rmt[temp_reg]){
          rn[i].src2_reg = rmt[temp_reg];
        }

        //Make space in the ROB
        //increment rob tail pointer
        int rob_index;
        if(rob_tail == (rob_size-1)){
          rob_tail = 0;
          rob_index = rob_size - 1;
        }
        else {
          rob_index = rob_tail;
          rob_tail++;
        }

        //if instruction has a destination reg
        //rename destination register. the + 100 is to "encode" the rob name so as to differentiate the value from actual reg values
        int rob_rename = rob_index + 100;
        if(rn[i].dest_reg != -1){
          //store rob name at actual register in rmt
          rmt[rn[i].dest_reg] = rob_rename;
        }
        //add the destination tag
        rn[i].dest_tag = rob_rename;
        //add instruction to the ROB
        rob[rob_index] = rn[i];
          //    printf("rename: %d %d %d %d %d rn_beg: %d\n" , rn[i].instr_num, rn[i].op_type, rn[i].dest_reg, rn[i].src1_reg, rn[i].src2_reg, rn[i].rn_beg);

        //after rename, transfer rename bundle to regRead pipeline
        rr[i] = rn[i];
        rr[i].empty = 1;
        memset(&rn[i], 0, sizeof(struct instruction));
      }

    }
  }
}

int rnIsEmpty(){
  for(int i = 0; i < width; i++){
    if(rn[i].empty){
      return 0;
    }
  }
  return 1;
}

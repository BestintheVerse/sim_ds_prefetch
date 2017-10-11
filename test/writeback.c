#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Process the writeback bundle in WB: For each instruction in WB, mark
// the instruction as “ready” in its entry in the ROB.
void writeback(){

  //cycle through wb and set wb_beg and ex_dur
  for(int i = 0; i < width*5; i++){
    if(wb[i].empty && (wb[i].wb_beg == 0)){
      wb[i].wb_beg = cycle_count;
      wb[i].ex_dur = wb[i].wb_beg - wb[i].ex_beg;
    }
  }

  for(int i = 0; i < width*5; i++){
    if(wb[i].empty){
      //copy over changes to instruction
      rob[wb[i].dest_tag - 100] = wb[i];
      //mark as ready in the rob
      rob[wb[i].dest_tag - 100].rob_ready = 1;

      //clear instruction from wb
      //printf("writeback: %x %d %d %d %d wb_beg: %d\n" , wb[i].pc, wb[i].op_type, wb[i].dest_reg, wb[i].src1_reg, wb[i].src2_reg, wb[i].wb_beg);
      memset(&wb[i], 0, sizeof(struct instruction));
    }
  }
}

int wbIsEmpty(){
  for(int i = 0; i < width*5; i++){
    if(wb[i].empty){
      return 0;
    }
  }
  return 1;
}

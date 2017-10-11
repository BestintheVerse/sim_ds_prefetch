#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Retire up to WIDTH consecutive “ready” instructions from the head of
// the ROB.
void retire(){

  //cycle through rob and set rt_beg and wb_dur
  for(int i = 0; i < rob_size; i++){
    if(rob[i].rob_ready && rob[i].rt_beg == 0){
      rob[i].rt_beg = cycle_count;
      rob[i].wb_dur = rob[i].rt_beg - rob[i].wb_beg;
    }
  }

  //increment rt_dur
  for(int i = 0; i < rob_size; i++){
    if(rob[i].rob_ready && rob[i].rt_beg != 0){
      rob[i].rt_dur++;
    }
  }

  //retire instructions
  int temp = rob_head;
  for(int i = temp; i < (width + temp); i ++){
    if(rob[rob_head].rob_ready && rob[rob_head].empty){
      //print retired instructions
      printf("%d fu{%d} src{%d,%d} dst{%d} FE{%d,%d} DE{%d,%d} RN{%d,%d} RR{%d,%d} DI{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,%d} RT{%d,%d}\n", rob[rob_head].instr_num, rob[rob_head].op_type, rob[rob_head].src1_orig, rob[rob_head].src2_orig, rob[rob_head].dest_reg, rob[rob_head].fe_beg, rob[rob_head].fe_dur, rob[rob_head].de_beg, rob[rob_head].de_dur, rob[rob_head].rn_beg, rob[rob_head].rn_dur, rob[rob_head].rr_beg, rob[rob_head].rr_dur, rob[rob_head].di_beg, rob[rob_head].di_dur, rob[rob_head].is_beg, rob[rob_head].is_dur, rob[rob_head].ex_beg, rob[rob_head].ex_dur, rob[rob_head].wb_beg, rob[rob_head].wb_dur, rob[rob_head].rt_beg, rob[rob_head].rt_dur);
      //Check if entry in RMT needs to be removed
      if(rob[rob_head].dest_tag == rmt[rob[rob_head].dest_reg]){
        rmt[rob[rob_head].dest_reg] = 0;
      }
      //mark rob entry as invalid
      rob[rob_head].empty = 0;
      //memset(&rob[rob_head], 0, sizeof(struct instruction));

      //increment head
      if(rob_head == (rob_size - 1)){
        rob_head = 0;
      }
      else{
        rob_head++;
      }
    }
    else {break;}
  }
}

int robIsEmpty(){
  for(int i = 0; i < rob_size; i++){
    if(rob[i].empty){
      return 0;
    }
  }
  return 1;
}

#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// Issue up to WIDTH oldest instructions from the IQ. (One approach to
// implement oldest-first issuing is to make multiple passes through
// the IQ, each time finding the next oldest ready instruction and then
// issuing it. One way to annotate the age of an instruction is to
// assign an incrementing sequence number to each instruction as it is
// fetched from the trace file.)
// To issue an instruction:
// 1) Remove the instruction from the IQ.
// 2) Add the instruction to the execute_list. Set a timer for
// the instruction in the execute_list that will allow you to
// model its execution latency.
void issue(){

  //update issue
  for(int i = 0; i < iq_size; i ++){
    //add rename cycle begin
    if(iq[i].empty && iq[i].is_beg == 0){
      iq[i].is_beg = cycle_count;
      iq[i].di_dur = iq[i].is_beg - iq[i].di_beg;
    }
  }

  //increment issue queue counters
  for(int i = 0; i < iq_size; i ++){
    //add rename cycle begin
    if(iq[i].empty){
      iq[i].iq_counter++;
    }
  }

  //issue up to WIDTH oldest instruction from the IQ, if both sources are ready
  for(int i = 0; i < width; i++){
    int index = -1;
    int val = 0;
    //find the oldest ready entry in the iq
    for(int j = 0; j < iq_size; j++){
      if(iq[j].empty){
        if(iq[j].iq_src1_ready & iq[j].iq_src2_ready){
          if(iq[j].iq_counter > val){
            val = iq[j].iq_counter;
            index = j;
          }
        }
      }
    }

    //add entry, if any, to execute list and set execution timer
    if(index != -1){
      for(int i = 0; i < width*5; i++){
        if(!ex_list[i].empty){
          ex_list[i] = iq[index];
            if(iq[index].pc == 0x2b6f64){
        //    printf("issue: %d %d %d %d %d is_beg: %d\n" , iq[index].instr_num, iq[index].op_type, iq[index].dest_reg, iq[index].src1_reg, iq[index].src2_reg, iq[index].is_beg);
          }
          switch (ex_list[i].op_type){
            case 0:
            ex_list[i].timer = 1;
            break;
            case 1:
            ex_list[i].timer = 2;
            break;
            case 2:
            ex_list[i].timer = 5;
          }
          //purge instruction from iq
          memset(&iq[index], 0, sizeof(struct instruction));
          break;
        }
      }
    }
  }
}

int iqIsEmpty(){
  for(int i = 0; i < iq_size; i++){
    if(iq[i].empty){
      return 0;
    }
  }
  return 1;
}

 #include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// From the execute_list, check for instructions that are finishing
// execution this cycle, and:
// 1) Remove the instruction from the execute_list.
// 2) Add the instruction to WB.
// 3) Wakeup dependent instructions (set their source operand ready
// flags) in the IQ, DI (dispatch bundle), and RR (the register-read bundle).
void execute(){

  //cycle through ex_list and set ex_beg and is_dur
  for(int i = 0; i < width*5; i++){
    if(ex_list[i].empty && (ex_list[i].ex_beg == 0)){
      ex_list[i].ex_beg = cycle_count;
      ex_list[i].is_dur = ex_list[i].ex_beg - ex_list[i].is_beg;
    }
  }

  //decrement ex_list counters
  for(int i = 0; i < width*5; i++){
    if(ex_list[i].empty){
        ex_list[i].timer--;
    }
  }

  //process instructions that are finished
  for(int i = 0; i < width*5; i ++){
    if(ex_list[i].empty){
      if(ex_list[i].timer == 0){
      //  printf("ex_list timer %x has ended\n" , ex_list[i].pc);
        //add finished instruction to wb
        for(int j = 0; j < width*5; j++){
          if(!wb[j].empty){
            wb[j] = ex_list[i];
            break;
          }
        }
        //send wakeups to RR
        for(int j = 0; j < width; j++){
          if(ex_list[i].dest_tag == rr[j].src1_reg){
            rr[j].iq_src1_ready = 1;
          }
          if(ex_list[i].dest_tag == rr[j].src2_reg){
            rr[j].iq_src2_ready = 1;
          }
        }
        //send wakeups to DI
        for(int j = 0; j < width; j++){
          if(ex_list[i].dest_tag == di[j].src1_reg){
            di[j].iq_src1_ready = 1;
          }
          if(ex_list[i].dest_tag == di[j].src2_reg){
            di[j].iq_src2_ready = 1;
          }
        }
        //send wakeups to iq
        for(int j = 0; j < iq_size; j++){
          if(ex_list[i].dest_tag == iq[j].src1_reg){
            iq[j].iq_src1_ready = 1;
          }
          if(ex_list[i].dest_tag == iq[j].src2_reg){
            iq[j].iq_src2_ready = 1;
          }
        }

      //  printf("execute: %x %d %d %d %d ex_beg: %d\n" , ex_list[i].pc, ex_list[i].op_type, ex_list[i].dest_reg, ex_list[i].src1_reg, ex_list[i].src2_reg, ex_list[i].ex_beg);
        //purge instruction from ex_list
        memset(&ex_list[i], 0, sizeof(struct instruction));
      }
    }
  }
}

int exIsEmpty(){
  for(int i = 0; i < width*5; i++){
    if(ex_list[i].empty){
      return 0;
    }
  }
  return 1;
}

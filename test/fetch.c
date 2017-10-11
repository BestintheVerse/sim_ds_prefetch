#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//trace file
extern FILE *in;

//trace file buffer
char buff[100];

//cache Hit
int cache_hit;

// Do nothing if instruction cache is perfect (CACHE_SIZE=0) and either
         // (1) there are no more instructions in the trace file or
         // (2) DE is not empty (cannot accept a new decode bundle).
         // If there are more instructions in the trace file and if DE is empty
         // (can accept a new decode bundle), then fetch up to WIDTH
         // instructions from the trace file into DE. Fewer than WIDTH
         // instructions will be fetched only if the trace file has fewer than
         // WIDTH instructions left.
         // If instruction cache is imperfect or next-line prefetcher is
         // enabled, in addition to the above operations, adjust the timer to
         // model fetch latency when necessary.
void fetch(){

  //if DE is empty, get WIDTH instructions from trace file
  if(!de[0].empty){
    for(int i = 0; i < width; i++){
      if(fgets(buff, sizeof buff, in) != NULL){
        int value;
        char pcaddress[64];
        char *token=strtok(buff," ");

        //scan the program counter
        sscanf(token, "%s", pcaddress);
        token=strtok(NULL," ");
        de[i].pc = strtol(pcaddress, NULL, 16);
        if(cache_size > 0){
          c_pc = strtol(pcaddress, NULL, 16);
          cache_hit = cache_process();
          if(!cache_hit){
            miss_timer += MISS_DELAY;
          }
        }


        //scan the rest of the instruction input
        int j = 0;
        while(token!=NULL){
          if(sscanf(token, "%d", &value) > 0)
          token=strtok(NULL," ");
          switch(j){
            //process value into operation type
            case 0:
              de[i].op_type = value;
              break;
            //process value into destination register
            case 1:
              de[i].dest_reg = value;
              break;
            //process value into source reg 1
            case 2:
              de[i].src1_reg = value;
              de[i].src1_orig = value;
              break;
            //process value into source reg 2
            case 3:
              de[i].src2_reg = value;
              de[i].src2_orig = value;
          }
          j++;
        }
        de[i].empty = 1;
        de[i].instr_num = instr_count;
        instr_count++;
      }
      else{
        trace_empty = 1;
      }
    //  printf("fetch: %d %d %d %d %d %d fe_beg: %d\n" , de[i].empty, de[i].instr_num, de[i].op_type, de[i].dest_reg, de[i].src1_reg, de[i].src2_reg, de[i].fe_beg);

    }
    miss_timer++;
  }



}

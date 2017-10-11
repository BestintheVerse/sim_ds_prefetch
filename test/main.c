#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

//Global variables for arguments
int width;
int iq_size;
int rob_size;
int cache_size;
int p;
char * trace;

//pipeline registers
struct instruction * de;
struct instruction * rn;
struct instruction * rr;
struct instruction * di;
struct instruction * iq;
struct instruction * ex_list;
struct instruction * wb;
struct instruction * rob;

//ROB variables
int rob_head = 0;
int rob_tail = 0;

//Rename map table
int rmt[67];

//boolean variables
int pipeline_empty;
int trace_empty;

//counters
int cycle_count = 0;
int instr_count = 0;

//trace file
FILE *in;
FILE *out;

//instruction cache variables
struct Block **cache;
int num_sets;
int tag_width;
int offset_width;
int index_width;
int c_hits = 0;
int c_pc;
int c_tag;
int c_index;
int miss_timer = 0;

//prefetch variables
int p_hits = 0;


int main(int argc, char **argv){

  //get command line args
  rob_size = atoi(argv[1]) + 1;
  iq_size = atoi(argv[2]);
  width = atoi(argv[3]);
  cache_size = atoi(argv[4]);
  p = atoi(argv[5]);
  trace = argv[6];

  //initialize pipeline registers
  de = calloc(width, sizeof(struct instruction));
  rn = calloc(width, sizeof(struct instruction));
  rr = calloc(width, sizeof(struct instruction));
  di = calloc(width, sizeof(struct instruction));
  iq = calloc(iq_size, sizeof(struct instruction));
  ex_list = calloc(width*5, sizeof(struct instruction));
  wb = calloc(width*5, sizeof(struct instruction));
  rob = calloc(rob_size, sizeof(struct instruction));

  //calculate cache variables
  num_sets = cache_size / (BLOCK_SIZE * NUM_WAYS);
  index_width = log2(num_sets);
  offset_width = log2(BLOCK_SIZE);
  tag_width = PC_LENGTH - index_width - offset_width;

  //initialize cache
  cache = calloc(num_sets, sizeof(struct Block *));
  for(int i=0; i< num_sets; i++){
    cache[i] = calloc(NUM_WAYS,  sizeof(struct Block));
  }

  //open the trace file for reading
  in = fopen(trace, "r");

  do {
    retire(); // Retire up to WIDTH consecutive “ready” instructions from the head of
              // the ROB.

    writeback(); // Process the writeback bundle in WB: For each instruction in WB, mark
                 // the instruction as “ready” in its entry in the ROB.

    execute(); // From the execute_list, check for instructions that are finishing
               // execution this cycle, and:
               // 1) Remove the instruction from the execute_list.
               // 2) Add the instruction to WB.
               // 3) Wakeup dependent instructions (set their source operand ready
               // flags) in the IQ, DI (dispatch bundle), and RR (the register-read
               // bundle).

    issue(); // Issue up to WIDTH oldest instructions from the IQ. (One approach to
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

    dispatch(); // If DI contains a dispatch bundle:
                // If the number of free IQ entries is less than the size of the
                // dispatch bundle in DI, then do nothing. If the number of free IQ
                // entries is greater than or equal to the size of the dispatch bundle
                // in DI, then dispatch all instructions from DI to the IQ.

    regRead(); // If RR contains a register-read bundle:
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

    rrename(); // If RN contains a rename bundle:
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

    decode(); // If DE contains a decode bundle:
              // If RN is not empty (cannot accept a new rename bundle), then do
              // nothing. If RN is empty (can accept a new rename bundle), then
              // advance the decode bundle from DE to RN.

    fetch(); // Do nothing if instruction cache is perfect (CACHE_SIZE=0) and either
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

    if(deIsEmpty() & rnIsEmpty() & rrIsEmpty() & diIsEmpty() & iqIsEmpty() & exIsEmpty() & wbIsEmpty() & robIsEmpty()){
      pipeline_empty = 1;
    }


  } while (advance_cycle() != 0);
                // advance_cycle() performs several functions.
                // (1) It advances the simulator cycle.
                // (2) When it becomes known that the pipeline is empty AND the trace
                // is depleted, the function returns “false” to terminate the loop.


    //print contents of ROB, rmt, and iq
  /* for(int i = 0; i < rob_size; i ++){
      printf("ROB %d: valid: %d, ready: %d %x %d %d %d %d\n", i, rob[i].empty, rob[i].rob_ready, rob[i].pc, rob[i].op_type, rob[i].dest_reg, rob[i].src1_reg, rob[i].src2_reg);
    }
    for(int i = 0; i < 67; i ++){
      printf("RMT %d: %d\n",i, rmt[i]);
    }
    for(int i = 0; i < iq_size; i ++){
      printf("IQ %d: %d %d %d source 1 ready: %d, %d source 2 ready: %d, %d\n", i, iq[i].instr_num, iq[i].op_type, iq[i].dest_tag, iq[i].iq_src1_ready, iq[i].src1_reg, iq[i].iq_src2_ready, iq[i].src2_reg);
    }
    for(int i = 0; i < width*5; i ++){
      printf("EX_LIST %d: PC: %d, timer: %d\n", i, ex_list[i].instr_num, ex_list[i].timer);
    }
    for(int i = 0; i < width*5; i ++){
      printf("WB %d: pc: %d\n", i, wb[i].empty);
    }
    for(int i = 0; i < width; i++){
      printf("DI %d: empty: %d\n", i, di[i].empty);
    }
    for(int i = 0; i < width; i ++){
      printf("DE %d: empty: %x\n", i, de[i].empty);
    };;
    for(int i = 0; i < width; i ++){
      printf("RN %d: empty: %x\n", i, rn[i].empty);
    }

    printf("rob tail: %d\n", rob_tail);
    printf("rob head: %d\n", rob_head);*/

    //print command line
    printf("# === Simulator Command =========\n");
    printf("# %s %s %s %s %s %s %s\n", argv[0], argv[1], argv[2], argv[3], argv[4], argv[5], argv[6]);
    //print configuration
    printf("# === Processor Configuration ===\n");
    printf("# ROB_SIZE 	= %s\n" , argv[1]);
    printf("# IQ_SIZE  	= %s\n" , argv[2]);
    printf("# WIDTH    	= %s\n" , argv[3]);
    printf("# CACHE_SIZE 	= %s\n" , argv[4]);
    printf("# PREFETCHING	= %s\n" , argv[5]);
    //print results
    printf("# === Simulation Results ========\n");
    printf("# Dynamic Instruction Count      = %d\n", instr_count);
    printf("# Cycles                         = %d\n", cycle_count);
    printf("# Instructions Per Cycle (IPC)   = %.2f\n", (float) instr_count / (float) cycle_count);
    if(cache_size > 0){
      printf("# Instruction Cache Hits:      = %d\n" , c_hits);
    }
    if(p){
      printf("# Prefetch Hits:               = %d\n" , p_hits);
    }

}

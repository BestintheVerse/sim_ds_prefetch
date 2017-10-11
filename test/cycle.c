#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// advance_cycle() performs several functions.
// (1) It advances the simulator cycle.
// (2) When it becomes known that the pipeline is empty AND the trace
// is depleted, the function returns “false” to terminate the loop.
int advance_cycle(){

  //advance cycle
  cycle_count++;

  //decrement miss penalty timer for cache
  if(miss_timer > 0 ){
    miss_timer--;
  }

  //if prefetching is enabled, decrement prefetch timers in cache
  if(p){
    for(int i = 0; i < num_sets; i++){
      for(int j = 0; j < NUM_WAYS; j++){
        if(cache[i][j].prefetch_timer > 0){
          cache[i][j].prefetch_timer--;
        }
      }
    }
  }


  //if cycle count reaches max, end program
  if(cycle_count > CYCLE_COUNT_MAX){
    return 0;
  }

  //end program if pipeline and trace are both empty
  if(pipeline_empty & trace_empty){
    return 0;
  } //otherwise continue running pipeline
  else return 1;

}

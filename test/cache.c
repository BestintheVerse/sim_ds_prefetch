#include "header.h"
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int fullNum;
int c_index;
int c_tag;
int prefetching = 0;

int cache_process(){
  fullNum = 0;
  int cacheHit = 0;

  //get index and tag from pc
  c_tag = c_pc >> (index_width + offset_width);
  c_index = (c_pc >> offset_width ) & (int)(pow(2, index_width) - 1);
  if(prefetching){
    c_index++;
    if(c_index >= (int)(pow(2, index_width))){
      c_index = 0;
      c_tag++;
    }
  }


  //Check cache blocks at index to see if hit
  for(int i=0; i<NUM_WAYS; i++){
    if(cache[c_index][i].isFull){
      if(cache[c_index][i].tag == c_tag){
        //Hit!
        if(!prefetching){ //actual hit
          if(cache[c_index][i].prefetch_flag){
            //prefetch hit
            miss_timer = cache[c_index][i].prefetch_timer;
            cache[c_index][i].prefetch_flag = 0;
            if(cache[c_index][i].prefetch_timer == 0){
              p_hits++;
              c_hits++;
            }
            //fetch p+1
            prefetching = 1;
            cache_process();
            prefetching = 0;
          }
          else{
            c_hits++;
          }
          rank_LRU_hit();
          cacheHit = 1;
          return 1;
        }
        else{ //hit while prefetching, do nothing
          return 1;
        }
      }
      fullNum++;
    }
  }
  //If miss, check and see how full cache is
  if(!cacheHit){
    //If way is completely full
    if(fullNum == NUM_WAYS){
      rank_LRU_miss_replace();
    }
    else if(fullNum < NUM_WAYS){
      //Way is not full, so load block into cache
      rank_LRU_miss_cold();
      cache[c_index][fullNum].tag = c_tag;
      cache[c_index][fullNum].isFull = 1;
      cache[c_index][fullNum].LRURank = 1;
      if(prefetching){
        cache[c_index][fullNum].prefetch_flag = 1;
        cache[c_index][fullNum].prefetch_timer = 10;
      }
    }
  }
  //on miss, if prefetch is enabled, fetch c_index + 1
  if(p & !prefetching){ //make sure not already prefetching

    prefetching = 1;
    cache_process();
    prefetching = 0;
  }

  return 0;
}

void rank_LRU_hit(){
  for(int i=0; i<NUM_WAYS;i++){
    if(cache[c_index][i].isFull && (cache[c_index][i].LRURank < cache[c_index][fullNum].LRURank)){
      cache[c_index][i].LRURank++;
    }
  }
  cache[c_index][fullNum].LRURank = 1;

}

void rank_LRU_miss_cold(){
  for(int i=0; i<NUM_WAYS; i++){
    if(cache[c_index][i].isFull){
      cache[c_index][i].LRURank++;
    }
  }
}

void rank_LRU_miss_replace(){
  int i = 0;
  while(i < NUM_WAYS){
    if(cache[c_index][i].LRURank == NUM_WAYS){break;}
    i++;
  }
  for(int j=0; j < NUM_WAYS; j++){
    if(cache[c_index][j].LRURank != NUM_WAYS){
      cache[c_index][j].LRURank++;
    }
  }
  cache[c_index][i].LRURank = 1;
  cache[c_index][i].tag = c_tag;
  //set prefetch flag low
  cache[c_index][i].prefetch_flag = 0;
  //set prefetch if enabled
  if(prefetching){
    cache[c_index][i].prefetch_flag = 1;
    cache[c_index][i].prefetch_timer = 10;
  }
}

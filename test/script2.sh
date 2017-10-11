#!/bin/bash
c=4096
while [ $c -le 32768 ]
do
./sim_ds 32 64 4 $c 1 ../traces/val_trace_gcc1
c=$(( $c * 2 ))
done

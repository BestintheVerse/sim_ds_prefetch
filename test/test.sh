#!/bin/bash
w=1
i=8
while [ $w -le 8 ]
do
while [ $i -le 256 ]
do
./sim_bp 512 $i $w 0 0 ../traces/val_trace_gcc1
i=$(( $i * 2 ))
done
w=$(( $2**$w ))
echo $w
done

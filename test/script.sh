#!/bin/bash
w=1
i=8
while [ $w -le 8 ]
do
r=32
while [ $r -le 512 ]
do
./sim_ds $r $i $w 0 0 ../traces/val_trace_perl1
r=$(( $r * 2 ))
done
w=$(( $w * 2 ))
i=$(( $i * 2 ))
if [ $i = 32 ]; then
i=$(( $i * 2 ))
fi
done

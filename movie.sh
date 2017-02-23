#!/bin/bash
rm output/*.png
for t in $(seq 1 1000 50000)
do
   ./renderer -d 255 255 -i $t -p 120 -a .0082 -g .95 --dt .16 --ppp 1.1 -s 43149
done
convert -delay 5 -loop 0 output/*.png animation.gif


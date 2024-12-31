#!/bin/bash
set +x

clear
echo "---------------------------------------------------"
echo "Dois experimentos diferentes, duas redes distintas."
echo "Para visualizar, instale matplotlib, seaborn & cia."
echo "Pressione <ENTER> para começar."
echo ""
echo "O tempo e qualidade da otimização depende de"
echo "  set_loops e set_pop em main*.cpp."
echo ""
echo "Pressione <ENTER>."
echo "--------------------------------------------------"

read x

make opt1
make opt2
rm *.o

./opt1
python3 surf.py output1.csv

./opt2
python3 surf.py output2.csv

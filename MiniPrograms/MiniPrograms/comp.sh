#!/bin/bash

minProgs=(psumscalar padding false1 psumvector pdotproduct counting pmatrixmult pmatrixcompare)
numOfProgs=7
PATH=/MiniPrograms/

REPEAT=5
DSIZE=300000000
D2SIZE=800

# GOOD MODE
for ((i=0; i<$numOfProgs; i++ )); do
        N=$DSIZE
        if [ $i -gt 5 ];then
                N=$D2SIZE
        fi
        gcc -o ${minProgs[i]}-default-g -DGOOD -DREPEAT=$REPEAT -DN=$N $PATH${minProgs[i]}.c -lpthread -lrt
done
          
# BAD_FS MODE 
for ((i=0; i<8; i++ )); do
        N=$DSIZE
        if [ $i -gt 5 ];then
                N=$D2SIZE
        fi   
        gcc -o ${minProgs[i]}-default-f -DBAD_FS -DREPEAT=$REPEAT -DN=$N $PATH${minProgs[i]}.c -lpthread -lrt
done

# GOOD MODE
for ((i=3; i<8; i++ )); do
        N=$DSIZE
        if [ $i -gt 5 ];then 
                N=$D2SIZE
        fi 
        gcc -o ${minProgs[i]}-default-m -DBAD_MA -DREPEAT=$REPEAT -DN=$N $PATH${minProgs[i]}.c -lpthread -lrt
done

#gcc -o false1-default-g -DGOOD -DN=300000000 -DREPEAT=5 ../newMiniPrograms/false1.c -lpthread -lrt 
#gcc -o pdotproduct-default-g -DGOOD -DN=300000000 -DREPEAT=20 ../newMiniPrograms/pdotproduct.c -lpthread -lrt 
#gcc -o pmatrixcompare-default-g -DGOOD -DREPEAT=100 ../newMiniPrograms/pmatrixcompare.c -lpthread -lrt -DN=10000 
#gcc -o pmatrixcompare-default-f -DBAD_FS -DREPEAT=100 ../newMiniPrograms/pmatrixcompare.c -lpthread -lrt -DN=10000
#gcc -o pmatrixcompare-default-m -DBAD_MA -DREPEAT=100 ../newMiniPrograms/pmatrixcompare.c -lpthread -lrt -DN=10000

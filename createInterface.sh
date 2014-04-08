#!/bin/bash

#1 remove files previous test
rm -rf 1 done todo
rm -f 1.sh 2.sh createTest.sh debug.log period test.out

mkdir done
mkdir todo

#2 compile files
gcc -lm -lrt -o period period.c
g++ -lm -lrt -o test.out testGen.cc
g++ -lm -lrt -o setup.out setupTest.cc

#3 create tasks , create input xml for Carts
./test.out 

#4 use Carts to create output xml with interfaces
cd /test/carts/carts-source
java -cp build Carts /test/interfaceIn.xml MPR2 /test/interfaceOut.xml





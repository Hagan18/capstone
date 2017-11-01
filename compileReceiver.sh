#!/usr/bin/env bash
gcc -c -D_GNU_SOURCE receiver.c -lpthread
gcc -c -D_GNU_SOURCE mkaddr.c
gcc -g receiver.o mkaddr.o -o receiver -lpthread

#!/usr/bin/env bash
gcc -c -D_GNU_SOURCE broadcast.c -lpthread
gcc -c -D_GNU_SOURCE mkaddr.c
gcc -g broadcast.o mkaddr.o -o broadcast -lpthread

#!/usr/bin/env bash
gcc -c -D_GNU_SOURCE broadcast.c -lpthread
gcc -g receiver.o mkaddr.o -o receiver -lpthread

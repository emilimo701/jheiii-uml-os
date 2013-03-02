#common.o: common.c
#	gcc -c common.c -I../include --std=c99
CC := gcc
CFLAGS := -I../include -I../../include --std=c99
headers := ../include/problem2.1.h ../../include/hw0.h

common.o: common.c $(headers)
	$(CC) -c $< $(CFLAGS)

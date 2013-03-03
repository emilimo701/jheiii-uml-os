CC := gcc
CFLAGS := -I../include -I../../include --std=c99 -Wformat=0
headers := ../include/problem2.1.h ../../include/hw0.h

problem2.1: main_problem2.1.o ../common/common.o
	$(CC) -o $@ $^ $(CFLAGS)

main_problem2.1.o: main_problem2.1.c $(headers)
	$(CC) -c $< $(CFLAGS)



CC := gcc
CFLAGS := -I../include -I../../include --std=c99 -Wformat=0
headers := ../include/problem2.1.h ../../include/hw0.h

c2: child2.o ../common/common.o
	$(CC) -o $@ $^ $(CFLAGS)

child2.o: child2.c $(headers)
	$(CC) -c $< $(CFLAGS)

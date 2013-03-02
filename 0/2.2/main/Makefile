CC := gcc
CFLAGS := --std=c99

objects := main.o

# link
jdu: $(objects)
	gcc $^ -o $@

# pull in dependency info for *existing* .o files
-include $(objects:.o=.d)

# compile and generate dependency info
%.o: %.c
	gcc -c $(CFLAGS) $*.c -o $*.o
	gcc -MM $(CFLAGS) $*.c > $*.d

# remove compilation products
clean:
	rm -f jdu *.o *.d


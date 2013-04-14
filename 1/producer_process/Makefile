exename := producer_process
includes :=
libs := pthread

# This makefile assumes the following: --------------------------|
#  - all .c source files are in working directory                |
#  - at least one of the sourcefile (.c) names match the target  |
#     name (.exe or no extension) defined on the first line (see |
#     "Linking a single object file" in the GNU make manual)     |
# ---------------------------------------------------------------|

# I keep adding .exe by accident which kills the implicit rule search alg
exename := $(exename:.exe=)

CFLAGS = --std=c99 $(patsubst %,-I%,$(includes))
LDLIBS = $(patsubst %,-l%,$(libs))

COMPILE.c := $(strip $(COMPILE.c))
LINK.o := $(strip $(LINK.o))

# compile and link ALL files in current w.d.
sources := $(wildcard *.c)
objects := $(patsubst %.c,%.o,$(sources))

# link.
$(exename): $(objects)

# import dependency file rules from previous compilation, if they exist
-include $(sources:.c=.P)

#depdir = deps
#df = $(depdir)/$(*F)
#makedepend = gcc -M $(CFLAGS) -o $*.d $<

# override built-in implicit rule for %.o:%.c
%.o: %.c
	@gcc -M $(CFLAGS) -o $*.d $< ; \
           cp $*.d $*.P ; \
           sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' -e '/^$$/ d' -e 's/$$/ :/' < $*.d >> $*.P ; \
           rm -f $*.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<

.PHONY: clean
clean:
	$(RM) $(exename) *.o *.P *.d


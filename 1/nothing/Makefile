exename := nothing
this_makefile_alias :=
extra_include_paths :=
libs := pthread
sub_exe_dirs :=

# This makefile assumes the following: --------------------------|
#  - all .c source files are in working directory                |
#  - at least one of the sourcefile (.c) names match the target  |
#     name (.exe or no extension) defined on the first line (see |
#     "Linking a single object file" in the GNU make manual)     |
# ---------------------------------------------------------------|

# I keep adding .exe by accident which kills the implicit rule search alg
exename := $(exename:.exe=)

CFLAGS = --std=c99 $(patsubst %,-I%,$(extra_include_paths))
LDLIBS = $(patsubst %,-l%,$(libs))

COMPILE.c := $(strip $(COMPILE.c))
LINK.o := $(strip $(LINK.o))

# compile and link ALL files in current w.d.
sources := $(wildcard *.c)
objects := $(patsubst %.c,%.o,$(sources))

# the following two lines may be commented if no sub_exe_dirs
#$(this_makefile_alias): $(exename) $(sub_exe_dirs)
#.PHONY: $(this_makefile_alias)

# link.
$(exename): $(objects)

# rule/recipe for any specified subdirectory Makefiles
$(sub_exe_dirs):
	@$(MAKE) --no-print-directory -C $@
.PHONY: $(sub_exe_dirs)

# import dependency file rules from previous compilation, if they exist
-include $(sources:.c=.P)

# definitions for dependency autogeneration
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


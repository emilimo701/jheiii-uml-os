export CC := gcc
export CFLAGS := -I../include --std=c99 -Wformat=0 
export MAKEFLAGS := --no-print-directory
export SUBDIRS := main child2 common

#%.o : %.c
#	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) $(MAKEFLAGS) -C $@

main: common
child2: common

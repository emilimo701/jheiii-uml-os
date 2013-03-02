export CC := gcc
export CFLAGS := -I../include --std=c99 -Wformat=0 
export MAKEFLAGS := --no-print-directory -e
export SUBDIRS := main

#%.o : %.c
#	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: subdirs $(SUBDIRS)

subdirs: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) $(MAKEFLAGS) -C $@


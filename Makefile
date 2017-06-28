CC = gcc
CFLAGS = -std=gnu11 -Wall -g -pthread
OBJS = threadpool.o benchmark.o

.PHONY: all clean

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

GIT_HOOKS := .git/hooks/applied
all: $(GIT_HOOKS) benchmark

$(GIT_HOOKS):
	@scripts/install-git-hooks
	@echo

deps := $(OBJS:%.o=.%.o.d)
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -MMD -MF .$@.d -c $<

benchmark: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) -rdynamic

# Default variables for auto testing
THREAD_NUM ?= 4

clean:
	rm -f $(OBJS) benchmark
	@rm -rf $(deps)

-include $(deps)

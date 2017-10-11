all: main.c
	gcc -std=c99 -g -Wall -o sim_ds main.c cycle.c decode.c dispatch.c execute.c fetch.c issue.c regRead.c retire.c rrename.c writeback.c cache.c -lm

clean:
	$(RM) sim_ds

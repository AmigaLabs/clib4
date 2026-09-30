#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ucontext.h>

ucontext_t mainctx = {};
ucontext_t *curthr = &mainctx;
ucontext_t *threads = NULL;
size_t thrcount = 0;

void
yieldto(ucontext_t *target)
{
	ucontext_t *oldthr = curthr;
	curthr = target;

	swapcontext(oldthr, curthr);
}

void
yield(void)
{
	ucontext_t *newthr = NULL;
	size_t alive = 0;

	/* Count the schedulable threads (not completed, not the current one).
	 * If none is left there is nothing to switch to: returning lets the
	 * caller finish and follow its uc_link chain, avoiding an infinite
	 * loop in the selection below. */
	for (size_t i = 0; i < thrcount; i++) {
		if (!threads[i].uc_flags && &threads[i] != curthr)
			alive++;
	}
	if (alive == 0)
		return;

	/* we set uc_flags to non-zero to signal thread completion. */
	do
		newthr = &threads[random() % thrcount];
	while (newthr == curthr || newthr->uc_flags);

	yieldto(newthr);
}

void
worker(size_t multiple)
{
	size_t accum = 1;

	for (size_t i = 0; i < 10; i++)
	{
		accum += (multiple * i);

		printf("[%p] accumulated %zu\n", curthr, accum);
		yield();
	}

	/* mark thread as completed, so we don't return here */
	curthr->uc_flags = 1;
}

void
create(size_t multiple)
{
	ucontext_t *cursor, *tmp;

	tmp = realloc(threads, sizeof(*threads) * (thrcount + 1));
	if (tmp == NULL) {
		perror("realloc");
		exit(EXIT_FAILURE);
	}
	threads = tmp;

	cursor = &threads[thrcount];
	memset(cursor, '\0', sizeof *cursor);

	/* initialize the new thread's values to our current context */
	getcontext(cursor);

	/* set up uc_link (repaired in main after all reallocations) */
	cursor->uc_link = NULL;

	/* set up a stack */
	cursor->uc_stack.ss_size = 8192;
	cursor->uc_stack.ss_sp = calloc(1, cursor->uc_stack.ss_size);

	/* set up the function call */
	makecontext(cursor, worker, 1, multiple);

	thrcount += 1;
}

int
main(int argc, const char *argv[])
{
	/* Fixed seed: deterministic scheduling, comparable across platforms. */
	srandom(42);

	getcontext(&mainctx);

	for (size_t i = 1; i < 4; i++)
		create(i);

	/* realloc() in create() may have moved the threads array, invalidating
	 * any uc_link set earlier: (re)build the chain now that the array is
	 * stable. Thread i returns to thread i-1, thread 0 to main. */
	for (size_t i = 0; i < thrcount; i++)
		threads[i].uc_link = i > 0 ? &threads[i - 1] : &mainctx;

	/* start the threads off by yielding to the last one */
	yieldto(&threads[thrcount - 1]);

	return EXIT_SUCCESS;
}
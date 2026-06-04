#ifdef UNIT_TESTS
#ifndef UNIT_TEST_FUNCTIONS
#define UNIT_TEST_FUNCTIONS

typedef void (*test_fn_t)(void);

typedef struct
{
	const char *name;
	test_fn_t fn;

} test_case_t;

void task_queue_tests();
void scheduler_tests();

#endif
#endif

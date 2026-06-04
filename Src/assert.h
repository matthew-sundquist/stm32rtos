
#include <stm32l4xx.h>

#ifdef UNIT_TESTS
extern uint32_t num_errors;
#define ASSERT(x)			\
	do						\
	{						\
		if (!(x))			\
		{					\
			num_errors++;	\
		}					\
	} while (0)
#elif DEBUG
#define ASSERT(x)			\
	do 						\
	{						\
		if (!(x))			\
		{					\
			__BKPT(0);		\
		}					\
	} while(0)
#else
#define ASSERT(x) ((void)0)
#endif

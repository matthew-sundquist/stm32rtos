

#ifdef DEBUG
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

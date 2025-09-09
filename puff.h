#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_monitor
{
	int	limit;
	pthread_mutex_t	finish_mutex;
}	t_monitor;

#endif
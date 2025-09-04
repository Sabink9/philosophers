#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_params
{
	int				number_of_philosophers;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				number_of_times_each_philosopher_must_eat;
}					t_params;

typedef struct s_shared
{
	int				finished_count;
	pthread_mutex_t	finished_mutex;

	int				someone_dead;
	pthread_mutex_t	death_mutex;

	pthread_mutex_t print_mutex; // 🔹 protège les affichages
	pthread_mutex_t *forks;      // 🔹 stocker les forks pour free/destroy
}					t_shared;

typedef struct s_philo
{
	int				id;
	pthread_t		thread;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	t_params		*params;
	t_shared		*shared;
	long			last_meal_time;
	int				meals_eaten;
	pthread_mutex_t	meal_mutex;
}					t_philo;

#endif
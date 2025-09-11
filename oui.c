#include "philosophers.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}
int	init_philo(t_data *data)
{
	int	i;

	data->forks = malloc(sizeof(pthread_mutex_t) * data->nb_philo);
	if (!data->forks)
		return (1);
	i = 0;
	while (i < data->nb_philo)
	{
		pthread_mutex_init(&data->forks[i], NULL);
		i++;
	}
	data->philos = malloc(sizeof(t_philo) * data->nb_philo);
	if (!data->philos)
		return (1);
	i = 0;
	while (i < data->nb_philo)
	{
		data->philos[i].id = i + 1;
		data->philos[i].left_fork = &data->forks[i];
		data->philos[i].right_fork = &data->forks[(i + 1) % data->nb_philo];
		data->philos[i].meals_eaten = 0;
		data->philos[i].last_meal = 0;
		data->philos[i].data = data;
		i++;
	}
	pthread_mutex_init(&data->print_mutex, NULL);
	pthread_mutex_init(&data->meal_mutex, NULL);
	data->stop = 0;
	return (0);
}

int	can_eat(t_philo *philo)
{
	int		i;
	long	my_last;
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->meal_mutex);
	my_last = philo->last_meal;
	i = 0;
	while (i < data->nb_philo)
	{
		if (i != philo->id - 1 && data->philos[i].waiting)
		{
			if (data->philos[i].last_meal < my_last)
			{
				pthread_mutex_unlock(&data->meal_mutex);
				return (0); // quelqu’un a plus faim que moi
			}
		}
		i++;
	}
	pthread_mutex_unlock(&data->meal_mutex);
	return (1); // je peux manger
}

void	take_forks(t_philo *philo, long start_time)
{
	t_data	*data;

	data = philo->data;
	// Philosophe signale qu'il veut manger
	philo->waiting = 1;
	// Attente active : on attend que ce soit notre tour selon can_eat
	while (!can_eat(philo))
	{
		if (data->stop)
			return ;
		usleep(100);
	}
	if (data->stop) // check stop avant de prendre les fourchettes
		return ;
	// Prendre les fourchettes selon pair/impair
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(philo->right_fork);
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(philo->left_fork);
	}
	philo->waiting = 0;
	pthread_mutex_lock(&data->print_mutex);
	if (!data->stop)
	{
		printf("%ld %d has taken a fork\n", get_time_ms() - start_time,
				philo->id);
		printf("%ld %d has taken a fork\n", get_time_ms() - start_time,
				philo->id);
	}
	pthread_mutex_unlock(&data->print_mutex);
}

void	release_forks(t_philo *philo)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_unlock(philo->left_fork);
		pthread_mutex_unlock(philo->right_fork);
	}
	else
	{
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
	}
}

void	eat_philo(t_philo *philo, long start_time)
{
	t_data	*data;
	long	end;

	data = philo->data;
	take_forks(philo, start_time);
	if (data->stop)
	{
		release_forks(philo);
		return ;
	}
	pthread_mutex_lock(&data->meal_mutex);
	philo->last_meal = get_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&data->meal_mutex);
	// Affichage
	pthread_mutex_lock(&data->print_mutex);
	if (!data->stop)
		printf("%ld %d is eating\n", get_time_ms() - start_time, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	// Simulation repas, interruption possible si stop = 1
	end = get_time_ms() + data->time_to_eat;
	while (!data->stop && get_time_ms() < end)
		usleep(500);
	// Relâcher les fourchettes
	release_forks(philo);
}

void	sleep_philo(t_philo *philo, long start_time)
{
	t_data	*data;
	long	end;

	data = philo->data;
	pthread_mutex_lock(&data->print_mutex);
	if (!data->stop)
		printf("%ld %d is sleeping\n", get_time_ms() - start_time, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	pthread_mutex_unlock(&data->print_mutex);
	end = get_time_ms() + data->time_to_sleep;
	while (!data->stop && get_time_ms() < end)
		usleep(500);
	if (data->stop)
	{
		pthread_mutex_unlock(&data->print_mutex);
		return ;
	}
}

void	think_philo(t_philo *philo, long start_time)
{
	t_data	*data;
	long	end;

	data = philo->data;
	pthread_mutex_lock(&data->print_mutex);
	if (!data->stop)
		printf("%ld %d is thinking\n", get_time_ms() - start_time, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	end = get_time_ms() + 1; // ton usleep original de 100ms
	while (!data->stop && get_time_ms() < end)
		usleep(500);
	if (data->stop)
	{
		pthread_mutex_unlock(&data->print_mutex);
		return ;
	}
}

void	*routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;
	long	start_time;

	philo = arg;
	data = philo->data;
	start_time = get_time_ms();
	if (philo->id % 2 == 0)
		usleep(100 * philo->id);
	while (!data->stop)
	{
		if (!data->stop)
			eat_philo(philo, start_time);
		if (!data->stop)
			sleep_philo(philo, start_time);
		if (!data->stop)
			think_philo(philo, start_time);
	}
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		i;
	int		done;
	long	start_time;

	start_time = get_time_ms();
	data = arg;
	while (!data->stop)
	{
		i = 0;
		done = 0;
		while (i < data->nb_philo && !data->stop)
		{
			pthread_mutex_lock(&data->meal_mutex);
			if (get_time_ms() - data->philos[i].last_meal > data->time_to_die)
			{
				pthread_mutex_lock(&data->print_mutex);
				printf("%ld %d died\n", get_time_ms() - start_time,
						data->philos[i].id);
				pthread_mutex_unlock(&data->print_mutex);
				data->stop = 1;
			}
			if (data->must_eat > 0
				&& data->philos[i].meals_eaten >= data->must_eat)
				done++;
			pthread_mutex_unlock(&data->meal_mutex);
			i++;
		}
		if (data->must_eat > 0 && done == data->nb_philo)
		{
			pthread_mutex_lock(&data->print_mutex);
			usleep(200000);
			printf("%ld All philosophers have eaten %d meals each\n",
					get_time_ms() - start_time,
					data->must_eat);
			pthread_mutex_unlock(&data->print_mutex);
			data->stop = 1;
		}
		usleep(8000); // vérifier toutes les 0.5 ms
	}
	return (NULL);
}

int	main(int ac, char **av)
{
	t_data		data;
	pthread_t	*threads;
	pthread_t	monitor;
	int			i;

	if (ac != 5 && ac != 6)
	{
		printf("Usage: ./philo nb_philo time_to_die time_to_eat time_to_sleep [must_eat]\n");
		return (1);
	}
	parse_args(ac, av, &data);
	if (init_philo(&data))
	{
		printf("Erreur lors de l'initialisation des philosophes\n");
		return (1);
	}
	threads = malloc(sizeof(pthread_t) * data.nb_philo);
	if (!threads)
		return (1);
	i = 0;
	while (i < data.nb_philo)
	{
		// on initialise last_meal au moment du lancement
		data.philos[i].last_meal = get_time_ms();
		data.philos[i].waiting = 0;
		pthread_create(&threads[i], NULL, routine, &data.philos[i]);
		i++;
	}
	pthread_create(&monitor, NULL, monitor_routine, &data);
	i = 0;
	while (i < data.nb_philo)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_join(monitor, NULL);
	i = 0;
	while (i < data.nb_philo)
	{
		pthread_mutex_destroy(&data.forks[i]);
		i++;
	}
	pthread_mutex_destroy(&data.print_mutex);
	pthread_mutex_destroy(&data.meal_mutex);
	free(data.forks);
	free(data.philos);
	free(threads);
	return (0);
}

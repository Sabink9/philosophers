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

void	take_forks(t_philo *philo, t_data *data, long start_time)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(&data->print_mutex);
		printf("%ld %d has taken a fork\n", get_time_ms() - start_time,
				philo->id);
		pthread_mutex_unlock(&data->print_mutex);
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(&data->print_mutex);
		printf("%ld %d has taken a fork\n", get_time_ms() - start_time,
				philo->id);
		pthread_mutex_unlock(&data->print_mutex);
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(&data->print_mutex);
		printf("%ld %d has taken a fork\n", get_time_ms() - start_time,
				philo->id);
		pthread_mutex_unlock(&data->print_mutex);
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(&data->print_mutex);
		printf("%ld %d has taken a fork\n", get_time_ms() - start_time,
				philo->id);
		pthread_mutex_unlock(&data->print_mutex);
	}
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

	data = philo->data;
	take_forks(philo, data, start_time);
	pthread_mutex_lock(&data->meal_mutex);
	philo->last_meal = get_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&data->meal_mutex);
	// Affichage
	pthread_mutex_lock(&data->print_mutex);
	printf("%ld %d is eating\n", get_time_ms() - start_time, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	// Simuler le repas
	usleep(data->time_to_eat * 1000);
	release_forks(philo);
}

void	sleep_philo(t_philo *philo, long start_time)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->print_mutex);
	printf("%ld %d is sleeping\n", get_time_ms() - start_time, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	usleep(data->time_to_sleep * 1000);
}

void	think_philo(t_philo *philo, long start_time)
{
	t_data	*data;

	data = philo->data;
	pthread_mutex_lock(&data->print_mutex);
	printf("%ld %d is thinking\n", get_time_ms() - start_time, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	usleep(100);
}

int	get_next_to_eat(t_data *data)
{
	int		i;
	int		next_id;
	long	min_last_meal;

	if (data->nb_philo == 0)
		return (0);
	i = 0;
	next_id = data->philos[0].id;
	min_last_meal = data->philos[0].last_meal;
	while (i < data->nb_philo)
	{
		if (data->philos[i].last_meal < min_last_meal)
		{
			min_last_meal = data->philos[i].last_meal;
			next_id = data->philos[i].id;
		}
		i++;
	}
	return (next_id);
}

void	*routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;
	long	start_time;
	int		fs;

	philo = arg;
	data = philo->data;
	start_time = get_time_ms();
	think_philo(philo, start_time);
	if (philo->id % 2 == 0)
		usleep(100 * philo->id);
	fs = 0;
	while (!data->stop)
	{
		while (1)
		{
			pthread_mutex_lock(&data->meal_mutex);
			if (data->next_to_eat == philo->id || data->stop)
			{
				pthread_mutex_unlock(&data->meal_mutex);
				break ;
			}
			pthread_mutex_unlock(&data->meal_mutex);
			usleep(100);
		}
		if (!fs)
		{
			usleep(100);
			fs = 1;
		}
		eat_philo(philo, start_time);
		pthread_mutex_lock(&data->meal_mutex);
		data->next_to_eat = get_next_to_eat(data);
		pthread_mutex_unlock(&data->meal_mutex);
		sleep_philo(philo, start_time);
		think_philo(philo, start_time);
	}
	return (NULL);
}

int	main(int ac, char **av)
{
	t_data data;
	pthread_t *threads;
	int i;

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

	// Initialiser le philosophe qui mange en premier
	data.next_to_eat = 1;

	threads = malloc(sizeof(pthread_t) * data.nb_philo);
	if (!threads)
		return (1);

	i = 0;
	while (i < data.nb_philo)
	{
		pthread_create(&threads[i], NULL, routine, &data.philos[i]);
		i++;
	}

	i = 0;
	while (i < data.nb_philo)
	{
		pthread_join(threads[i], NULL);
		i++;
	}

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
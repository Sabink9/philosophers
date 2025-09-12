#include "philosophers.h"

void	*routine(void *arg)
{
	t_philo	*philo;
	t_data	*data;
	long	start_time;

	philo = arg;
	data = philo->data;
	start_time = get_time_ms();
	if (philo->id % 2 == 0)
		usleep(50 * philo->id);
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

void	handle_dead(t_data *data, long start_time, int i, int *done)
{
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
		if (data->must_eat > 0 && data->philos[i].meals_eaten >= data->must_eat)
			(*done)++;
		pthread_mutex_unlock(&data->meal_mutex);
		i++;
	}
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
		handle_dead(data, start_time, i, &done);
		if (data->must_eat > 0 && done == data->nb_philo)
		{
			pthread_mutex_lock(&data->print_mutex);
			usleep(data->time_to_eat * 1000);
			printf("%ld All philosophers have eaten %d meals each\n",
					get_time_ms() - start_time,
					data->must_eat);
			pthread_mutex_unlock(&data->print_mutex);
			data->stop = 1;
		}
		usleep(5000);
	}
	return (NULL);
}

int	create_threads(pthread_t **threads, t_data *data, pthread_t *monitor)
{
	int	i;

	i = 0;
	*threads = malloc(sizeof(pthread_t) * data->nb_philo);
	if (!*threads)
		return (1);
	while (i < data->nb_philo)
	{
		data->philos[i].last_meal = get_time_ms();
		data->philos[i].waiting = 0;
		pthread_create(&(*threads)[i], NULL, routine, &data->philos[i]);
		i++;
	}
	if (pthread_create(monitor, NULL, monitor_routine, data) != 0)
		return (1);
	i = 0;
	while (i < data->nb_philo)
	{
		pthread_join((*threads)[i], NULL);
		i++;
	}
	pthread_join(*monitor, NULL);
	return (0);
}

int	main(int ac, char **av)
{
	t_data		data;
	pthread_t	*threads;
	pthread_t	monitor;

	threads = NULL;
	if (parse_args(ac, av, &data))
		return (1);
	if (data.nb_philo <= 0 || data.time_to_die <= 0 || data.time_to_eat <= 0
		|| data.time_to_sleep <= 0)
		printf("Erreur : les valeurs doivent être supérieures à 0.\n");
	if (data.must_eat == 0)
		printf("Erreur : must_eat doit être supérieur à 0 ou absent.\n");
	if (data.nb_philo <= 0 || data.time_to_die <= 0 || data.time_to_eat <= 0
		|| data.time_to_sleep <= 0 || data.must_eat == 0)
			return (1);
	if (init_philo(&data))
	{
		printf("Erreur lors de l'initialisation des philosophes\n");
		return (1);
	}
	if (create_threads(&threads, &data, &monitor))
		return (1);
	cleanup(&data, threads);
	return (0);
}

#include "philosophers.h"

int	ft_atoi(char *str)
{
	int	res;
	int	i;

	res = 0;
	i = 0;
	while (str[i] >= '0' && str[i] <= '9')
	{
		res = res * 10 + (str[i] - '0');
		i++;
	}
	if (str[i] != '\0')
		return (-1);
	return (res);
}

void	parse_args(int ac, char **av, t_params *params)
{
	params->number_of_philosophers = ft_atoi(av[1]);
	params->time_to_die = ft_atoi(av[2]);
	params->time_to_eat = ft_atoi(av[3]);
	params->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		params->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	else
		params->number_of_times_each_philosopher_must_eat = -1;
}

long	current_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
int	ft_strcmp(const char *s1, const char *s2)
{
	int	i;

	i = 0;
	while (s1[i] && s2[i] && (unsigned char)s1[i] == (unsigned char)s2[i])
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
}

void	print_status(t_philo *philo, char *message, long start_time)
{
	long	time;

	pthread_mutex_lock(&philo->shared->print_mutex);
	if (!philo->shared->someone_dead || ft_strcmp(message, "died") == 0)
	{
		time = current_time_ms() - start_time;
		printf("%ld %d %s\n", time, philo->id, message);
	}
	pthread_mutex_unlock(&philo->shared->print_mutex);
}

int	finish_eat(t_philo *philo, t_params *params)
{
	if (params->number_of_times_each_philosopher_must_eat == -1)
		return (1);
	if (philo->meals_eaten == params->number_of_times_each_philosopher_must_eat)
		return (0);
	else
		return (1);
}

void	take_fork(t_philo *philo, long start_time)
{
	if (philo->id % 2 == 0)
	{
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork", start_time);
		usleep(50);
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a fork", start_time);
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		print_status(philo, "has taken a fork", start_time);
		usleep(50);
		pthread_mutex_lock(philo->left_fork);
		print_status(philo, "has taken a fork", start_time);
	}
}

void	eating(t_philo *philo, long start_time)
{
	pthread_mutex_lock(&philo->meal_mutex);
	philo->last_meal_time = current_time_ms();
	philo->meals_eaten++;
	pthread_mutex_unlock(&philo->meal_mutex);
	print_status(philo, "is eating", start_time);
	usleep(philo->params->time_to_eat * 1000);
}

int	is_dead(t_philo *philo, long start_time)
{
	long	now;

	now = current_time_ms();
	if ((now - philo->last_meal_time) > philo->params->time_to_die)
	{
		print_status(philo, "died", start_time);
		return (1);
	}
	return (0);
}

void	*routine_philo(void *arg)
{
	t_philo	*philo;
	long	start_time;

	philo = (t_philo *)arg;
	start_time = current_time_ms();
	usleep(100 * philo->id);
	while (1)
	{
		pthread_mutex_lock(&philo->shared->death_mutex);
		if (philo->shared->someone_dead)
		{
			pthread_mutex_unlock(&philo->shared->death_mutex);
			break ;
		}
		pthread_mutex_unlock(&philo->shared->death_mutex);
		take_fork(philo, start_time);
		eating(philo, start_time);
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		if (!finish_eat(philo, philo->params))
		{
			pthread_mutex_lock(&philo->shared->finished_mutex);
			philo->shared->finished_count++;
			pthread_mutex_unlock(&philo->shared->finished_mutex);
			break ;
		}
		print_status(philo, "is sleeping", start_time);
		usleep(philo->params->time_to_sleep * 1000);
		print_status(philo, "is thinking", start_time);
	}
	return (NULL);
}

int	init_philos(t_params *params, t_shared *shared, t_philo **philos_out)
{
	t_philo	*philos;
	int		i;

	philos = malloc(sizeof(t_philo) * params->number_of_philosophers);
	if (!philos)
		return (-1);
	shared->forks = malloc(sizeof(pthread_mutex_t)
			* params->number_of_philosophers);
	if (!shared->forks)
	{
		free(philos);
		return (-1);
	}
	i = 0;
	while (i < params->number_of_philosophers)
		pthread_mutex_init(&shared->forks[i++], NULL);
	i = 0;
	while (i < params->number_of_philosophers)
	{
		philos[i].id = i + 1;
		philos[i].params = params;
		philos[i].left_fork = &shared->forks[i];
		philos[i].right_fork = &shared->forks[(i + 1)
			% params->number_of_philosophers];
		philos[i].last_meal_time = current_time_ms();
		philos[i].meals_eaten = 0;
		philos[i].shared = shared;
		pthread_mutex_init(&philos[i].meal_mutex, NULL);
		i++;
	}
	*philos_out = philos;
	return (0);
}

void	*monitor_routine(void *arg)
{
	t_philo		*philos;
	t_shared	*shared;
	long		start_time;
	int			i;

	philos = (t_philo *)arg;
	shared = philos[0].shared;
	start_time = current_time_ms();
	while (1)
	{
		i = 0;
		while (i < philos[0].params->number_of_philosophers)
		{
			if (is_dead(&philos[i], start_time))
			{
				pthread_mutex_lock(&shared->death_mutex);
				shared->someone_dead = 1;
				pthread_mutex_unlock(&shared->death_mutex);
				return (NULL);
			}
			i++;
		}
		pthread_mutex_lock(&shared->finished_mutex);
		if (shared->finished_count == philos[0].params->number_of_philosophers)
		{
			pthread_mutex_unlock(&shared->finished_mutex);
			break ;
		}
		pthread_mutex_unlock(&shared->finished_mutex);
		usleep(1000);
	}
	return (NULL);
}

int	main(int ac, char **av)
{
	t_params	params;
	t_shared	shared;
	t_philo		*philos;
	int			i;
	pthread_t	monitor_thread;

	if (ac < 5 || ac > 6)
		return (1);
	parse_args(ac, av, &params);
	shared.finished_count = 0;
	shared.someone_dead = 0;
	pthread_mutex_init(&shared.finished_mutex, NULL);
	pthread_mutex_init(&shared.death_mutex, NULL);
	pthread_mutex_init(&shared.print_mutex, NULL);
	philos = NULL;
	if (init_philos(&params, &shared, &philos) != 0)
		return (1);
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_create(&philos[i].thread, NULL, routine_philo, &philos[i]);
		i++;
	}
	pthread_create(&monitor_thread, NULL, monitor_routine, philos);
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}
	pthread_join(monitor_thread, NULL);
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_mutex_destroy(&shared.forks[i]);
		pthread_mutex_destroy(&philos[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(&shared.finished_mutex);
	pthread_mutex_destroy(&shared.death_mutex);
	pthread_mutex_destroy(&shared.print_mutex);
	free(shared.forks);
	free(philos);
	printf("Simulation terminée.\n");
	return (0);
}

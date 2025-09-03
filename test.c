#include "philosophers.h"

int	ft_atoi(char *str)
{
	int	res;
	int	i;

	i = 0;
	res = 0;
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

// Affiche le statut d'un philosophe
void	print_status(t_philo *philo, char *message, long start_time)
{
	long	time;

	pthread_mutex_lock(&philo->meal_mutex);
	time = current_time_ms() - start_time;
	printf("%ld %d %s\n", time, philo->id + 1, message);
	pthread_mutex_unlock(&philo->meal_mutex);
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

// Routine du philosophe
void	*routine_philo(void *arg)
{
	t_philo	*philo;
	long	start_time;

	philo = (t_philo *)arg;
	start_time = current_time_ms();
	usleep(100 * philo->id); // petit décalage initial
	while (1)
	{
		// Prendre les fourchettes (stratégie anti-deadlock)
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
		// Manger
		pthread_mutex_lock(&philo->meal_mutex);
		philo->last_meal_time = current_time_ms();
		philo->meals_eaten++;
		pthread_mutex_unlock(&philo->meal_mutex);
		print_status(philo, "is eating", start_time);
		usleep(philo->params->time_to_eat * 1000);
		// Reposer les fourchettes
		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);
		// Vérifier si le philosophe a fini de manger
		if (!finish_eat(philo, philo->params))
		{
			// Incrémenter le compteur partagé
			pthread_mutex_lock(&philo->shared->finished_mutex);
			philo->shared->finished_count++;
			pthread_mutex_unlock(&philo->shared->finished_mutex);
			break ; // Sortir de la boucle
		}
		// Dormir
		print_status(philo, "is sleeping", start_time);
		usleep(philo->params->time_to_sleep * 1000);
		// Penser
		print_status(philo, "is thinking", start_time);
	}
	return (NULL);
}

// Initialisation des philosophes et des fourchettes
int	init_philos(t_params params, t_philo **philos_out)
{
	t_philo			*philos;
	pthread_mutex_t	*forks;
	int				i;

	philos = malloc(sizeof(t_philo) * params.number_of_philosophers);
	if (!philos)
		return (-1);
	forks = malloc(sizeof(pthread_mutex_t) * params.number_of_philosophers);
	if (!forks)
	{
		free(philos);
		return (-1);
	}
	// Initialiser les mutex des fourchettes
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	// Initialiser les philosophes
	i = 0;
	while (i < params.number_of_philosophers)
	{
		philos[i].id = i;
		philos[i].params = &params;
		philos[i].left_fork = &forks[i];
		philos[i].right_fork = &forks[(i + 1) % params.number_of_philosophers];
		philos[i].last_meal_time = current_time_ms();
		philos[i].meals_eaten = 0;
		pthread_mutex_init(&philos[i].meal_mutex, NULL);
		i++;
	}
	*philos_out = philos;
	return (0);
}

// Main
int	main(int ac, char **av)
{
	t_params params;
	parse_args(ac, av, &params);

	// 1️⃣ Allocation des philosophes et des forks
	t_philo *philos = malloc(sizeof(t_philo) * params.number_of_philosophers);
	pthread_mutex_t *forks = malloc(sizeof(pthread_mutex_t)
			* params.number_of_philosophers);
	if (!philos || !forks)
		return (1);

	// 2️⃣ Initialisation de la structure partagée
	t_shared shared;
	shared.finished_count = 0;
	pthread_mutex_init(&shared.finished_mutex, NULL);

	// 3️⃣ Initialisation des forks et des philosophes
	int i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_mutex_init(&forks[i], NULL);

		philos[i].id = i + 1; // id commence à 1
		philos[i].left_fork = &forks[i];
		philos[i].right_fork = &forks[(i + 1) % params.number_of_philosophers];
		philos[i].params = &params;
		philos[i].shared = &shared; // pointeur vers la structure partagée
		philos[i].meals_eaten = 0;
		pthread_mutex_init(&philos[i].meal_mutex, NULL);

		i++;
	}

	// 4️⃣ Création des threads
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_create(&philos[i].thread, NULL, routine_philo, &philos[i]);
		i++;
	}

	// 5️⃣ Attente de la fin de tous les threads
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_join(philos[i].thread, NULL);
		i++;
	}

	// 6️⃣ Nettoyage
	i = 0;
	while (i < params.number_of_philosophers)
	{
		pthread_mutex_destroy(&forks[i]);
		pthread_mutex_destroy(&philos[i].meal_mutex);
		i++;
	}
	pthread_mutex_destroy(&shared.finished_mutex);

	free(philos);
	free(forks);

	printf("Simulation terminée : tous les philosophes ont fini de manger.\n");
	return (0);
}
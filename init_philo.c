#include "philosophers.h"


void	init_philo(t_params params)
{
	t_philo	*philos;
	pthread_mutex_t	*forks;
	int	i;

	philos = malloc(sizeof(t_philo) * params.number_of_philosophers);
	if (!philos)
		return (-1);
	forks = malloc(sizeof(pthread_mutex_t) * params.number_of_philosophers);
	if (!forks)
		return (-1);
	i = 1;
	while (i <= params.number_of_philosophers)
	{
		pthread_mutex_init(&forks[i], NULL);
		i++;
	}
	i = 1;
	while (i <= params.number_of_philosophers)
	{
		philos[i].id = i;
		philos[i].left_fork = &forks[i];
		philos[i].right_fork = &forks[(i + 1) % params.number_of_philosophers];
		i++;
	}
	return (0);
}
void	*routine_philo(void *arg)
{
	t_philo *philo;

	philo = (t_philo *)arg;
	while (1)
	{
		
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(philo->right_fork);

		// manger (mettre à jour last_meal_time, meals_eaten, usleep(time_to_eat))
		// afficher un message

		pthread_mutex_unlock(philo->right_fork);
		pthread_mutex_unlock(philo->left_fork);

		// dormir (usleep(time_to_sleep))
		// afficher un message

		// penser (petite pause, ou juste passer au tour suivant)
	}
	return (NULL);
}

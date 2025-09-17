/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saciurus <saciurus@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 14:24:43 by saciurus          #+#    #+#             */
/*   Updated: 2025/09/16 18:59:55 by saciurus         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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

int	check_and_join(pthread_t *threads, t_data *data, pthread_t *monitor,
		int created)
{
	int	i;

	if (pthread_create(monitor, NULL, monitor_routine, data) != 0)
	{
		i = 0;
		while (i < created)
		{
			pthread_join(threads[i], NULL);
			i++;
		}
		return (1);
	}
	i = 0;
	while (i < created)
	{
		pthread_join(threads[i], NULL);
		i++;
	}
	pthread_join(*monitor, NULL);
	return (0);
}

int	create_threads(pthread_t **threads, t_data *data, pthread_t *monitor)
{
	int	i;
	int	created;

	*threads = malloc(sizeof(pthread_t) * data->nb_philo);
	if (!*threads)
		return (1);
	i = 0;
	created = 0;
	while (i < data->nb_philo)
	{
		data->philos[i].last_meal = get_time_ms();
		data->philos[i].waiting = 0;
		if (pthread_create(&(*threads)[i], NULL, routine,
			&data->philos[i]) != 0)
		{
			while (created-- > 0)
				pthread_join((*threads)[created], NULL);
			return (1);
		}
		created++;
		i++;
	}
	if (check_and_join(*threads, data, monitor, created))
		return (1);
	return (0);
}

int	check_parse(t_data *data)
{
	if (data->nb_philo <= 0 || data->time_to_die <= 0 || data->time_to_eat <= 0
		|| data->time_to_sleep <= 0)
	{
		printf("Erreur : les valeurs doivent être supérieures à 0.\n");
		return (1);
	}
	if (data->must_eat == 0)
	{
		printf("Erreur : must_eat doit être supérieur à 0 ou absent.\n");
		return (1);
	}
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
	if (check_parse(&data))
		return (1);
	if (init_philo(&data))
	{
		printf("Erreur lors de l'initialisation des philosophes\n");
		cleanup(&data, threads);
		return (1);
	}
	if (create_threads(&threads, &data, &monitor))
	{
		cleanup(&data, threads);
		return (1);
	}
	cleanup(&data, threads);
	return (0);
}

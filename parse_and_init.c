/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse_and_init.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saciurus <saciurus@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 14:20:02 by saciurus          #+#    #+#             */
/*   Updated: 2025/09/16 18:29:44 by saciurus         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	attribut_params(int ac, char **av, t_data *params)
{
	params->nb_philo = ft_atoi(av[1]);
	params->time_to_die = ft_atoi(av[2]);
	params->time_to_eat = ft_atoi(av[3]);
	params->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		params->must_eat = ft_atoi(av[5]);
	else
		params->must_eat = -1;
}

int	parse_args(int ac, char **av, t_data *params)
{
	int	i;

	if (ac != 5 && ac != 6)
	{
		printf("usage : nb_philo timetodie timetoeat timetosleep [musteat]\n");
		return (1);
	}
	i = 1;
	while (i < ac)
	{
		if (!is_positive_number(av[i]))
		{
			printf("Erreur tout les args doivent etre des entiers positifs\n");
			return (1);
		}
		i++;
	}
	attribut_params(ac, av, params);
	return (0);
}

void	fill_data(t_data *data, int i)
{
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
	fill_data(data, i);
	pthread_mutex_init(&data->print_mutex, NULL);
	pthread_mutex_init(&data->meal_mutex, NULL);
	data->stop = 0;
	return (0);
}

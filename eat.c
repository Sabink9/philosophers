/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   eat.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saciurus <saciurus@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 15:08:48 by saciurus          #+#    #+#             */
/*   Updated: 2025/09/12 15:21:56 by saciurus         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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
				return (0);
			}
		}
		i++;
	}
	pthread_mutex_unlock(&data->meal_mutex);
	return (1);
}

void	print_actions(t_philo *philo, long timestamp)
{
		printf("%ld %d has taken a fork\n", timestamp, philo->id);
		printf("%ld %d has taken a fork\n", timestamp, philo->id);
		printf("%ld %d is eating\n", timestamp, philo->id);
}

void	eat_philo(t_philo *philo, long start_time)
{
	t_data	*data;
	long	end;
	long	timestamp;

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
	timestamp = get_time_ms() - start_time;
	pthread_mutex_lock(&data->print_mutex);
	if (!data->stop)
		print_actions(philo, timestamp);
	pthread_mutex_unlock(&data->print_mutex);
	end = get_time_ms() + data->time_to_eat;
	while (!data->stop && get_time_ms() < end)
		usleep(500);
	release_forks(philo);
}

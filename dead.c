/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dead.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saciurus <saciurus@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/16 18:30:06 by saciurus          #+#    #+#             */
/*   Updated: 2025/09/16 18:59:34 by saciurus         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

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

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sleep_and_think.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saciurus <saciurus@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 15:12:24 by saciurus          #+#    #+#             */
/*   Updated: 2025/09/12 15:12:38 by saciurus         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	sleep_philo(t_philo *philo, long start_time)
{
	t_data	*data;
	long	end;
	long	timestamp;

	data = philo->data;
	pthread_mutex_lock(&data->print_mutex);
	timestamp = get_time_ms() - start_time;
	if (!data->stop)
		printf("%ld %d is sleeping\n", timestamp, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	end = get_time_ms() + data->time_to_sleep;
	while (!data->stop && get_time_ms() < end)
		usleep(500);
	if (data->stop)
		return ;
}

void	think_philo(t_philo *philo, long start_time)
{
	t_data	*data;
	long	end;
	long	timestamp;

	data = philo->data;
	pthread_mutex_lock(&data->print_mutex);
	timestamp = get_time_ms() - start_time;
	if (!data->stop)
		printf("%ld %d is thinking\n", timestamp, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	end = get_time_ms();
	while (!data->stop && get_time_ms() < end)
		usleep(500);
	if (data->stop)
		return ;
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   forks.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saciurus <saciurus@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/12 15:04:42 by saciurus          #+#    #+#             */
/*   Updated: 2025/09/12 15:22:21 by saciurus         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "philosophers.h"

void	lonely_philo(t_philo *philo, t_data *data, long start_time)
{
	pthread_mutex_lock(philo->left_fork);
	pthread_mutex_lock(&data->print_mutex);
	printf("%ld %d has taken a fork\n", get_time_ms() - start_time, philo->id);
	pthread_mutex_unlock(&data->print_mutex);
	while (!data->stop)
	{
		if (get_time_ms() - philo->last_meal >= data->time_to_die)
		{
			pthread_mutex_lock(&data->print_mutex);
			printf("%ld %d died\n", get_time_ms() - start_time, philo->id);
			pthread_mutex_unlock(&data->print_mutex);
			data->stop = 1;
		}
		usleep(100);
	}
	pthread_mutex_unlock(philo->left_fork);
	return ;
}

void	take_forks(t_philo *philo, long start_time)
{
	t_data	*data;

	data = philo->data;
	philo->waiting = 1;
	while (!can_eat(philo))
	{
		if (data->stop)
			return ;
		usleep(100);
	}
	if (data->stop)
		return ;
	if (philo->data->nb_philo == 1)
		lonely_philo(philo, data, start_time);
	else if (philo->id % 2 == 0 && philo->data->nb_philo != 1)
	{
		pthread_mutex_lock(philo->left_fork);
		pthread_mutex_lock(philo->right_fork);
	}
	else
	{
		pthread_mutex_lock(philo->right_fork);
		pthread_mutex_lock(philo->left_fork);
	}
	philo->waiting = 0;
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

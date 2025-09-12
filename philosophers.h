/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   philosophers.h                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: saciurus <saciurus@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/09 17:10:03 by saciurus          #+#    #+#             */
/*   Updated: 2025/09/12 15:49:20 by saciurus         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PHILOSOPHERS_H
# define PHILOSOPHERS_H

# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <sys/time.h>
# include <unistd.h>

typedef struct s_philo
{
	int				id;
	int				meals_eaten;
	long			last_meal;
	int				waiting;
	pthread_mutex_t	*left_fork;
	pthread_mutex_t	*right_fork;
	struct s_data	*data;
}					t_philo;

typedef struct s_data
{
	int				nb_philo;
	long			time_to_die;
	long			time_to_eat;
	long			time_to_sleep;
	int				must_eat;
	int				stop;
	t_philo			*philos;
	pthread_mutex_t	*forks;
	pthread_mutex_t	print_mutex;
	pthread_mutex_t	meal_mutex;
}					t_data;

int				parse_args(int ac, char **av, t_data *params);
int					init_philo(t_data *data);
int					can_eat(t_philo *philo);
void				eat_philo(t_philo *philo, long start_time);
void				take_forks(t_philo *philo, long start_time);
void				release_forks(t_philo *philo);
void	sleep_philo(t_philo *philo, long start_time);
void	think_philo(t_philo *philo, long start_time);
int					ft_atoi(char *str);
long				get_time_ms(void);
int					is_positive_number(const char *str);
void				cleanup(t_data *data, pthread_t *threads);

#endif
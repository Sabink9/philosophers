#include "philosophers.h"


void	parse_args(int ac, char **av, t_data *params)
{
	int	i;

	if (ac != 5 && ac != 6)
	{
		printf("Usage: ./philo nb_philo time_to_die time_to_eat time_to_sleep [must_eat]\n");
		return (1);
	}
	i = 1;
	while (i < ac)
	{
		if (!is_positive_number(av[i]))
		{
			printf("Erreur tout les args doivent etre des entiers positifs");
			return (1);
		}
	}
	params->nb_philo = ft_atoi(av[1]);
	params->time_to_die = ft_atoi(av[2]);
	params->time_to_eat = ft_atoi(av[3]);
	params->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		params->must_eat = ft_atoi(av[5]);
	else
		params->must_eat = -1;
}

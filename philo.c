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
void	parse_args(int ac, char **av, t_data *params)
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

// int	main(int ac, char **av)
// {
// 	int i;

// 	if (ac != 5 && ac != 6)
// 	{
// 		printf("il faut 5 args");
// 		return (1);
// 	}
// 	t_data params;
// 	parse_args(ac, av, &params);
// 	printf("%d\n", params.nb_philo);
// 	printf("%ld\n", params.time_to_die);
// 	printf("%ld\n", params.time_to_eat);
// 	printf("%ld\n", params.time_to_sleep);
// 	printf("%d\n", params.must_eat);
// 	return (0);
// }
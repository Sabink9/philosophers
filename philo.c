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
void	parse_args(int ac, char **av, t_params *params)
{
	params->number_of_philosophers = ft_atoi(av[1]);
	params->time_to_die = ft_atoi(av[2]);
	params->time_to_eat = ft_atoi(av[3]);
	params->time_to_sleep = ft_atoi(av[4]);
	if (ac == 6)
		params->number_of_times_each_philosopher_must_eat = ft_atoi(av[5]);
	else
		params->number_of_times_each_philosopher_must_eat = -1;
}

int	main(int ac, char **av)
{
	int i;

	if (ac != 5 && ac != 6)
	{
		printf("il faut 5 agrs");
		return (1);
	}
	t_params params;
	parse_args(ac, av, &params);
	printf("%d\n", params.number_of_philosophers);
	printf("%ld\n", params.time_to_die);
	printf("%ld\n", params.time_to_eat);
	printf("%ld\n", params.time_to_sleep);
	printf("%d\n", params.number_of_times_each_philosopher_must_eat);
	return (0);
}
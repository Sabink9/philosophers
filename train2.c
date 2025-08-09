#include <pthread.h>
#include <stdio.h>

typedef struct
{
	int	compteur;
	pthread_mutex_t	mutex;
}		DonneesPartagees;

void	*fonction_thread(void *arg)
{
	DonneesPartagees	*data;
	int					i;

	data = arg;
	i = 0;
	while (i < 100000)
	{
		pthread_mutex_lock(&data->mutex);
		data->compteur++;
		pthread_mutex_unlock(&data->mutex);
		i++;
	}
	return (NULL);
}

int	main(void)
{
	DonneesPartagees	data;

	pthread_t t1, t2;
	data.compteur = 0;
	pthread_mutex_init(&data.mutex, NULL);
	pthread_create(&t1, NULL, fonction_thread, &data);
	pthread_create(&t2, NULL, fonction_thread, &data);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_mutex_destroy(&data.mutex);
	printf("Compteur final = %d\n", data.compteur);
	return (0);
}

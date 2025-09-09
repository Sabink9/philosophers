#include "puff.h"

typedef struct
{
	int	compteur;
	int	stop;
	pthread_mutex_t mutex;
}	t_donne;

void	*boucle(void *arg)
{
	t_donne	*data;
	int	valeur;
	data = arg;
	valeur = 0;
	while(1)
	{
		pthread_mutex_lock(&data->mutex);
		if (data->stop)
		{
			pthread_mutex_unlock(&data->mutex);
			break;
		}
		data->compteur++;
		pthread_mutex_unlock(&data->mutex);
		valeur++;
		usleep(2);
	}
	return (NULL);
}

void	*monitor(void *arg)
{
	t_donne	*data;

	data = arg;
	while(1)
	{
		pthread_mutex_lock(&data->mutex);
		if (data->compteur >= 1000)
		{
			data->stop = 1;
			pthread_mutex_unlock(&data->mutex);
			break;
		}
		pthread_mutex_unlock(&data->mutex);
		usleep(100);
	}
	return (NULL);
}

int	main(void)
{
	t_donne data;

	pthread_t t1, t2, mon;
	data.compteur = 0;
	data.stop = 0;
	pthread_mutex_init(&data.mutex, NULL);
	pthread_create(&t1, NULL, boucle, &data);
	pthread_create(&t2, NULL, boucle, &data);
	pthread_create(&mon, NULL, monitor, &data);
	pthread_join(t1, NULL);
	pthread_join(t2, NULL);
	pthread_join(mon, NULL);
	pthread_mutex_destroy(&data.mutex);
	printf("Compteur final = %d\n", data.compteur);
	return (0);
}
#include <pthread.h>
#include <stdio.h>

void	*fonction_thread(void *arg)
{
	int	valeur;
	valeur = *(int *)arg;
	printf("%d\n", valeur);
	return (NULL);
}

int	main(void)
{
	pthread_t	thread1, thread2;
	int			valeur;

	valeur = 42;
	pthread_create(&thread1, NULL, fonction_thread, &valeur);
	pthread_create(&thread2, NULL, fonction_thread, (&valeur) + 1);
	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return (0);
}

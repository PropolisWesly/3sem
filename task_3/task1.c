#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/shm.h>
#include <stdlib.h>

int main ()
{
	key_t key;
	int shmid;
	int *array;
	int I, S=1000;

	if ((key=ftok("task.c",0))<0)
	{
		printf("Can't generate key\n");
		exit(-1);
	}

	if ((shmid = shmget(key, 3*sizeof(int), 0))<0)
	{
		printf("Can't find shared memory\n");
		exit(-1);
	}

	if ((array = shmat(shmid, NULL, 0))==(int *)(-1))
	{
		printf("Can't attach shared memory\n");
		exit(-1);
	}
	
	I=(S*array[0])/array[1];
	printf("This is an integral for f(x) = x*x in a range [0, 10]. A real value: 333. The obtained value: %i\n", I);

	if((shmdt(array))<0)
	{
		printf("Can't detach shared memory\n");
		exit(-1);
	}
	
	return 0;
}

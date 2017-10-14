/* Рассчитать определённый интеграл какой-нибудь простой, интегрируемой аналитически на некотором промежутке функции f(x) методом Монте-Карло:
расчёт проводить параллельно в n-потоков некоторой программы А, генерируя в каждом из них N/n точек равномерно как по интервалу, так и по области значений.
Собирать и обрабатывать результаты в программе Б, читающей результаты из разделяемой памяти. [В случае использования независимых ячеек разделяемой памяти, 
сохранение результатов можно обеспечить без синхронизации, в случае разделяемой ячейки в разделяемой памяти - синхронизация необходима. Реализация способа хранения
влияет на результаты эксперимента 2 а,б*) (см. ниже).

Задачи, поставленные перед Вами как исследователем:
1) Оценить прирост производительности в связи с использованием n' потоков вместо 1, где n' - число физических ядер Вашей машины. 
Узнать число ядер-например, 'cat /proc/cpuinfo'
2) Провести серию измерений, в которой, увеличивая число n' :
  а) пронаблюдать стагнацию роста производительности при увеличении числа потоков
  б*) определить, при каких n' (насколько больших) накладные расходы от использования многопоточности [и синхронизации, если она имела место]
  превосходят преимущества их использования.
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/shm.h>
#include <pthread.h>
#include <sys/ipc.h>

pthread_mutex_t m;

void *pointer (void *dummy)
{
	key_t key;
	int shmid;
	unsigned int seed;
	int x,y, n=0, N=100, i;
	int *array;
	pthread_t mythid;

	if((key=ftok("task.c", 0))<0)
	{
		printf("Can't generate key\n");
		exit (-1);
	}
	
	if ((shmid = shmget(key, 3*sizeof(int), 0))<0)
	{
		printf("Can't find shared memory\n");
		exit(-1);
	}
	
	if ((array=(int *)shmat(shmid ,NULL, 0))<0)
	{
		printf("Can't attach shared memory\n");
		exit(-1);
	}

	if((pthread_mutex_lock(&m))!=0)
	{
		printf("Can't lock crirical zone\n");
		exit(-1);
	}
	seed=array[2];
	array[2] += 1;
	if((pthread_mutex_unlock(&m))!=0)
	{
		printf("Can't unlock critical zone\n");
		exit(-1);
	}

	for (i=N; i !=0 ; i--)
	{
		x=rand_r(&seed)%(11);
		y=rand_r(&seed)%(101);
		if ((x*x)>y)
			n++;
	}
	
	if((pthread_mutex_lock(&m))!=0)
	{
		printf("Can't lock critical zone\n");
		exit(-1);
	}
	
	array[0] += n;
	array[1] += N;

	if((pthread_mutex_unlock(&m))!=0)
	{
		printf("Can't unlock critical zone\n");
		exit(-1);
	}

	if (shmdt(array)<0)
	{
		printf("Can't detach shared memory\n");
		exit(-1);
	}
	
	mythid = pthread_self();
	printf ("%u has done his work. n = %d. N = %d\n", mythid, n, N);

	return NULL;
}

int main ()
{
	key_t key;
	int *array;
	int shmid;
	int n, i;
	pthread_t *thread;
	pthread_mutex_init(&m, NULL);
	int result;

	if((key = ftok("task.c", 0))<0)
	{
		printf("Can't generate key\n");
		exit(-1);
	}
	
	if ((shmid = shmget(key,3*sizeof(int), 0666 | IPC_CREAT))<0)
	{
		printf("Can't create shared memory\n");
		exit(-1);
	}
	
	if ((array = (int*)shmat(shmid, NULL, 0))==(int *)(-1))
	{
		printf("Can't attach shared memory\n");
		exit(-1);
	}

	array[0]=0;
	array[1]=0;
	array[2]=0;

	printf("Type number of threads: ");
	scanf("%i", &n);

	thread=(pthread_t *)malloc(n*sizeof(pthread_t));
	
	for (i=n; i !=0; i--)
	{
		result = pthread_create(&thread[i-1], (pthread_attr_t *)NULL, pointer, NULL );
		if (result != 0)
		{
			printf("Error on thread create, return value = %d\n", result);
			exit(-1);
		}
		printf("%u is created\n", thread[i-1]);
	}

	for(i=n; i != 0; i--)
	{
		pthread_join(thread[i-1], (void **)NULL);
		printf("%u has returned\n",thread[i-1]);
	}

	printf("Summary: n = %d, N = %d\n", array[0], array[1]);

	if (shmdt(array)<0)
	{
		printf("Can't detach memory\n");
		exit(-1);
	}
	
	free(thread);

	return 0;
}

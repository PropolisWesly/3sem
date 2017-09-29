#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <string.h>

typedef struct {
int txd[2];
int rxd[2]; 
} dpipe_t;

int main ()
{
	dpipe_t dpipe;
	pid_t pid;
	size_t size;
	char string[4096];
	int i;

	if (pipe(dpipe.txd)<0)
	{
		printf("Can\'t creat pipe1\n");
		exit(-1);
	}
	if (pipe(dpipe.rxd)<0)
	{
		printf("Can\'t creat pipe2\n");
		exit(-1);
	}

	pid = fork();
	if (pid<0)
	{
		printf("Can\'t fork child\n");
		exit(-1);
	}


	if(pid)	//txd 1-write rxd 0-read
	{
		if(close(dpipe.txd[0])<0)
		{
			printf("Can\'t close output for txd\n");
			write(dpipe.txd[1], "error", 6);
			exit(-1);
		}
		if(close(dpipe.rxd[1])<0)
		{
			printf("Can\'t close input for rxd\n");
			write(dpipe.txd[1], "error", 6);
			exit(-1);
		}

		while((strstr(string, "exit")!=string)&&(i==0))
		{
			size=read(0, string, sizeof(string)-1);
			string[size]='\0';
			printf("Parent send: %s\n", string);
			write(dpipe.txd[1], string, size);

			if (strstr(string, "exit")==string)
				i=1;

			size=read(dpipe.rxd[0], string, sizeof(string)-1);
			string[size]='\0';
			printf("Parent recieved: %s\n", string);
			
			if (strstr(string, "error")==string)
				exit(-1);
			
		}
		if (close(dpipe.txd[1])<0)
		{
			printf("Can\'t close input for txd\n");
			exit(-1);
		}
		if (close(dpipe.rxd[0])<0)
		{
			printf("Can\'t close output for rxd\n");
			exit(-1);
		}
		
	}
	else // txd 0-read rxd 1-write
	{
		if(close(dpipe.txd[1])<0)
		{
			printf("Can\'t close input for txd\n");
			write(dpipe.rxd[1], "error", 6);
			exit(-1);
		}
		if(close(dpipe.rxd[0])<0)
		{
			printf("Can\'t close output for rxd\n");
			write(dpipe.rxd[1], "error", 6);
			exit(-1);
		}
		while((strstr(string, "exit")!=string)&&(i==0))
		{
			size=read(dpipe.txd[0], string, sizeof(string)-1);
			string[size]='\0';
			printf("Child recieved: %s\n ", string);
			
			if (strstr(string, "exit")==string)
				i=1;

			if (strstr(string,"error")==string) 
				exit(-1);
			
			size=read(0, string, sizeof(string)-1);
			string[size]='\0';
			printf("Child send: %s\n", string);
			write(dpipe.rxd[1], string, size);
		}
		if (close(dpipe.txd[0])<0)
		{
			printf("Can\'t close output for txd\n");
			exit(-1);
		}
		if (close(dpipe.rxd[1])<0)
		{
			printf("Can\'t close input for rxd\n");
			exit(-1);
		}
	}

	return 0;
}

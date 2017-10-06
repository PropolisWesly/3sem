/*
  Write a program which allow you to: 
- run another programs via command line.
- get exit codes of terminated programs

## TIPS:
1. Use "2_fork_wait_exit.c" and "4_exec_dir.c" from examples. Combine them.
2. Parse input string according to the type of exec* (see "man exec").
   a) if execvp is used, string splitting into "path" and "args" is all you need.
3. Collect exit codes via waitpid/WEXITSTATUS.
*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>

int main (int argc, char *argv[], char *envp[])
{

	pid_t pid=fork();
	int status;
	char str[100];
	
	if (pid<0)
	{
		printf("Can't create child\n");
		exit(-1);
	}

	while  (pid>0)
	{
		waitpid(pid, &status, 0);
		printf("Ret code: %d\n", WEXITSTATUS(status));
		if (WEXITSTATUS(status)==0)
			exit(0);
		else pid = fork();

		if (pid<0)
		{
			printf("Can't fork child\n");
			exit(-1);
		}

		
	}
	scanf("%s", str);
	if ((str[0]=='q')&&(str[1]=='\0'))
		exit(0);
	execvp(str, str);
	printf("Can't open program\n");
	exit(0);
	
	
	return 0;
}

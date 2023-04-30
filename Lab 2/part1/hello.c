#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

char greeting[] = "Hello World";

int main()
{
	for (int i = 0; i < 11; i++)
	{
		printf("%c %d\n", greeting[i], getpid());
		sleep(rand() % 4 + 1);

		if (fork() != 0)
		{
			// If parent
			wait(NULL);
			break;
		}
	}
	exit(0);
}
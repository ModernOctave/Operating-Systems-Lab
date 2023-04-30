#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int ans;

int main(int argc, char *argv[])
{
	ans = atoi(argv[argc-1]) / 2;
	printf("Half: Current process id: %d, Current result: %d\n", getpid(), ans);
	sprintf(argv[argc-1], "%d", ans);

	if (argc > 2)
	{
		execvp(argv[1], argv+1);
	}
	
	exit(0);
}

#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
	int i;
	int prev_fd;
	pid_t pid;
	int fd[2];

	i = 0;
	prev_fd = 0;
	while(cmds[i])
	{
		if(cmds[i+1])
			pipe(fd);
		pid = fork();
		if(pid == 0)
		{
			if(prev_fd !=0)
			{
				dup2(prev_fd, STDIN_FILENO);
				close(prev_fd);
			}
			if(cmds[i+1])
			{
				close(fd[0]);
				dup2(fd[1], STDOUT_FILENO);
				close(fd[1]);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		else
		{
			if(prev_fd !=0)
				close(prev_fd);
			if(cmds[i+1])
			{
				close(fd[1]);
				prev_fd = fd[0];
			}
		}
		i++;
	}
	while(wait(NULL) > 0)
		;
	return (0);
}
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

int ft_popen(const char *file, const char *argv[], char type)
{
	pid_t pid;
	int fd[2];

	if(!file || !argv ||(type != 'w' && type !='r'))
		return (-1);
	pid = fork();
	if(pid == 0)
	{
		if(type == 'r')
		{
			close(fd[0]);
			dup2(fd[1], STDOUT_FILENO);
			close(fd[1]);
		}
		else if(type == 'w')
                {
			close(fd[1]);
			dup2(fd[0], STDIN_FILENO);
			close(fd[0]);
		}
		execvp(file,(char *const*) argv);
		exit(1);
	}
	else
	{
		if(type == 'r')
		{
			close(fd[1]);
			return (fd[0]);
		}
		if(type == 'w')
		{
			close(fd[0]);
			return(fd[1]);
		}
	}
	return (-1);
}

#include <string.h>

int main()
{
    int fd = ft_popen("cat", (const char *[]){"cat", NULL}, 'w');
    if (fd == -1) {
        perror("ft_popen");
        return 1;
    }

    // Write to the child's stdin
    const char *message = "Hello, from parent to child via pipe!\n";
    write(fd, message, strlen(message));

	 // Close the write-end to signal EOF to the child process
	close(fd); // Close the write-end to signal EOF
    return 0;
}
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int	ft_popen(const char *file, char *const argv[], char type)
{
	int pipe_fds[2];
	pid_t pid;

	if (type != 'r' && type != 'w')
		return -1;

	if (pipe(pipe_fds) == -1)
		return -1;

	pid = fork();
	if (pid == -1)
	{
		close(pipe_fds[0]);
		close(pipe_fds[1]);
		return -1;
	}

	if (pid == 0)
	{
		if (type == 'r')
		{
			close(pipe_fds[0]);  // Close the unused (read) end
			if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
				exit(EXIT_FAILURE);
			close(pipe_fds[1]); // Safe to close the write end after dup2
		}
		else if (type == 'w')
		{
			close(pipe_fds[1]);  // Close the unused (write) end
			if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
				exit(EXIT_FAILURE);
			close(pipe_fds[0]); // Safe to close the read end after dup2
		}
		execvp(file, argv);
		perror("execvp"); // execvp only returns if there's an error
		exit(EXIT_FAILURE);
	}

	// Parent process
	if (type == 'r')
	{
		close(pipe_fds[1]);	// Close the write end
		return pipe_fds[0]; // Return the reading end
	}
	else if (type == 'w')
	{
		close(pipe_fds[0]); // Close the read end
		return pipe_fds[1]; // Return the writing end
	}
	return -1; // Fallback (will not reach here)
}
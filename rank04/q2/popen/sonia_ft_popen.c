#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int ft_popen(const char *file, char *const argv[], char type)
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
            close(pipe_fds[0]);
            if (dup2(pipe_fds[1], STDOUT_FILENO) == -1)
                exit(EXIT_FAILURE);
            close(pipe_fds[1]);
        }
        else
        {
            close(pipe_fds[1]);
            if (dup2(pipe_fds[0], STDIN_FILENO) == -1)
                exit(EXIT_FAILURE);
            close(pipe_fds[0]);
        }
        execvp(file, argv);
        exit(EXIT_FAILURE);
    }

    return (type == 'r') ? (close(pipe_fds[1]), pipe_fds[0]) 
                        : (close(pipe_fds[0]), pipe_fds[1]);
}
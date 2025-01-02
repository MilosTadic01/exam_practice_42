#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

int picoshell(char **cmds[])
{
    int prev_pipe[2] = {-1, -1};
    int pid;
    int cmd_count = 0;
    // Count commands
    while (cmds[cmd_count])
        cmd_count++;
    for (int i = 0; i < cmd_count; i++)
    {
        int curr_pipe[2] = {-1, -1};
        
        // Create pipe and check error
        if (i < cmd_count - 1)
        {
            if (pipe(curr_pipe) == -1)
                return 1;
        }
        pid = fork();
        if (pid == -1)
        {
            // Clean up pipes on fork error
            if (prev_pipe[0] != -1)
            {
                close(prev_pipe[0]);
                close(prev_pipe[1]);
            }
            return 1;
        }
        if (pid == 0)
        {
            // Child process
            if (i > 0)
            {
                if (dup2(prev_pipe[0], STDIN_FILENO) == -1)
                    exit(1);
                if (close(prev_pipe[0]) == -1 || close(prev_pipe[1]) == -1)
                    exit(1);
            }
            if (i < cmd_count - 1)
            {
                if (dup2(curr_pipe[1], STDOUT_FILENO) == -1)
                    exit(1);
                if (close(curr_pipe[0]) == -1 || close(curr_pipe[1]) == -1)
                    exit(1);
            }
            execvp(cmds[i][0], cmds[i]);
            exit(1);  // execvp failed
        }
        // Parent process - clean up pipes
        if (prev_pipe[0] != -1)
        {
            if (close(prev_pipe[0]) == -1 || close(prev_pipe[1]) == -1)
                return 1;
        }
        prev_pipe[0] = curr_pipe[0];
        prev_pipe[1] = curr_pipe[1];
    }
    // Wait for all children
    int status;
    while (wait(&status) > 0)
    {
        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
            return 1;
    }
    return 0;
}
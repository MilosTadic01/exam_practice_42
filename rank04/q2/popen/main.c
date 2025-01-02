#include <unistd.h>
#include <stdio.h>

int	ft_popen(const char *file, char *const argv[], char type);

int	main(void)
{
	char	buffer[256];
	ssize_t	bytes_read;

	// Test 1: Reading output from "ls"
	int fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
	if (fd == -1)
		return 1;
	printf("Output of `ls`:\n");
	while ((bytes_read = read(fd, buffer, 256)) > 0)
		write(STDOUT_FILENO, buffer, bytes_read);
	close(fd);

	// Test 2: Writing input to "cat"
	int fd2 = ft_popen("cat", (char *const[]){"cat", NULL}, 'w');
	if (fd2 == -1)
		return 1;
	printf("`cat`:\n");
	while ((bytes_read = read(STDOUT_FILENO, buffer, 256)) > 0)  // until EOF
		write(fd2, buffer, bytes_read);
	close(fd2);

	// Test 3: pipe the output of "ls" to "wc -l" (not very safe, esp valgrind)
	fd = ft_popen("ls", (char *const[]){"ls", NULL}, 'r');
	if (fd == -1)
		return 1;
	fd2 = ft_popen("wc", (char *const[]){"wc", "-l", NULL}, 'w');
	if (fd2 == -1)
		return 1;
	while ((bytes_read = read(fd, buffer, 256)) > 0)
		write(fd2, buffer, bytes_read);
	close(fd);
	close(fd2);

	return 0;
}

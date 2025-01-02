#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

pid_t child_pid;
bool kill_failed = false;
bool killed_child = false;

void kill_proc(int signal) {
	(void)signal;
	if (kill(child_pid, SIGKILL) == -1)
		kill_failed = true;
	killed_child = true;
}

int sandbox(void (*f)(void), unsigned int timeout, bool verbose) {
	if ((child_pid = fork()) == 0) {
		f();
		exit(0);
	} else if (child_pid == -1)
		return -1;
	if (sigaction(SIGALRM, &(struct sigaction){.sa_handler = kill_proc, .sa_flags = SA_RESTART}, NULL))
		return -1;
	alarm(timeout);
	int wstatus;
	if (waitpid(child_pid, &wstatus, 0) == -1)
		return -1;
	if (kill_failed)
		return -1;
	if (WIFEXITED(wstatus)) {
		int exit_status = WEXITSTATUS(wstatus);
		if (exit_status == 0) {
			if (verbose)
				(void)printf("Good function!\n");
			return 1;
		} else {
			if (verbose)
				(void)printf("Bad function: exited with exit status %d\n", exit_status);
			return 0;
		}
	} else if (WIFSIGNALED(wstatus)) {
		int signal_code = WTERMSIG(wstatus);
		if (signal_code == SIGKILL && killed_child) {
			if (verbose)
				(void)printf("Bad function: timed out after %d seconds\n", timeout);
		} else {
			if (verbose)
				(void)printf("Bad function: %s\n", strsignal(signal_code));
		}
		return 0;
	} else {
		return -1;
	}
}

void f1(void) {
	(void)printf("I'm a nice function\n");
}

void f2(void) {
	(void)printf("I'm a bad function\n");
	exit(1);
}

void f3(void) {
	(void)printf("I'm a terrible function\n");
	*(char *)0 = 0;
}

void f4(void) {
	(void)printf("I'm a terrible function AGAIN\n");
	abort();
}

void f5(void) {
	(void)printf("I will run forevaaaa\n");
	while (true) {
	}
}

void f6(void) {
	(void)printf("I will block all signalsss (and run forever)!\n");
	(void)sigaction(SIGALRM, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGKILL, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGSTOP, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGTERM, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGSEGV, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGPIPE, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGBUS, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGCHLD, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGHUP, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGTSTP, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGTTIN, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGTTOU, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGURG, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	(void)sigaction(SIGQUIT, &(struct sigaction){.sa_handler = SIG_IGN, .sa_flags = SA_RESTART}, NULL);
	while (true) {
	}
}

int main(void) {
	void (*funcs[])(void) = {f1, f2, f3, f4, f5, f6};
	for (unsigned int i = 0; i < sizeof(funcs)/sizeof(funcs[0]); ++i) {
		(void)printf("sandbox ret: %d\n", sandbox(funcs[i], 2, true));
	}
	return 0;
}
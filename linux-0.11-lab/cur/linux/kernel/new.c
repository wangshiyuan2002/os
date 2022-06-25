#include <unistd.h>
#include <linux/kernel.h>
#include <signal.h>
int sys_execve2(const char* path, char* argv[], char* envp[]) {

	int res;
	res = execve(path, argv,envp);

	return res;
}




void handler(int sign) {};
int sys_sleep(unsigned int s) {
	struct sigaction a, oa;
	a.sa_handler = handler;
	a.sa_flags = 0;
	sigemptyset(&a.sa_mask);
	sigaction(SIGALRM, &a, &oa);
	alarm(s);
	pause();
	int ret = alarm(0);
	sigaction(SIGALRM, &oa, NULL);
	return ret;
}
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<signal.h>


#define SIG_FROM_KERNEL 44 // custom signal number sent from kernel module to user space

static int done=0;
int check=0;
/*
 Program's signal handling mechanism
 */

//handles SIGINT signal
void ctrl_c_handler(int n,siginfo_t *info,void *unused){
	if(n==SIGINT){
		printf("\n recieved ctrl-c\n");
		done=1;//indicating program should terminate
	}
}

//handles custom signal(SIG_FROM_KERNEL), sent from kerenl module
void sig_event_handler(int n,siginfo_t *info,void *unused){
	if(n==SIG_FROM_KERNEL){
		check=info->si_int;//extracts signal value
		printf("Recieved signal from kernel: Value =%u\n",check);
	}
}

int main(){
	int fd;
	int32_t value,number;
	struct sigaction act;
	char buf[100]={0};

	//install ctrl-c interrupt handler to cleanup at exit
	//initialize signal set(sa_mask) to an empty set
	//i.e no signals are blocked during execution of signal handler
	sigemptyset(&act.sa_mask);
	act.sa_flags=(SA_SIGINFO | SA_RESETHAND);//set flags for signal handler
	act.sa_sigaction=ctrl_c_handler;
	sigaction(SIGINT,&act,NULL);//install ctrl_c_handler for SIGINT signal

	//install custom signal handler
	sigemptyset(&act.sa_mask);
	act.sa_flags=(SA_SIGINFO | SA_RESTART);
	act.sa_sigaction=sig_event_handler;
	sigaction(SIG_FROM_KERNEL,&act,NULL);


	printf("Installed signal handler for SIGETX=%d\n",SIG_FROM_KERNEL);

	sprintf(buf,"echo %d,%d > /proc/sig_target",getpid(),SIG_FROM_KERNEL);

	system(buf);//writes information to the kernel module through /proc/sig_target file
	printf("Done \n");

	/*
	 enter a loop and wait for signals
	 (either Ctrl+C or custom signal from kernel module) 
	 */
	while(!done){
		printf("Waiting for signal\n");
		//blocking check
		while(!done && !check);
		check=0;
	}
}

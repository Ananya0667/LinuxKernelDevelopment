#define _GNU_SOURCE
#include<stdio.h>
#include<linux/kernel.h>
#include<sys/syscall.h>
#include<unistd.h>
#include<sys/types.h>
#include<signal.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sched.h>

struct pid_ctxt_switch{
	unsigned long ninvctxt;
	unsigned long nvctxt;
};
int main()
{
	int i,loop=0;
	long int amma;
	struct pid_ctxt_switch ctxt={0,0};
	while(loop<5)
	{
		pid_t pid=getpid();
		printf("pid=%d\n",pid);
		i=548;
		sleep(5);
		sched_yield();
		amma=syscall(i,pid);
		printf("system call %d returned %ld \n",i,amma);
		if(amma==-1)
			printf("error no is %d \n",errno);
		i=549;
		sleep(2);
		sched_yield();
		amma=syscall(i,&ctxt);
		printf("system call %d returned %ld \n",i,amma);
		if(amma==-1)
			printf("error no %d \n",errno);
		else{
			printf("nvctxt=%ld \n",ctxt.nvctxt);
			printf("ninvctxt=%ld \n",ctxt.ninvctxt);
		}
		sleep(2);
		sched_yield();
		i=549;
		amma=syscall(i,&ctxt);
		printf("system call %d returned %ld \n",i,amma);
		if(amma==-1)
			printf("error no is %d\n",errno);
		else{
			printf("nvctxt=%ld \n",ctxt.nvctxt);
			printf("ninvctxt=%ld \n",ctxt.ninvctxt);
		}
		sleep(2);
		sched_yield();
		i=549;
		amma=syscall(i,&ctxt);
		printf("system call %d returned %ld \n",i,amma);
		if(amma==-1)
			printf("error no %d\n",errno);
		else{
			printf("nvctxt=%ld\n",ctxt.nvctxt);
			printf("ninvctxt=%ld\n",ctxt.ninvctxt);
		}
		sleep(2);
		sched_yield();
		i=548;
		amma=syscall(i,pid);
		printf("system call %d returned %ld\n",i,amma);
		if(amma==-1)
			printf("error no is %d\n",errno);
		sleep(5);
		sched_yield();
		i=549;
		amma=syscall(i,&ctxt);
		printf("system call %d returned %ld\n",i,amma);
		if(amma==-1)
			printf("errno %d\n",errno);
		else{
			printf("nvctxt=%ld\n",ctxt.nvctxt);
			printf("ninvctxt=%ld\n",ctxt.ninvctxt);
		}
		sleep(5);
		sched_yield();
		i=550;
		sched_yield();
		amma=syscall(i,pid);
		printf("system call %d returned %ld \n",i,amma);
		if(amma==-1)
			printf("error code is %d\n",errno);
		sleep(5);
		sched_yield();
		i=549;
		amma=syscall(i,&ctxt);
		printf("system call %d returned %ld\n",i,amma);
		if(amma==-1)
			printf("error no is %d\n",errno);
		else{
			printf("nvctxt=%ld\n",ctxt.nvctxt);
			printf("invctxt=%ld\n",ctxt.ninvctxt);
		}
		i=550;
		sched_yield();
		amma=syscall(i,pid);
		printf("system call %d returned %ld\n",i,amma);
		loop++;
	}
	return 0;
}

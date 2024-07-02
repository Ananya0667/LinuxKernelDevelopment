#include <stdio.h>
#include <linux/kernel.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sched.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdbool.h>
#include <string.h>
#include<time.h>

double time_range(struct timespec time1, struct timespec time2)
{
	double time_taken;
	time_taken = time2.tv_sec - time1.tv_sec;
	time_taken += time2.tv_nsec - time1.tv_nsec;
	return time_taken;
}

void perform_job(int n){
	int ans=1;
	for(int i=1;i<=n;i++){
		ans=ans*i;
	}
}

int main(int argc, char *argv[]){
    int period,deadline,exec_time,pid;
    int i,n=5,count1=0,count2=5,counter=0;
    long int amma;
    struct timespec start,finish,initial;
    pid=atoi(argv[1]);
    period=atoi(argv[2]);
    deadline=atoi(argv[3]);
    exec_time=atoi(argv[4]);
    //int pid=getpid();
    i = 551; // sys_register_rm
    amma=syscall(i,pid,period,deadline,exec_time);
    printf("system call %d returned %ld \n", i, amma);
    if (amma != 0) {
        printf("Registration failed\n\n");
        return 0;
    }
        clock_gettime(CLOCK_MONOTONIC_RAW, &initial);
    do{
	    printf("For PID %d\n",pid);
        clock_gettime(CLOCK_MONOTONIC_RAW, &start);
        printf(" for PID %d\tWakeup : %.3f\n",pid, time_range(initial, start));
        perform_job(n);
	    clock_gettime(CLOCK_MONOTONIC_RAW, &finish);
        printf("for PID %d\tTime to finish : %.3f\n", pid,time_range(start, finish));
	    count1=count1+1;
        i = 553; // sys_yield
        amma = syscall(i, pid);
        printf("system call %d returned %ld \n", i, amma);
        if (amma == -1)
            printf("error no is %d \n", errno);	
    }while(count1<count2);
  
    i = 555; // list
    syscall(i);
    printf("system call %d returned \n", i);
    i = 554; // sys_remove
    amma = syscall(i,pid);
    printf("system call %d returned %ld \n", i, amma);
    if (amma == -1)
     printf("error no is %d \n", errno);
 
     i = 555; // list
    syscall(i);
    printf("system call %d returned\n", i);
    return 0;
}

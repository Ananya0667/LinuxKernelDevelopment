#include<linux/module.h>
#include<linux/proc_fs.h>
#include<linux/seq_file.h>
#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/types.h>
#include<linux/sched.h>
#include<linux/pid.h>
#include<linux/resource.h>
#include<linux/timer.h>
#include<linux/jiffies.h>
#include<linux/string.h>
#include<linux/workqueue.h>
#include<linux/delay.h>
#include<linux/sched/signal.h>

#define WORK_QUEUE_NAME "sig_target_wq" //name of the workqueue
#define TIMEOUT 6000 //timeout interval for timer to run every minute
		     
static struct timer_list sig_timer;//represents timer used for periodic execution
static struct kernel_siginfo info;//usede for sending signal info
static struct task_struct *t;
static struct work_struct workq;//work item for workqueue

struct pid_sig
{
	int signo;//signal number
	pid_t pid;
};

int flag_sig=0;//indicates whether a signal should be sent

struct pid_sig input;//to store input pid and signal info

//handles work executed by workqueue
//sends signal to processes based on input info
/*
 workqueue is used to handle asynchronous tasks, such as sending signals to target processes
 */
static void work_handler(struct work_struct *_work){
	static int count=0;//no of times work_handler() is called
	printk("work_handler function called\n");

	if(flag_sig==1){// there is a signal to be sent
		printk("going to send signal ->%d, to pid ->%d\n",input.signo,input.pid);
		//memset(&info,0,sizeof(struct siginfo));

		/*
		 struct siginfo structure(info)- preparing with info about the signal to be sent
		 */
		info.si_signo=input.signo;//user input signal number
		info.si_code=SI_QUEUE;//signal is being sent using real time signaling mechanism
		info.si_int=++count;

		//rcu_read_lock();
		t=find_task_by_vpid(input.pid);
		//t=get_pid_task(find_get_pid(input.pid),PIDTYPE_PID);
		if(t){
			//rcu_read_unlock();
			if(send_sig_info(input.signo,&info,t)<0)//send signal
				printk("send sig_info error\n");
			else
				printk("send sig_info success! signal send\n");
		}
		else{
			printk("pid_task_error\n");
		}
	}

}

//callback for timer
//schedules work item in the workqueue
static void timer_callback(struct timer_list *data){
	printk("Timer callback function called\n");
	schedule_work(&workq);//schedules the work item(workq)[work handler()] exe in the workqueue
	mod_timer(&sig_timer,jiffies+msecs_to_jiffies(TIMEOUT));//modifies the timer (sig_timer) 
								//updates expiration time
								//ensuring timer_callback()called
								//after specified timeout
}


/*
 file operations and proc operations for /proc/sig_target file
 */
static int open_proc(struct inode *inode,struct file *file){
	//when a process opens the /proc/sig_target file
	printk("proc file is opened\n");
	return 0;
}

static int release_proc(struct inode *inode,struct file *file){
	//when a process releases/closes the /proc/sig_target file
	printk("proc file is released\n");
	return 0;
}

static ssize_t read_proc(struct file *filp,char __user *buffer,size_t length,loff_t *offset){
	printk("proc file is read\n");
	/*
	 if(copy_to_user(buffer,"msg from /proc/sig_target",25)){
	 	printk("Data send: ERR!\n");
	 }
	 else{
	 	printk("data sent to user space \n");
	 }
	 */
	return length;
}

 
static ssize_t write_proc(struct file *filp,const char *buff,size_t len,loff_t *off){
	char buf[100]={0};
	printk(KERN_INFO "proc file is being written \t");
	flag_sig=0;

	if(len>sizeof(buf))//data to be written from user space doesn't exceed size of local buffer
		len=sizeof(buf);

	memset(buf,0,sizeof(buf));//clear contents of local buffer

	if(copy_from_user(buf,buff,len))//copy the content from user space to local buffer
		printk("Data write: ERR \n");
	else
		printk("Data write: Success \n");
	
	//print contents of local buffer
	printk("write function data input is : %s\n",buf);
	//parse contents of local buffer to extract info(pid,signal no)
	sscanf(buf,"%d,%d",&input.pid,&input.signo);
	//extracted signal no print
	printk("write_func data signo %d\n",input.signo);
	if(((input.pid>0)&& (input.pid<99999))&& ((input.signo>0) && (input.signo<999))){
				flag_sig=1;//valid signal is ready to be sent
	}
	return len;
}

//proc fops containers function pointers for file operations asoc with proc file
static struct proc_ops proc_fops={
	.proc_open=open_proc,
	.proc_read=read_proc,
	.proc_write=write_proc,
	.proc_release=release_proc
};


//module initialization

static int __init proc_init(void){
	if(proc_create("sig_target",0666,NULL,&proc_fops)==NULL)//creates /proc/sig_target file
		return -ENOMEM;
	timer_setup(&sig_timer,timer_callback,0);//sets up timer for periodic exe
	mod_timer(&sig_timer,jiffies+msecs_to_jiffies(TIMEOUT));
	printk("/proc/sig_target file is created \n");
	INIT_WORK(&workq,work_handler);//initializes workqueue
	return 0;
}

//module cleanup
static void __exit proc_exit(void){
	flush_scheduled_work();//flushes work queue
	del_timer(&sig_timer);//deletes timer,prevents timer_callback() to exe after module unloaded
	remove_proc_entry("sig_target",NULL);//removes /proc/sig_target entry from proc file system
}

MODULE_LICENSE("GPL");
module_init(proc_init);// module loaded in kernel
module_exit(proc_exit);//module unloaded in kernel


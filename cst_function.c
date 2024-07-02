#include<linux/kernel.h>
#include<linux/syscalls.h>
#include<linux/types.h>
#include<linux/sched.h>
#include<linux/pid.h>
#include<linux/resource.h>
/*struct task_struct *my_task;
struct rusage r;

SYSCALL_DEFINE1(register, pid_t,pid){
	printk("Inside kernel space sys_register system call \n");
	printk("Process id recieved =%d \n",pid);
	my_task=get_pid_task(find_get_pid(pid),PIDTYPE_PID);
	if(my_task){
		printk("process nvcsw =%ld,nivcsw =%ld \n",my_task->nvcsw,my_task->nivcsw);
		getrusage(my_task,RUSAGE_SELF,&r);
		printk("after getruage process nvcsw =%ld, nivcsw=%ld \n", r.ru_nvcsw,r.ru_nivcsw);

		return 0;
	}
	return -EINVAL;
}

SYSCALL_DEFINE1(fetch,struct pid_ctxt_switch __user *,ru)
{
	struct pid_ctxt_switch ctxt;
	printk("Inside kernel space sys_fetch system call \n");
	ctxt.ninvctxt=r.ru_nivcsw;
	ctxt.nvctxt=r.ru_nvcsw;
	printk("in fetch nvcsw = %ld,nivcsw =%ld \n",ctxt.nvctxt,ctxt.ninvctxt);
	return copy_to_user(ru,&ctxt,sizeof(struct pid_ctxt_switch)) ? -EFAULT :0;
}
SYSCALL_DEFINE1(deregister,pid_t,pid)
{
	printk("inside sys_deregister \n");
	printk("pid is %d\n",pid);
	return 0;
}

*/
LIST_HEAD(mylist);//declare and initialize an empty linked list
//struct rusage r;

//register a process with a given PID 
SYSCALL_DEFINE1(register,pid_t,pid){
	struct task_struct *tasks;//process descriptor
	//struct task_struct *my_task;
	struct pid_node *new_node;
	printk("Inside kernel space sys_register system call\n");
	printk("Process id recieved =%d\n",pid);
	tasks=find_task_by_vpid(pid);//pointer to task struct corres to given PID
	//my_task=get_pid_task(find_get_pid(pid),PIDTYPE_PID);
	//my_task=find_task_by_vpid(pid);
	new_node=kmalloc(sizeof(struct pid_node),GFP_KERNEL);// GFP flag indicates memory allocation within kernel
	if(pid<1)
		return -22;	
	if(!tasks)
		return -3;	//pid doesn't exist
	if(!new_node)
		return -ENOMEM;	//memory allocation failure
	new_node->pid=pid;
	// add process to linked list
	INIT_LIST_HEAD(&new_node->next_prev_list);
	list_add_tail(&new_node->next_prev_list,&mylist);
	//printk("\n\n");
	
	/*my_task=get_pid_task(find_get_pid(pid),PIDTYPE_PID);
	//retriving information about context switches
	if(my_task){
		printk("process nvcsw=%ld, nivcsw=%ld\n",my_task->nvcsw,my_task->nivcsw);
		getrusage(my_task,RUSAGE_SELF,&r);// to get info for current process
		printk("after getrusage process nvcsw =%ld, nivcsw=%ld\n",r.ru_nvcsw,r.ru_nivcsw);
	}*/
	return 0;// return success
}


SYSCALL_DEFINE1(fetch,struct pid_ctxt_switch __user *,stats){
	struct pid_node *entry;
	struct task_struct *mytasks;
	printk("Inside kernel space sys_register system call\n");
	
	// initialize counters
	stats->ninvctxt=0;
	stats->nvctxt=0;
	
	list_for_each_entry(entry,&mylist,next_prev_list){	//iterate over LL
		mytasks=find_task_by_vpid(entry->pid);
		if(!mytasks)//task with given PID not found
			return -22;	
		//printk("pid=%d\n",entry->pid);//if task with given PID is found
		//add context switch counts for each process in the LL
		stats->ninvctxt=stats->ninvctxt+mytasks->nivcsw;
		stats->nvctxt=stats->nvctxt+mytasks->nvcsw;	
	}
	return 0;//return success
}
//remove a process with a given PID
SYSCALL_DEFINE1(deregister,pid_t,pid){
	struct pid_node *entry;
	int flag=0;
	printk("Inside kernel space sys_deregister system call\n");
	if(pid<1)
		return -22;
	printk("Process id passed =%d\n",pid);
	list_for_each_entry(entry,&mylist,next_prev_list){
		if(entry->pid==pid){
			//remove the process from the LL
			flag=1;
			list_del(&entry->next_prev_list);
			kfree(entry);
			break;
		}
	}
	if(flag==1)//process removed successfully
		return 0;
	return -3;//PID doesn't exist in LL
}

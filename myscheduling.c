#include <linux/types.h>
#include <linux/errno.h>
#include <linux/cpumask.h>
#include <linux/sched.h>
#include <uapi/linux/sched/types.h>
#include <linux/pid.h>
#include <linux/syscalls.h>
#include <linux/list.h>
#include <linux/kthread.h>
#include<linux/timer.h>
#include <linux/sched/signal.h>

// Define the head of the linked list
LIST_HEAD(my_list);// list is maintained in increasing order of priority of tasks.

// Define constants for task states
#define READY 0
#define RUNNING 1
#define SLEEPING 2

// Define the structure for tasks-Process Control Block
struct my_task_struct {
    struct list_head next_prev_list;
	struct task_struct* my_task; // Pointer to the task
    struct timer_list my_timer;// Timer for the task
    unsigned int state;// Task state (READY, RUNNING, SLEEPING)
	unsigned int period;// Task period
	unsigned int deadline;// Task deadline
	unsigned int exec_time;// Execution time
	int priority;// Task priority
};

struct task_struct* dispatcher_thread = NULL; //dispatcher thread pointer
struct my_task_struct* my_curr_tasks;//current task pointer


//helperfunction to find my_task_struct with a given process ID 
struct my_task_struct* find_my_taskstruct(pid_t pid){
	struct my_task_struct *task;
	list_for_each_entry(task, &my_list, next_prev_list) {
		if (task->my_task->pid == pid)
			return task;
	}
	return NULL;
}

static void my_timer_function(struct timer_list *timer) {
    // Get the pointer to my_task_struct from the timer
	struct my_task_struct *task = from_timer(task, timer, my_timer);
    printk( "Timer is running \n ");
    if(!task)
        printk("Task is invalid\n");

    if(task->state == SLEEPING){
        task->state = READY;
        WRITE_ONCE(task->my_task->__state, TASK_INTERRUPTIBLE);
    }
	printk("Dispatcher is called \n");
	if (dispatcher_thread)
		wake_up_process(dispatcher_thread);
	else 
		printk("No running thread\n");
    printk( "Timer has finished\n");
 }

//helper_function01 to compare priorities of two tasks
int helperfunction_priority(struct my_task_struct* task, struct my_task_struct* tsk) {
	printk("checking which task has higher prioirity\n");
	if (task->priority > tsk->priority) 
		return 1;
	else	
		return 0;// tsk has higher priority or equal priority
}

static int dispatcher_thread_callback(void* data) {
	struct my_task_struct* p_task;
	struct my_task_struct* higher_priority_task = NULL;
	struct kernel_siginfo info;
	struct sched_param sp1;
	struct sched_param sparam;
	printk("Dispatcher thread callback is called\n");
	set_current_state(TASK_INTERRUPTIBLE);
    // Continuously loop until the thread should stop
	while (!kthread_should_stop()) {
		printk("Inside dispatcher thread loop\n");

    // Find the highest priority task that is in READY state
	list_for_each_entry(p_task, &my_list, next_prev_list) {
		if (p_task->state == READY && (higher_priority_task == NULL || helperfunction_priority(p_task, higher_priority_task))) {
			higher_priority_task = p_task;
		}
	}

	if (!higher_priority_task) {
		printk("No process is READY to to next scheduled\n");
		return -1;
	}

	
	if (my_curr_tasks && helperfunction_priority(my_curr_tasks, higher_priority_task)) {
		printk("my_curr_tasks is READY and has a higher priority i.e small deadline than scheduled task\n");
		return -1;
	}

	//Premption of lower priority running task
	if (my_curr_tasks) {
		printk("PID %u is Running\n", my_curr_tasks->my_task->pid);
		printk("Premption of PID %u\n",my_curr_tasks->my_task->pid);
        // Send SIGSTOP signal to suspend the current task
		info.si_signo = SIGSTOP;
		send_sig_info(SIGSTOP, &info, my_curr_tasks->my_task);

        // Set the scheduler to SCHED_NORMAL
		sp1.sched_priority = 0;
		sched_setscheduler(my_curr_tasks->my_task, SCHED_NORMAL, &sp1);
		my_curr_tasks->state = READY; 
	}

     // Switch to the higher priority task
	printk("Now, the higher priority task will run \n");
	printk("PID %u has higher priority\n", higher_priority_task->my_task->pid);
	printk("PID %u is now, running\n", higher_priority_task->my_task->pid);

    // Send SIGCONT signal to resume the higher priority task
	info.si_signo = SIGCONT;
	send_sig_info(SIGCONT, &info, higher_priority_task->my_task);

    // Set the scheduler to SCHED_FIFO for the higher priority task
	sparam.sched_priority = 99;
	sched_setscheduler(higher_priority_task->my_task, SCHED_FIFO, &sparam);

     // Update the state of the higher priority task to RUNNING
	higher_priority_task->state = RUNNING;
    // Update the current task to the higher priority task
	my_curr_tasks = higher_priority_task;

	printk("Dispatcher has completed its task\n");
    // Schedule the next task to run
	schedule();
	}

	set_current_state(TASK_RUNNING);
	printk("Dispatcher thread callback ended\n");
	return 0;
}

// Admission control function 
unsigned int compute_interference(int current_task_index, int current_time) {
    struct my_task_struct *task;
    int task_index = 0;
    unsigned int sum = 0;

    list_for_each_entry(task, &my_list, next_prev_list) {
        // Check if the current task index exceeds the given current task index
        if (++task_index >= current_task_index) 
            break;

        // Add to total
        sum += ((current_time + task->period - 1) / task->period) * task->exec_time;
    }
    // Return the total interference
    return sum;
}


// Schedulability test function
int schedulability_test(void) {
    struct my_task_struct *task;
    unsigned int running_time = 0;
    unsigned int totalsum = 0; 
    int task_index = 0; 
    printk("Schedulability test check\n");

    list_for_each_entry(task, &my_list, next_prev_list) {
        unsigned int task_completion_time = running_time + task->exec_time;
        int continue_checking = 1; // Flag to continue checking

        // Keep checking until the condition is met
        while (continue_checking) {
            // Compute interference for the task at its completion time
            totalsum = compute_interference(task_index, task_completion_time);
            
            if (totalsum + task->exec_time <= task_completion_time)
                continue_checking = 0; // Stop checking
            else 
                task_completion_time = totalsum + task->exec_time;
            
            // If task completion time exceeds its deadline, return 0 (schedulability test fails)
            if (task_completion_time > task->deadline)
                return 0;
        }

        // Increment task index and update running time
        task_index++;
        running_time += task->exec_time;
    }

    // Return 1 (schedulability test passes)
    return 1;
}



int __register_dm(pid_t pid, unsigned int period, unsigned int deadline, unsigned int exec_time) {
	struct my_task_struct* new_reg_task;
	struct task_struct* task;
	struct my_task_struct *d_task;
	struct cpumask cpu_mask;
	struct pid* my_pid_struct;
	printk("Inside sys_register system call for PID %u\n", pid);
    // If dispatcher thread is not running, start it
	if (!dispatcher_thread) {
		printk("Dispatcher has started \n");
		dispatcher_thread = kthread_run(&dispatcher_thread_callback, NULL, "kdmd");
		if (IS_ERR(dispatcher_thread)) {
			pr_info("create kdmd failed\n");
		}
		else
			pr_info("Dispatcher thread running with pid %u\n\n", dispatcher_thread->pid);
		}

	if (find_my_taskstruct(pid)) {
		printk("Please enter a different PID\n");
		printk("List contains a process with that PID");
		return -22;
	}
    
    // Allocate memory for the new task
    new_reg_task = (struct my_task_struct*)kmalloc(sizeof(*new_reg_task), GFP_KERNEL);
	new_reg_task->period   = period;
	new_reg_task->deadline = deadline;
	new_reg_task->exec_time = exec_time;
	new_reg_task->priority = -((int)deadline);

    // Find the task associated with the given PID
	my_pid_struct = find_get_pid(pid);
	task = pid_task(my_pid_struct, PIDTYPE_PID);
	if (!task) {
		kfree(new_reg_task);
		printk("No task is associated with given PID\n");
		return -22;
	}
	new_reg_task->my_task = task;


    // Initialize timer for the task
	timer_setup(&new_reg_task->my_timer, my_timer_function, 0);
	//mod_timer(&new_reg_task->my_timer,jiffies+msecs_to_jiffies(1000));
	
	printk("Task with PID %u inserted in list\n", new_reg_task->my_task->pid);
    // Find the correct position to insert the task based on priority
	list_for_each_entry(d_task, &my_list, next_prev_list) {
		if (new_reg_task->priority>=d_task->priority){
			//we have found the location,to insert our task
			break;
		}
	}
      // Insert the new task into the list
	list_add(&new_reg_task->next_prev_list, d_task->next_prev_list.prev);

    // Perform schedulability test to check if the process satisfies criteria
	if (!schedulability_test()) {
		printk("Process do not satisfy schedulability criteria\n");
		list_del(&new_reg_task->next_prev_list);
		kfree(new_reg_task);
		return -22;
	}
	printk("schedulability test passed\n");
	// cpumask used to run all the processes on say CPU 0
	cpumask_clear(&cpu_mask);
	cpumask_set_cpu(0, &cpu_mask);
	sched_setaffinity(pid, &cpu_mask);

	printk("PID %u registered\n", pid);

	return 0;
}


SYSCALL_DEFINE4(register_rm, pid_t, pid, unsigned int, period, unsigned int, deadline, unsigned int, exec_time) {
	if (period != deadline) 
		return -22;
	//period is same as deadline in RMS Scheduling
	return __register_dm(pid, deadline, deadline, exec_time);
}


SYSCALL_DEFINE4(register_dm, pid_t, pid, unsigned int, period, unsigned int, deadline, unsigned int, exec_time) {

	return __register_dm(pid, period, deadline, exec_time);
}


SYSCALL_DEFINE0(list) {
	struct my_task_struct* task;
	list_for_each_entry(task, &my_list, next_prev_list) {
		printk("PID: %u\tPeriod: %u\tDeadline: %u\tExecution Time:%u\n",task->my_task->pid, task->period, task->deadline, task->exec_time);
	}
	return 0;
}

SYSCALL_DEFINE1(remove, pid_t, pid) {
	struct my_task_struct* task = find_my_taskstruct(pid);
	if(pid<1)
		return -22;
	if (!task) 
		return -22;	
    // Remove the task from the list, delete its timer, and free memory
	list_del(&task->next_prev_list);
	del_timer(&task->my_timer);
	kfree(task);

	printk("Removed PID %u\n\n", pid);
	return 0;
}

SYSCALL_DEFINE1(yield, pid_t, pid) {
	struct my_task_struct* task;
	struct kernel_siginfo info;
	list_for_each_entry(task, &my_list, next_prev_list) {
		if (task->my_task->pid == pid) {	
			printk("Yield the process with PID %u\n", pid);	
            // Send SIGSTOP signal to suspend the task	
			info.si_signo = SIGSTOP;
			send_sig_info(SIGSTOP, &info, task->my_task);
			printk("Signal send succesfully \n");
            // Update the task state to SLEEPING
			task->state = SLEEPING;
			if (task == my_curr_tasks)
				my_curr_tasks = NULL;
			break;
		}
	}
	printk("Dispatcher is called\n");
     // Modify the task timer and wake up the dispatcher thread
	mod_timer(&task->my_timer, task->period);
	if (dispatcher_thread)
		wake_up_process(dispatcher_thread);
	else 
		printk("No running thread\n");
	return 0;
}
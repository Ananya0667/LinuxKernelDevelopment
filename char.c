#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include "linux/spinlock.h"
#include <linux/fs.h>
#include <linux/kdev_t.h>
#include <linux/slab.h>

#define mem_size 1024
#define DEVICE_COUNT 2

dev_t dev=0;   //to store major and minor numbers
char kernel_buffer[mem_size];
int write_start = 0;
static struct cdev my_cdev1, my_cdev2;
static struct class *dev_class = NULL;      //to store address of class structure

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off);
static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off);
static int my_open(struct inode *inode, struct file *file);
static int my_release(struct inode *inode, struct file *file);
static int __init my_init(void);
static void __exit my_exit(void);

static struct file_operations fops = {
    .owner      = THIS_MODULE,
    .read       = my_read,
    .write       = my_write,
    .open       = my_open,
    .release    = my_release,
};

static int my_open(struct inode *inode, struct file *file){
    printk("Device file opened successfully \n");
    return 0;
}

static int my_release(struct inode *inode, struct file *file){
    printk("Device file closed successfully \n");
    return 0;
}

static ssize_t my_read(struct file *filp, char __user *buf, size_t len, loff_t *off){
    char *output_string1;
    char *start,*end;
    char temp;
    int check_minor;
    //comment for LIFO- Writer
    //output_string1=kmalloc(strlen(kernel_buffer),GFP_KERNEL);
    //strcpy(output_string1, kernel_buffer);

    output_string1=kmalloc(strlen(kernel_buffer)+1,GFP_KERNEL);
    if (!output_string1)
        return -ENOMEM; // Memory allocation failed
    strcpy(output_string1, kernel_buffer);
    start=output_string1;
    end=output_string1+strlen(output_string1)-1;
    if (!output_string1)
        return -ENOMEM; // Memory allocation failed
    while(start<end){
        temp=*start;
        *start++=*end;
        *end--=temp;
    }
    
    printk("current written data is %s of length %ld",output_string1,strlen(output_string1));
    // If no data available, return "$"
    if (strlen(output_string1) == 0) {
        if (copy_to_user(buf, "$", 1)) {
            kfree(output_string1); // Free allocated memory before returning
            return -EFAULT; // Copying to user failed
        }
        kfree(output_string1); // Free allocated memory
        return 1; // Return 1 byte read
    }

    //check if the device is the reading device
    check_minor = MINOR(filp->f_path.dentry->d_inode->i_rdev);
     if (check_minor != 0) {
        printk("Device not allowed to read\n");
        kfree(output_string1); // Free allocated memory before returning
        return -EPERM; // Return appropriate error code
    }
     // Limit the length to read to the available data length
    if (len > strlen(output_string1))
        len = strlen(output_string1);
    
    // Copy data to user space
    if (copy_to_user(buf, output_string1, len)) {
        kfree(output_string1); // Free allocated memory before returning
        return -EFAULT; // Copying to user failed
    }
   

	printk("Read from Device file succesfully\n");
	kfree(output_string1); // Free allocated memory
    return len; // Return number of bytes read
}


static ssize_t my_write(struct file *filp, const char __user *buf, size_t len, loff_t *off){
    size_t maxdatalen = mem_size;
    int check_minor;
   
    //check if the device is the writing device
    check_minor = MINOR(filp->f_path.dentry->d_inode->i_rdev);
	if(check_minor!=1){
		printk("device not allowed to write\n");
		return -EPERM; 
	}
    printk("len of string is %zu \n",len);
    
	// Prevent buffer overrun
    if (len > maxdatalen) {
        printk("Data truncated, exceeding buffer size\n");
        len = maxdatalen;
    }

    // Copy data from user space 
    if(copy_from_user(&kernel_buffer[write_start], buf, len)){
        printk("error in writing data\n");
		return -EFAULT;
    }
    
    printk("Written %zu bytes succesfully\n", len);

    //update write_start counter
    write_start+=len;
    if (write_start >= mem_size) {
        write_start = mem_size - 1; // ensures not to go beyond buffer size
    }
    printk("Total Data in buffer is: %s\n", kernel_buffer);

    return len;
}


static int __init my_init(void){
    //registering major and minor number
    if((alloc_chrdev_region(&dev,0,DEVICE_COUNT,"mydevice"))<0){	
		pr_err("cannot allocate major number for device \n");
		return -1;
	}
    printk("Major =%d\n",MAJOR(dev));

    //creating class named as my_class
	if((dev_class=class_create(THIS_MODULE,"mydevice"))==NULL){
		pr_err("cannot create the struct class for device \n");
		unregister_chrdev_region(dev,DEVICE_COUNT);
		return -1;
	}
    
    //DEVICE 1
    //creating cdev structure
	cdev_init(&my_cdev1,&fops);

    //adding character device to the system
	if((cdev_add(&my_cdev1,MKDEV(MAJOR(dev), 0),1))<0){
		printk("cannot add device 0 to the system\n");
		class_destroy(dev_class);
        unregister_chrdev_region(dev, DEVICE_COUNT);
		return -1;
	}

    //creating device under /dev dir
	if((device_create(dev_class,NULL,MKDEV(MAJOR(dev), 0),NULL,"my_device0"))==NULL){
		pr_err("cannot create device 0 \n");
		class_destroy(dev_class);
        unregister_chrdev_region(dev, DEVICE_COUNT);
		return -1;
	}


    //DEVICE 2
     //creating cdev structure
	cdev_init(&my_cdev2,&fops);
   
    //adding character device to the system
	if((cdev_add(&my_cdev2,MKDEV(MAJOR(dev), 1),1))<0){
		printk("cannot add device 1 to the system\n");
		class_destroy(dev_class);
        unregister_chrdev_region(dev, DEVICE_COUNT);
		return -1;
	}

    //creating device under /dev dir
	if((device_create(dev_class,NULL,MKDEV(MAJOR(dev), 1),NULL,"my_device1"))==NULL){
		pr_err("cannot create device 1 \n");
		class_destroy(dev_class);
        unregister_chrdev_region(dev, DEVICE_COUNT);
		return -1;
	}

    printk("Kernel module succesfully inserted\n");
    return 0;
}

static void __exit my_exit(void){
    device_destroy(dev_class, MKDEV(MAJOR(dev), 0));
    device_destroy(dev_class, MKDEV(MAJOR(dev), 1));

    // // Destroy both cdev structures
    cdev_del(&my_cdev1);
    cdev_del(&my_cdev2);

    class_unregister(dev_class);
    class_destroy(dev_class);
    unregister_chrdev_region(dev,DEVICE_COUNT);
    printk("Kernel module sucessfully removed\n");
}

MODULE_LICENSE("GPL");
module_init(my_init);
module_exit(my_exit);

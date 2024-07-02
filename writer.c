// /dev/my_device1 -Writer
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>

#define MAX_BUF_SIZE 100
int main (int argc , char * argv [])
{
    char device [MAX_BUF_SIZE];
    char user_msg [MAX_BUF_SIZE];
    int fd;
    if(argc!=3){
    	printf("error in arguments %s\n",argv[0]);
	return -1;
    }    
    strcpy(device,argv[1]);

    strcpy(user_msg,argv[2]);

    fd = open(device,O_WRONLY);
     if(fd==-1){
	printf("cannot open device file \n");
	return -1;
    }
    printf("opened file descriptor %d\n", fd);

    //write to device
    ssize_t bytes_written=write(fd, user_msg, strlen(user_msg));
    if(bytes_written==-1){
    	printf("cannot write to device \n");
	close(fd);
	return -1;
    }
    printf("written %zd bytes to device \n",bytes_written);
    close(fd);
    return 0;
}

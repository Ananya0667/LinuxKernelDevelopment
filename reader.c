// /dev/my_device0 -Reader
#include <stdio.h>
#include <fcntl.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_BUF_SIZE 1024

int main (int argc , char * argv [])
{
    char device[MAX_BUF_SIZE];
    char user_msg[MAX_BUF_SIZE];
    int fd;
    if(argc!=2){
	    printf("error in # of arguments %s\n",argv[0]);
	    return -1;
    }
    strcpy( device , argv [1]);
    fd = open(device,O_RDONLY);
    if(fd==-1){
	    printf("cannot open device file \n");
	    return -1;
    }
    printf("Reading from file %d\n",fd);
    //memset(user_msg,0,sizeof(user_msg));
    ssize_t bytes_read=read(fd, user_msg, MAX_BUF_SIZE-1);
    if(bytes_read==-1){
	    printf("cannot read from device");
	    close(fd);
	    return -1;
    }
    printf("String read from device is %s\n", user_msg);
    close(fd);
    return 0;
}

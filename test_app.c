#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<fcntl.h>
#include<string.h>
#include<unistd.h>
#include<sys/ioctl.h>
#include<stdint.h>


typedef struct time 
{
	unsigned int hr;
	unsigned int min;
	unsigned int sec;
}time_tr;

static time_tr t;
#define SUCCESS		 0
#define NO_DEVICE  	-1
#define DEV_NOT_INIT	-2
#define INIT_LCD	_IOWR('a',3,uint8_t*)
#define SET_TIME_LCD	_IOWR('b',3,uint8_t*)
int main()
{
	int ret;
        int fd;
	int val;
	fd = open("/dev/I2C_CHAR_DEV",O_RDWR);
	if(fd < 0)
	{
		perror("Failed to open the device\n");
		return -1;
	}
	while(1) 
	{
	  printf("please select menu:\n1. Initialize lcd\n2. Set-time\n3. Exit\n");
	  scanf("%d",&ret);
	  switch(ret)
	  {
	    case 1:
		val = ioctl(fd,INIT_LCD,NULL);
 		if(val == SUCCESS)
			printf("device initialised successfully\n");
		else if (val == NO_DEVICE)
			printf("No device! couldn't be initialised\n");
		break;
	    case 2:
		printf("enter hour:\n");
		scanf("%d",&t.hr);
		printf("enter minute:\n");
		scanf("%d",&t.min);
		if(t.hr <=23 && t.min <= 60)
                {
		  write(fd,&t,sizeof(t));
		  val = ioctl(fd,SET_TIME_LCD,NULL);
		  if(val == SUCCESS)
		  {
		     printf("time set success\n");
		  }else if(val == DEV_NOT_INIT|| val == NO_DEVICE) {
		   printf("device is not initialised or device not found!\n");
		 }
		}else { printf("time is out of range!\n"); }
		break;
	    case 3:
		printf("closing driver\n");
		close(fd);
		exit(1);
	    default:
		break;
	  }
	}
}

#include "header.h"
#include "prototype.h"

static int thread_func(void * arg)
{
  allow_signal(SIGKILL);
  allow_signal(SIGINT);
  allow_signal(SIGALRM);
  while(!kthread_should_stop())
  {
	set_current_state(TASK_INTERRUPTIBLE);
	if(signal_pending(current))
	{
		siginfo_t info;
		unsigned long signr;
		signr = dequeue_signal(current, &current->blocked, &info);
		switch(signr)
		{
		  case SIGALRM:
				blink();
				break;			
		  case SIGINT:
			printk(KERN_INFO "SIGINT to change time is recieved\n");
			if(is_lcd_initialized())
			{
				update_time_in_lcd();
			}
			break;
		  case SIGKILL:
			printk(KERN_INFO "signal recieved is SIGKILL\n");
			goto die;
		  default:
			break;
		}
	}
  }
  
die:  thread_update = NULL;
      do_exit(0);
}
void _lcd_init_update(void)
{
	INIT_CPMT = TRUE;
}
int is_lcd_initialized(void)
{
	return INIT_CPMT;
}
void timer_callback(struct timer_list * timer)
{
	if(SET_TIME == TRUE)
	{
	    ++t.sec;
	    if(t.sec > 59)
	    {		
		++t.min;
		t.sec = 0;
		if( t.min > 59 )
		{
			++t.hr;
			t.min = 0;
			if( t.hr > 24 )
			{
				t.hr = 0;
			}
		}
		send_sig(SIGINT,thread_update, 1);
		printk(KERN_INFO "time change is triggered\n");
	     }
	     send_sig(SIGALRM,thread_update,1);
	}
	mod_timer(&chr_timer,jiffies+msecs_to_jiffies(TIMEOUT));
}
static void start(void)
{
  gpio_set_value(DIO, OFF_STATE);	
  msleep(0.5);
}
static void stop(void)
{
  gpio_set_value(DIO, OFF_STATE);
  msleep(0.5);
  gpio_set_value(CLK, ON_STATE);
  msleep(0.5);
  gpio_set_value(DIO, ON_STATE);
}
static int sendByte(unsigned char ch)
{
	int i;
	int errno = 0;
	int val,count = 0;
	for(i=0;i<8;i++)
	{
		gpio_set_value(CLK,OFF_STATE);
		msleep(0.5); 
		if(ch&0x01)
			gpio_set_value(DIO,ON_STATE);
		else
			gpio_set_value(DIO,OFF_STATE);
		msleep(0.5);
		gpio_set_value(CLK,ON_STATE);
		msleep(0.5);
		ch>>=1;
	}
	gpio_set_value(CLK,OFF_STATE);
	gpio_set_value(DIO,ON_STATE);
	msleep(0.5);
	if((errno = gpio_direction_input(DIO)) != 0)
	{
		printk(KERN_INFO "Can't set GPIO direction, error %i\n", errno);
		return -EINVAL;
	}
	gpio_set_value(CLK,ON_STATE);
	
	while((val = gpio_get_value(DIO)) != 0)
	{
		if(val == 0) {
			break;
		} else if(count > TIMEOUT_I2C) {  
			errno = NO_DEVICE;
			break;
		}
		count++;
	}
	gpio_direction_output(DIO,OFF_STATE);
	gpio_set_value(CLK,OFF_STATE);
	return errno;
}
void i2c_send_byte(uint8_t pos,uint8_t msg)
{
   mutex_lock(&etx_mutex);
   if(is_lcd_initialized()) {
   start();
   sendByte(pos);
   sendByte(msg);
   stop();
   }
   mutex_unlock(&etx_mutex);
}
void blink(void)
{
	int h1;
	char seg;
	h1 = t.hr%10;
	if(blink_dot == TRUE)
	{
		seg = segments[h1];
		seg ^= (1<<7);
		i2c_send_byte(0xC1,seg);
	}else{
	i2c_send_byte(0xC1,segments[h1]);
	}
	blink_dot = !blink_dot;
}

static int initLED(void)
{
	int k;
	int pos = 0xC0;
	int status;
	if( is_lcd_initialized() != TRUE )
	{
		start();
		status = sendByte(0x40);
		stop();
		if(status == SUCCESS)
		{
			for(k=0;k<4;k++)
			{
			start();
			sendByte(pos);
			sendByte(segments[0]);
			stop();
			pos++;
			}
		
			start();
			sendByte(0x88);
			stop();
			_lcd_init_update();
		}
	}
	return status;	
}

void update_time_in_lcd(void)
{
	int temp = t.hr;
	int h1,h2;
	h1 = temp%10;
	h2 = temp/10;
	switch(h2)
	{
		case 0:
			i2c_send_byte(0xC0,segments[h2]);
			i2c_send_byte(0xC1,segments[h1]);
			break;
		default:
			i2c_send_byte(0xC0,segments[h2]);
			i2c_send_byte(0xC1,segments[h1]);
			break;
	}
	temp = t.min;
	h1 = temp%10;
        h2 = temp/10;
        switch(h2)
        {
                case 0:
			i2c_send_byte(0xC2,segments[h2]);
			i2c_send_byte(0xC3,segments[h1]);
                        break;
                default:
			i2c_send_byte(0xC2,segments[h2]);	
			i2c_send_byte(0xC3,segments[h1]);
			break;
	}
}
/*
void send_msg(void)
{
	int pos = 0xC3;
	int i,j,k=0x01,l=0,lim;
	int chr[11]={ 0x73,0x30,0x7B,0x5F,0x6D,0x7B,0x00,0x6D,0x7B,0x78,0x00 };
	for(i=0;i<4;i++)
	{
		start();
		sendByte(pos);
		sendByte(0x00);
		stop();
		--pos;
	}
	pos = 0xC3;
	for(i=0;i<10;i++)
	{
		if( k%4 != 0 && l == 0 )
		{
			pos = pos - (k-0x1);
		}else{ pos =0xC0; l++; }
		lim = (i<3)?i:3;
		//printk(KERN_INFO "%s: Printing Msg..\n",DEVICE_NAME);
		for(j=0;j<=lim;j++)
		{
			start();
			sendByte(pos);
			if(l==0 || l==1)
				sendByte(chr[j+0]);
			else
				sendByte(chr[j+l]);
			stop();
			pos++;
		}
		pos = 0xC3;
		k++;
	}
	//printk(KERN_INFO "%s: MSG_ENDS\n",DEVICE_NAME);
	
}*/
/*-------------- FileIO_opr --------------*/

static int dev_open(struct inode *inodep, struct file *filep)
{
  printk(KERN_INFO "%s: Device successfully opened\n",DEVICE_NAME);
  return 0; 
}
static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
  printk(KERN_INFO "%s: reading device is in progress\n",DEVICE_NAME);
  return 0;
}
static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
  int ret;
  printk(KERN_INFO "%s: writing device is in progress\n",DEVICE_NAME);
  ret = copy_from_user(&t,buffer,len);
  t.sec = 0;
  return ret;
}
static int dev_close(struct inode *inodep, struct file *filep)
{
   printk(KERN_INFO "%s: Device successfully closed\n",DEVICE_NAME);
   return 0;
}
static long chr_ioctl(struct file* filep,unsigned int cmd, unsigned long arg)
{
  int status = 0;
  printk(KERN_INFO "%s: Executing command is in progress: %d\n",DEVICE_NAME,cmd);
  switch(cmd)
  {
    case INIT_LCD:
		printk(KERN_INFO "%s: Initializing the led.\n",DEVICE_NAME);
		if( initLED() == SUCCESS )
		{	
			/* print "strt" */
			i2c_send_byte(0xC0,0xED);
			i2c_send_byte(0xC1,0x78);
			i2c_send_byte(0xC2,0x50);
			i2c_send_byte(0xC3,0x78);
			printk(KERN_INFO "%s: Initialization completes!\n",DEVICE_NAME);
		}else{
			printk(KERN_INFO "%s: couldn't initialised, maybe device not connected\n",DEVICE_NAME);  
			status = NO_DEVICE; 
		}
		break;
    case SET_TIME_LCD:
		if(t.hr <=23 && t.min <= 60)
 		{
			SET_TIME = TRUE;
    			if(is_lcd_initialized()){
        		update_time_in_lcd();
			} else { status = DEV_NOT_INIT; }
		}
		break;
    default:
		return -EINVAL;
		break;
  }
  return status;	
}
/*-------------- ~~~ End ~~~ -------------*/
static int __init mod_init(void)
{
	majorno = register_chrdev(0,DEVICE_NAME,&fops);
	if(majorno < 0)
	{
		printk(KERN_INFO "%s: couldn't register\n",DEVICE_NAME);
		return majorno;
	}
	printk(KERN_INFO "%s: register success\n",DEVICE_NAME);
	/* Register the device class */
	charClass = class_create(THIS_MODULE, CLASS_NAME);
	if(IS_ERR(charClass))
	{	/* Check for error and clean up if there is */
		unregister_chrdev(majorno,DEVICE_NAME);
		printk(KERN_INFO "%s: Failed to register device class\n",DEVICE_NAME);
		/* Correct way to return an error on a pointer */
		return PTR_ERR(charClass); 
	}
	printk(KERN_INFO "%s: device class registered correctly\n",DEVICE_NAME);
	/* Create device file */
	charDev = device_create(charClass, NULL, MKDEV(majorno, 0), NULL, DEVICE_NAME);
	if(IS_ERR(charDev))
	{
		class_destroy(charClass);
		unregister_chrdev(majorno,DEVICE_NAME);
		printk(KERN_INFO "%s: Failed to create the device file\n",DEVICE_NAME);
		return PTR_ERR(charDev);
	}
	printk(KERN_INFO "%s: created device file\n",DEVICE_NAME);
	gpio_direction_output(CLK, OFF_STATE);
	gpio_direction_output(DIO, OFF_STATE);
	/* setup the timer */
	timer_setup(&chr_timer,timer_callback,0);
	/* setup the timer interval */
	mod_timer(&chr_timer,jiffies+msecs_to_jiffies(TIMEOUT));
	thread_update = kthread_run(thread_func,NULL,"my_thread");
	mutex_init(&etx_mutex);	
	return 0;
}
static void __exit mod_exit(void)
{
	gpio_set_value(CLK, OFF_STATE);
	gpio_set_value(DIO, OFF_STATE);
	gpio_free(CLK);
	gpio_free(DIO);

	del_timer(&chr_timer);
	if(thread_update !=NULL)
	{
		kthread_stop(thread_update);
	}
	/* remove the device file */
	device_destroy(charClass, MKDEV(majorno, 0));
	/* unregister the device class */     
   	class_unregister(charClass);
        /* remove device class  */
   	class_destroy(charClass);
        /* unregister the char driver */
   	unregister_chrdev(majorno, DEVICE_NAME);
	printk(KERN_INFO "%s: Goodbye, lkm is removed\n",DEVICE_NAME);
}

MODULE_LICENSE("GPL"); //module is registered with General public license
MODULE_AUTHOR("Sajan Jha");
module_init(mod_init);
module_exit(mod_exit);


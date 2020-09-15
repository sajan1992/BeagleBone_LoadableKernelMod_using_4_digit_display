#include<linux/module.h>
#include<linux/init.h>
#include<linux/gpio.h>
#include<linux/delay.h> 
#include<linux/timer.h>
#include<linux/jiffies.h>
#include<linux/kthread.h>
#include<linux/mutex.h>
#include<linux/uaccess.h>
#include<linux/sched.h>
#include<linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
#include<linux/sched/signal.h>
#endif

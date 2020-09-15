#define CLASS_NAME  "I2C_Dev"
#define DEVICE_NAME "I2C_CHAR_DEV"

#define TRUE  1
#define FALSE 0

static unsigned INIT_CPMT = 0;
static int majorno;
static char blink_dot;
static int     dev_open(struct inode *, struct file *);
static int     dev_close(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static long chr_ioctl(struct file*,unsigned int, unsigned long);

static struct class * charClass = NULL;
static struct device * charDev  = NULL;

static struct file_operations fops = 
{
        .open     	= dev_open,
        .release 	= dev_close,
        .write   	= dev_write,
        .read    	= dev_read,
	.unlocked_ioctl = chr_ioctl,
};

/*-------------------------------------*/

static unsigned int CLK   = 60;
static unsigned int DIO   = 48;

typedef struct time 
{
	unsigned int hr;
	unsigned int min;
	unsigned int sec;
}time_tr;

#define OFF_STATE    0
#define ON_STATE     1 

#define INIT_LCD 	_IOWR('a',3,uint8_t*)
#define SET_TIME_LCD 	_IOWR('b',3,uint8_t*)
const unsigned char segments[] = {
 // XGFEDCBA
  0xBF,    // :0
  0x86,    // :1
  0xDB,    // :2
  0xCF,    // :3
  0xE6,    // :4
  0xED,    // :5
  0xFD,    // :6
  0x87,    // :7
  0xFF,    // :8
  0xEF,    // :9
  0xF7,    // :A
  0xFC,    // :b
  0xB9,    // :C
  0xDE,    // :d
  0xF9,    // :E
  0xF1     // :F
  };

#define TIMEOUT 	 1000 /* in millisec */
#define SUCCESS 	 0
#define NO_DEVICE 	-1
#define DEV_NOT_INIT 	-2
#define TIMEOUT_I2C 	100

bool SET_TIME = FALSE;
static struct timer_list chr_timer;
static struct task_struct * thread_update;
int is_lcd_initialized(void);
void _lcd_init_update(void);
static time_tr t;

void update_time_in_lcd(void);
int is_lcd_initialized(void);
void send_msg(void);
void blink(void);
void i2c_send_byte(uint8_t pos,uint8_t msg);
static int sendByte(unsigned char ch);
static void stop(void);
static void start(void);

struct mutex etx_mutex;
/*------------------------------------*/

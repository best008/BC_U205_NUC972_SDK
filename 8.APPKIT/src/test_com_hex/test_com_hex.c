
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/socket.h>


#include "device_serial.h"

#define dmsg_on 0
#if (dmsg_on)
#define dmsg(msg...)                                    \
	do                                                  \
	{                                                   \
		printf("[MSG]:%04d,%s:\t", __LINE__, __func__); \
		printf(msg);                                    \
	} while (0)
#else
#define dmsg(msg...)
#endif

#define derr_on 0
#if (derr_on)
#define derr(msg...)                                    \
	do                                                  \
	{                                                   \
		printf("[ERR]:%04d,%s:\t", __LINE__, __func__); \
		printf(msg);                                    \
	} while (0)
#else
#define derr(msg...)
#endif

/*串口通信变量####################################################################*/

/*最大支持串口数*/
#define DEF_COM_MAX 8
/*默认串口波特率*/
#define DEF_COM_BAUDRATE 115200
/*默认数据位*/
#define DEF_DATA_BITS 8
/*默认停止位*/
#define DEF_DATA_STOP 1
/*默认校验位*/
#define DEF_DATA_PARITY PARITY_NONE
/*收发间隔毫秒数*/
#define DEF_TIME_ITV 3000
#define DEF_BUF_SIZE 256

/*存放串口名称*/
const char *device_com_path[] =
	{
		"/ual/com1",
		"/ual/com2",
		"/ual/com3",
		"/ual/com4",
		"/ual/com5",
		"/ual/com6",
		"/ual/com7",
		"/ual/com8",
};

/*存放串口实例*/
serial_t serial_com_objs[DEF_COM_MAX];

/*串口端口互斥锁，防止读写线程同时访问设备*/
pthread_mutex_t serial_com_mutex[DEF_COM_MAX];

/*串口接收线程条件， 当条件到达时，开始接收*/
pthread_cond_t serial_recv_cond[DEF_COM_MAX];

/*串口接受线程条件互斥锁，防止在等待条件时锁死*/
pthread_mutex_t serial_recv_cond_mutex[DEF_COM_MAX];

/*用于标记串口是否初始化*/
int serial_com_flags[DEF_COM_MAX];
int serial_com_flags_inited = 0;

char *com12_str = "1234567\0";

unsigned char com_send_buf[DEF_BUF_SIZE];
unsigned char com_recv_buf[DEF_BUF_SIZE];

/*串口索引*/
int com_idx = 0;
int com_baud= 0;

int com_send_len = 0;

/*串口发送线程*/
pthread_t com_send_tid = -1;
/*串口接收线程*/
pthread_t com_recv_tid = -1;

char fmt_char_to_hex(char c)
{
	if((c>='0')&&(c<='9'))
		return c-0x30;
	else if((c>='A')&&(c<='F'))
		return c-'A'+10;
	else if((c>='a')&&(c<='f'))
		return c-'a'+10;
	else 
		return 0x10;
}

int fmt_hexstr_to_hexarray(char *hexstr, char *hexarray)
{
	int t,t1;
	int i    = 0;
	int len  = 0;
	int rlen = 0;

	len = strlen(hexstr);

	dmsg("get hexstr length: %d\n", len);
	if(len%2 != 0)
	{
		derr("hexstr length: %d \n", len);
		return -1;
	}

	for(i=0;i<len;)
	{
		char l,h=hexstr[i];
		if(h==' ')
		{
			i++;
			continue;
		}
		i++;
		if(i>=len)
		{
			break;
		}
		l =hexstr[i];
		t =fmt_char_to_hex(h);
		t1=fmt_char_to_hex(l);
		if((t==16)||(t1==16))
		{
			break;
		}
		else 
		{
			t=t*16+t1;
		}
		i++;
		hexarray[rlen]=(char)t;
		rlen++;
	}
	return rlen;
}

int fmt_hexarray_to_hexstr(char *hexstr, char *hexarray, int len)
{
	int i = 0;
	char *phexstr = hexstr;

	for (i = 0; i < len; i++)
	{
		sprintf(phexstr, "%02X", hexarray[i]);
		phexstr+=2;
	}
	sprintf(phexstr, "\0");
	return 0;
}

int com_init_devices(void)
{
	int i = 0;

	for (i = 0; i < DEF_COM_MAX; i++)
	{
		pthread_mutex_init(&serial_com_mutex[i], NULL);
	}

	for (i = 0; i < DEF_COM_MAX; i++)
	{
		pthread_cond_init(&serial_recv_cond[i], NULL);
	}

	for (i = 0; i < DEF_COM_MAX; i++)
	{
		pthread_mutex_init(&serial_recv_cond_mutex[i], NULL);
	}
	return 0;
}

int com_deinit_devices(void)
{
	int i = 0;

	for (i = 0; i < DEF_COM_MAX; i++)
	{
		pthread_mutex_destroy(&serial_com_mutex[i]);
	}

	for (i = 0; i < DEF_COM_MAX; i++)
	{
		pthread_cond_destroy(&serial_recv_cond[i]);
	}

	for (i = 0; i < DEF_COM_MAX; i++)
	{
		pthread_mutex_destroy(&serial_recv_cond_mutex[i]);
	}
	return 0;
}

int init_com_default(int comidx)
{
	int ret = 0;
	int i = 0;
	if (serial_com_flags_inited != 1)
	{
		for (i = 0; i < DEF_COM_MAX; i++)
		{
			serial_com_flags[i] = 0;
		}
		serial_com_flags_inited = 1;
	}
	if (serial_com_flags[comidx] == 0)
	{
		ret = serial_open_advanced(&serial_com_objs[comidx], device_com_path[comidx], com_baud, DEF_DATA_BITS, DEF_DATA_PARITY, DEF_DATA_STOP, false, false);
		if (ret != 0)
		{
			derr("open com:%d with default config , failed, ret: %d\n", comidx + 1, ret);
			return -1;
		}
		dmsg("open com:%d with default config , done!\n", comidx + 1);
		serial_com_flags[comidx] = 1;
	}
	return 0;
}

void com_send_thread(void *data)
{
	int i = 0;
	int wlen_ret = 0;

	if (0 != init_com_default(com_idx))
	{
		derr("init com%d failed!\n", com_idx + 1);
		return;
	}

	dmsg("start process ...\n");

#if (dmsg_on)
	dmsg("try com%d send len %d msg: \n", com_idx+1, com_send_len);
	for(i = 0; i < com_send_len; i++)
	{
		printf("%02x ", com_send_buf[i]);
	}
	printf("\n");
#endif

	pthread_mutex_lock(&serial_com_mutex[com_idx]);
	wlen_ret = serial_write(&serial_com_objs[com_idx], com_send_buf, com_send_len);
	serial_flush(&serial_com_objs[com_idx]);
	pthread_mutex_unlock(&serial_com_mutex[com_idx]);

	if (wlen_ret != com_send_len)
	{
		derr("send com%d  len: %d, return len: %d\n", com_idx+1, com_send_len, wlen_ret);
	}else
	{

		dmsg("finish com%d send len %d msg: %s\n", com_idx+1, com_send_len, com_send_buf);
	}
	pthread_cond_signal(&serial_recv_cond[com_idx]);
	dmsg("stop process ...\n");
}

void com_recv_thread(void *recv)
{
	char readbuf[DEF_BUF_SIZE];
	int i = 0;
	int rlen_ret = 0;

	if (0 != init_com_default(com_idx))
	{
		derr("init com%d failed!\n", com_idx + 1);
		return;
	}

	dmsg("start process ...\n");

#if 1
	pthread_mutex_lock(&serial_recv_cond_mutex[com_idx]);
	pthread_cond_wait(&serial_recv_cond[com_idx], &serial_recv_cond_mutex[com_idx]);
	pthread_mutex_unlock(&serial_recv_cond_mutex[com_idx]);
#endif

	pthread_mutex_lock(&serial_com_mutex[com_idx]);
	rlen_ret = serial_read(&serial_com_objs[com_idx], readbuf, DEF_BUF_SIZE, DEF_TIME_ITV);
	serial_flush(&serial_com_objs[com_idx]);
	pthread_mutex_unlock(&serial_com_mutex[com_idx]);

#if (dmsg_on)
	if(rlen_ret > 0)
	{
		dmsg("read com%d len %d msg: \n", com_idx+1, rlen_ret);
		for(i = 0; i < rlen_ret; i++)
		{
			printf("%02x ", readbuf[i]);
		}
		printf("\n");
	}
#endif

	if(rlen_ret > 0)
	{
		fmt_hexarray_to_hexstr(com_recv_buf, readbuf, rlen_ret);
		printf("%s\n", com_recv_buf);

	}
	dmsg("stop process ...\n");
}
/*------------------------------------------------------------------SERIAL*/

#if 1
void show_helpmsg(void)
{
	printf("Usage:\n");
	printf("      ./test_comhex com_port(1~8)  com_baudrate  sendhexstr\n");
	printf("      ./test_comhex -h shello this help message\n");
}

/*example: test_comhex com8 9600 010302100006c5b5*/
int main(int argc, char **argv)
{
	int com_no    = -1;

	if(((argc == 2) && (strcmp(argv[1], "-h") == 0)) || (argc == 1) || (argc != 4))
	{
		show_helpmsg();
		return -1;
	}

	com_no = atoi(argv[1]);
	if(com_no <= 0 || com_no >8)
	{

		dmsg("error: com port: %d\n", com_no);
		return -1;
	}


	com_idx  = com_no -1;
	com_baud = atoi(argv[2]);

	dmsg("com_no: %d, com_idx: %d\n", com_no, com_idx);

	/*初始化串口设备*/
	com_init_devices();

	memset(com_send_buf, 0, DEF_BUF_SIZE);
	memset(com_recv_buf, 0, DEF_BUF_SIZE);

	com_send_len = fmt_hexstr_to_hexarray(argv[3], com_send_buf);

	if(com_send_len <= 0)
	{
		return -1;
	}
#if 1
	pthread_create(&com_recv_tid, NULL, (void *)com_recv_thread, NULL);
	usleep(200 * 1000);
	pthread_create(&com_send_tid, NULL, (void *)com_send_thread, NULL);
#endif

	pthread_join(com_send_tid, NULL);
	pthread_join(com_recv_tid, NULL);

	return 0;
}
#endif

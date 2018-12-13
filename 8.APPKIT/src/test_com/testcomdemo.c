/*********************************************************************
* Filename      :   testcom.c
* Description   :   The main program to test the Serial ports
* Function      :
*
*
* Change Log:
* Rev. V1.0 2018-08-25 Created by TONY
*********************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <ctype.h>
#include <signal.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define CMSPAR			010000000000
#define B8000			0010016
#define B10416			0010017

#define ST_STOP1        0x0001
#define ST_STOP15       0x0003   //1.5 bit
#define ST_STOP2        0x0002
#define ST_LOOPBACK     0x0020
#define ST_CS5          0x0060
#define ST_CS6          0x0080
#define ST_CS7          0x00a0
#define ST_CS8          0x00c0
#define ST_PARNONE      0x0004
#define ST_PARODD       0x0008
#define ST_PAREVEN      0x000c
#define ST_PARSPACE		0x0010
#define ST_PARMASK		0x0014
#define ST_B300         0xb700
#define ST_B600         0xb800
#define ST_B1200        0xb900
#define ST_B1800        0xba00
#define ST_B2400        0xbb00
#define ST_B4800        0xbc00
#define ST_B7200        0xbd00
#define ST_B9600        0xbe00
#define ST_B19200       0xbf00
#define ST_B38400       0xc000
#define ST_B57600       0xc100
#define ST_B115200      0xc200

#define READ_RESET_JUDGE_REG   1
#define READ_VERSION_REG       2
#define READ_ID_REG_HIGH       3
#define READ_ID_REG_LOW        4

//#define _DEBUG
#define GZ_TEST_ALL
#define GZ_TEST_422
//#define GZ_TEST_

char ttyS1[20]="/dev/ttyS";
char ttyS2[20]="/dev/ttyS";

int set_Parity(int fd,int databits,int stopbits,int parity);
int set_speed(int fd, int speed);

int speed_arr[] = { B115200, B57600, B38400, B19200, B9600, B8000,B4800, B2400, B1200, B300,};
int name_arr[] = {115200, 57600, 38400,  19200,  9600, 8000, 4800,  2400,  1200,  300,};
int i=0;


typedef struct
{
	unsigned char sError;
	unsigned char rError;
} ERROR, *PERROR;


int set_speed(int fd,int speed)
{
	int status;
	struct termios newtio;
	tcgetattr(fd,&newtio);
#ifdef _DEBUG
	printf("\n******speed = %d******\n", speed);
#endif
	switch(speed)
	{
	case 115200:
		speed=B115200;
		break;
	case 57600:
		speed=B57600;
		break;
	case 38400 :
		speed=B38400;
		break;
	case 19200 :
		speed=B19200;
		break;
	case 9600 :
		speed=B9600;
		break;
	case 4800 :
		speed=B4800;
		break;
	case 2400 :
		speed=B2400;
		break;
	case 1200 :
		speed=B1200;
		break;
	case 300 :
		speed=B300;
		break;
	case 20833:
		speed=20833;
		break;
	case 10416:
		speed=B10416;
		break;
	case 8000:
		speed=B8000;
		break;
	default :
		printf("Set speed error!\n");
		exit(0);
	}

	tcflush(fd,TCIOFLUSH); //stop read and write to port
	cfsetispeed(&newtio,speed);
	cfsetospeed(&newtio,speed);
	status=tcsetattr(fd,TCSANOW,&newtio);
	if(status!=0)
	{
		perror("tcsetattr fd");
		printf("set parameter err!\n");
		return ;
	}
	tcflush(fd,TCIOFLUSH);
}

int set_Parity(int fd,int databits,int stopbits,int parity)
{
	struct termios options;
	if(tcgetattr(fd,&options)!=0)
	{
		perror("SetupSerial 1");
		return(-1);
	}


	options.c_cflag&=~CSIZE;  //clear the old config of databits
	switch(databits)
	{
	case 7:
		options.c_cflag|=CS7;
		break;
	case 8:
		options.c_cflag|=CS8;
		break;
	case 6:
		options.c_cflag|=CS6;
		break;
	default:
		fprintf(stderr,"Unsupported data size!\n");
		return(-1);
	}

#ifdef _DEBUG
	printf("parity = \"%c\"\n", parity);
#endif

	options.c_cflag&=~PARENB;  /* default, no parity */
	switch(parity)
	{
	case 'n':
	case 'N':
		options.c_cflag&=~PARENB; //clear parity enable
		options.c_iflag&=~INPCK;  //enable parity checking
		break;
	case 'o':
	case 'O':
		options.c_cflag|=(PARODD|PARENB); //Set odd check
		options.c_iflag|=INPCK;         //Disable parity checking
		break;
	case 'e':
	case 'E':
		options.c_cflag|=PARENB;  //Enable parity
		options.c_cflag&=~PARODD; //set even check
		options.c_iflag|=INPCK;   //disable parity checking
		break;
#if 1
	case 's':
		options.c_cflag |=PARENB | CMSPAR;
		options.c_cflag &= ~PARODD;
		break;
	case 'm':
		options.c_cflag |= PARENB | CMSPAR | PARODD;
		break;
#else
	case 'm':
		printf("fan\n");
	case 'M':
		options.c_cflag|=PARENB;  //Enable parity
		options.c_cflag&=~PARODD; //set even check
		break;
	case 's':
		printf("fan\n");
	case 'S':
		options.c_cflag&= ~PARENB;  //Enable parity
		options.c_cflag&= ~CSTOPB; //set even check
		break;

#endif
	default:
		fprintf(stderr,"Unsupported parity!\n");
		return(-1);
	}

	switch(stopbits)
	{
	case 1:
		options.c_cflag&=~CSTOPB;
		break;
	case 2:
		options.c_cflag|=CSTOPB;
		break;
	default :
		fprintf(stderr,"Unsupported stop bits!\n");
		return(-1);
	}

//Set input parity option
	if(parity!='n')
		options.c_iflag|=INPCK;
	tcflush(fd,TCIFLUSH);
	options.c_cc[VTIME]=0; //SET timeout time 15s
	options.c_cc[VMIN]=0;    //update the options and do it now
	options.c_lflag&=~(ICANON|ECHO|ECHOE|ISIG); //Input
	options.c_oflag&=~OPOST;  //Output
	if(tcsetattr(fd,TCSANOW,&options)!=0)
	{
		perror("SetupSerial 3");
		return(-1);
	}

	return(0);

}

/******************************************************************
Function	：  main
Description	：  The main function
Parameter	:	argc, **argv
Call		：  None
Called by	：  None
Input		：  None
Return		：  None
******************************************************************/
int main(int argc, char **argv)
{
	int length,count;
	int filep1,filep2,filep3,filep4,filep5,filep6,filep7,filep8,filep9,filep10,filep11,filep12,filep13,filep14,filep15,filep16;
	char COM1PATH[20]= "";
	char COM2PATH[20]= "";
	char rbuf[512];
	float loss=00.0;
	int value;

	char Send[10] ;
	char Send_temp[10] = "123456789a";
	char GZTest[100] = {0};
	char Test1[40] = {0};
	int controlword = 0;
	int baudrate=115200;
	int count_send=0;
	int count_failed=0;

	controlword  |=  ST_CS8;
	controlword  |=  ST_STOP1;
	controlword  |=  ST_PARNONE;
	controlword  |=  ST_B19200;
	if(((argc == 2) && (strcmp(argv[1], "-h") == 0)) || (argc == 1) || (argc != 4))
	{
		printf("Usage:\n");
		printf("     ./test_com  1 2 115200\n");
		// printf(" Parameters:\n");		
		return -1;
	}
	else
	{
		int port1 = atoi(argv[1]);
		sprintf(COM1PATH,"/ual/com%d",port1);
		int port2 = atoi(argv[2]);
		sprintf(COM2PATH,"/ual/com%d",port2);
		int baud_rate = atoi(argv[3]);
		/*if ((baud_rate < 0) || (baud_rate > 6))
		{
			printf("ERR:baudrate\n");
			exit 0;
		}*/

		{
			//测试串口1和串口2自环

			filep1 = open(COM1PATH, O_RDWR | O_NOCTTY | O_NDELAY);
			if(filep1 <=0)
			{
				printf("failed to open com%d\n",port1);
				return -1;
			}

			filep2 = open(COM2PATH, O_RDWR | O_NOCTTY | O_NDELAY);
			if(filep2 <=0)
			{
				printf("failed to open com%d\n",port2);
				return -1;
			}


			set_speed(filep1,baudrate);
			set_Parity(filep1,8,1,'n');
			set_speed(filep2,baudrate);
			set_Parity(filep2,8,1,'n');

			//Send[0]='X';


			for(count=0; count<2; count++)
			{
				//5次循环测试程序开始
				/*1st :  COM0/COM2/COM4 send; COM1/COM3/COM5 receive*/
				//Send[0]=Send_temp[count];
				//write(filep1, Send, 1);
				// printf("+----------------------------------------------------------------+\n");
				// printf("COM test: COM%d send,COM%d Receive\n",port1,port2);
				memset(GZTest,0x31,sizeof(GZTest));
				GZTest[99] = 0;

				write(filep1, GZTest, sizeof(GZTest));
				count_send+=1;

				sleep(1);

				memset(rbuf,0,sizeof(rbuf));
				length=read(filep2,rbuf,300);

				if((length!=100)||(strcmp(rbuf, GZTest) != 0))
				{
					count_failed+=1;
					// printf("Send length  to  com%d is %d  data is %s \n",port2,sizeof(GZTest),GZTest);
					// printf("Read length from com%d is %d  data is %s \n",port2,length,rbuf );
				}


				loss=((float)count_failed/count_send)*100;
				// printf("  COM test result:Send = %6d  Failed = %6d .loss(%2.4F%)  \n",count_send,count_failed,loss);
				// printf("+----------------------------------------------------------------+\n");
				// printf("+----------------------------------------------------------------+\n");
				// /*2nd :  COM1/COM3/COM5 send; COM0/COM2/COM4 receive*/
				// printf("COM test: COM%d send,COM%d Receive\n",port2,port1);

				memset(GZTest,0x32,sizeof(GZTest));
				GZTest[99] = 0;
				write(filep2, GZTest, sizeof(GZTest));
				count_send+=1;

				sleep(1);

				memset(rbuf,0,sizeof(rbuf));
				length=read(filep1,rbuf,300);

				if((length!=100)||(strcmp(rbuf, GZTest) != 0))
				{
					count_failed+=1;
					// printf("Send length  to  com%d is %d  data is %s \n",port1,sizeof(GZTest),GZTest);
					// printf("Read length from com%d is %d  data is %c \n",port1,length,rbuf);
				}
				
				loss=((float)count_failed/count_send)*100;
				// printf("  COM test result:Send = %6d  Failed = %6d .loss(%2.4F%)  \n",count_send,count_failed,loss);
				// printf("+----------------------------------------------------------------+\n");

			}//2次循环测试程序结束

			close(filep1);
			close(filep2);

			if(count_failed == 0)
				printf("\ntest result:OK\n");
			else
				printf("\ntest result:ERROR\n");

		}
		return 0;
	}

}





#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

FILE *run_led_fp;
FILE *alarm_led_fp;
char file_name_str[256];

#define RUNLED_ON  "0"
#define RUNLED_OFF "1"

#define ALARMLED_ON  "0"
#define ALARMLED_OFF "1"

void show_help(void)
{
    printf ("Use below param for leds: \n");
	printf ("1\t for ledrun on  \n");
	printf ("2\t for ledrun off \n");
	printf ("3\t for ledalarm on  \n");
	printf ("4\t for ledalarm off \n");
}

int ctrl_run_led(int onoff)
{
	char *usrAL="ual";
	char *runledN="ledrun";

	memset(file_name_str,0,256);

	sprintf(file_name_str,"/%s/%s/value",usrAL,runledN);

	if ((run_led_fp = fopen(file_name_str, "rb+")) == NULL)
	{
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}

	if(onoff != 0)
	{
		fprintf(run_led_fp, RUNLED_ON);
	}else
	{
		fprintf(run_led_fp, RUNLED_OFF);
	}
	fclose(run_led_fp);

	return 0;
}

int ctrl_alarm_led(int onoff)
{
	char *usrAL="ual";
	char *alarmledN="ledalarm";

	memset(file_name_str,0,256);

	sprintf(file_name_str,"/%s/%s/value",usrAL,alarmledN);

	if ((alarm_led_fp = fopen(file_name_str, "rb+")) == NULL)
	{
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}

	if(onoff != 0)
	{
		fprintf(alarm_led_fp, ALARMLED_ON);
	}else
	{
		fprintf(alarm_led_fp, ALARMLED_OFF);
	}
	fclose(alarm_led_fp);

	return 0;
}

int main(int argc, char **argv)
{
	if(argc != 2 )
	{
		show_help();
		return -1;
	}
    if ( argv[1][0] == '1' )
	{
		ctrl_run_led(1);
	}else if ( argv[1][0] == '2' )
	{
		ctrl_run_led(0);
	}else if ( argv[1][0] == '3' )
	{
		ctrl_alarm_led(1);
	}else if ( argv[1][0] == '4' )
	{
		ctrl_alarm_led(0);
	}else
	{
        show_help();
	}
	return 0;
}

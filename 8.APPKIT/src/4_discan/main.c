#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
FILE *fp;
char file_name_str[256];

#define DI_ON  0
#define DI_OFF 1

int di_prev_value[6] = {-1,-1,-1,-1,-1,-1};

int scan_di(int idx)
{
	char *usrAL="ual";
	char *diN="di";
	char buffer[10];
	int  value;
	memset(file_name_str,0,256);
	sprintf(file_name_str,"/%s/%s%d/value",usrAL,diN,idx);
	if ((fp = fopen(file_name_str, "rb")) == NULL) {
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}
	fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
	fclose(fp);
	value = atoi(buffer);
	if(value != di_prev_value[idx-1])
	{
        if(value == DI_ON)
        {
            printf("Get /ual/di%d value: %d, turn on\n", idx, value);
        }else if(value == DI_OFF)
        {
            printf("Get /ual/di%d value: %d, turn off\n", idx, value);
        }else
        {
            printf("Get /ual/di%d value: %d, error\n", idx, value);
        }
        di_prev_value[idx-1] = value;
    }
	return 0;
}

int main(int argc, char **argv)
{
    printf("Start scan di channel change...\n");
    while(1)
    {
        scan_di(1);
        usleep(100*1000);
        scan_di(2);
        usleep(100*1000);
        scan_di(3);
        usleep(100*1000);
        scan_di(4);
        usleep(100*1000);
        scan_di(5);
        usleep(100*1000);
        scan_di(6);
        usleep(100*1000);
    }
	return 0;
}

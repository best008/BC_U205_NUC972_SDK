#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
FILE *fp;
char file_name_str[256];

#define KEY_DOWN  0
#define KEY_UP    1

int key_prev_value[2] = {-1,-1};

int scan_key(char *name, int idx)
{
	char *usrAL="ual";
	char *keyN="key";
	char buffer[10];
	int  value;
	memset(file_name_str,0,256);
	if (1 == idx)
	{
		sprintf(file_name_str,"/ual/keyreset/value");
	}
	else if (2 == idx)
	{
		sprintf(file_name_str,"/ual/keyconfig/value");
	}else
	{
		return -1;
	}
	if ((fp = fopen(file_name_str, "rb")) == NULL) {
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}
	fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
	fclose(fp);
	value = atoi(buffer);
	if(value != key_prev_value[idx-1])
	{
        if(value == KEY_DOWN)
        {
            printf("Get %s value: %d, key down\n", name, value);
        }else if(value == KEY_UP)
        {
            printf("Get %s value: %d, key up\n", name, value);
        }else
        {
            printf("Get %s value: %d, error\n", name, value);
        }
        key_prev_value[idx-1] = value;
    }
	return 0;
}

int main(int argc, char **argv)
{
    printf("Start scan key status ...\n");
    while(1)
    {
        scan_key("keyrun", 1);
        usleep(100*1000);
        scan_key("keyconfig", 2);
        usleep(100*1000);
    }
	return 0;
}

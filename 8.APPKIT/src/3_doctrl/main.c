#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
FILE *fp;
char file_name_str[256];

#define DO_ON  "1"
#define DO_OFF "0"

void show_help(void)
{
    printf ("Use below param for beep: \n");
	printf ("1\t for do1 on\n");
	printf ("2\t for do1 off\n");
	printf ("3\t for do2 on\n");
	printf ("4\t for do2 off\n");

}

int ctrl_do(int idx, int onoff)
{
	char *usrAL="ual";
	char *doN="do";

	memset(file_name_str,0,256);

	sprintf(file_name_str,"/%s/%s%d/value",usrAL,doN,idx);

	if ((fp = fopen(file_name_str, "rb+")) == NULL)
	{
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}

	if(onoff != 0)
	{
		fprintf(fp, DO_ON);
	}else
	{
		fprintf(fp, DO_OFF);
	}
	fclose(fp);
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
		ctrl_do(1,1);
    }else if ( argv[1][0] == '2' )
	{
		ctrl_do(1,0);
	}else if ( argv[1][0] == '3' )
	{
		ctrl_do(2,1);
	}else if ( argv[1][0] == '4' )
	{
		ctrl_do(2,0);
	}else
	{
        show_help();
	}
	return 0;
}

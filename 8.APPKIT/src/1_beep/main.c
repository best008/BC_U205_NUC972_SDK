#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
FILE *fp;
char file_name_str[256];

#define BEEPON  "1"
#define BEEPOFF "0"

void show_help(void)
{
    printf ("Use below param for beep: \n");
	printf ("1\t for enable beep\n");
	printf ("2\t for disable beep\n");
}

int beep_enable(int enable)
{
	char *usrAL="ual";
	char *beepN="beep";

	memset(file_name_str,0,256);

	sprintf(file_name_str,"/%s/%s/value",usrAL,beepN);

	if ((fp = fopen(file_name_str, "rb+")) == NULL)
	{
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}

	if(enable != 0)
	{
		fprintf(fp, BEEPON);
	}else
	{
		fprintf(fp, BEEPOFF);
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
		beep_enable(1);
	}else if ( argv[1][0] == '2' )
	{
		beep_enable(0);
	}else
	{
        show_help();
	}
	return 0;
}

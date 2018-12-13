#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/watchdog.h>

//#define BC_U202_AM335X

#if defined (BC_U202_AM335X)
//for U202 AM335X
#define WDT_ON  "1"
#define WDT_OFF "0"

#define WDT_FEED_MAX 30

void enable_watchdog(void)
{
    FILE *wdt_en_fp; //wdt enable pin
    char file_name_str[256];

	sprintf(file_name_str,"/ual/wdt_en/value");
	if ((wdt_en_fp = fopen(file_name_str, "rb+")) == NULL)
	{
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}
	fprintf(wdt_en_fp, WDT_ON);
	fclose(wdt_en_fp);
    printf("Open watchdog enable [OK]\n");
}
int main(void)
{
    int left = WDT_FEED_MAX;
    int fcnt = 0;

    FILE *wdt_fd_fp; //wdt feed   pin

    char file_name_str[256];

	sprintf(file_name_str,"/ual/ledrun/value");

	if ((wdt_fd_fp = fopen(file_name_str, "rb+")) == NULL)
	{
		printf("Cannot open %s.\n",file_name_str);
		exit(1);
	}
	printf("Open watchdog feed pin [OK]\n");

    printf("Feed watchdog [START]\n");
    while (1) {
        if(left-- > 0)
        {
            if( 0 == fcnt)
            {
                fprintf(wdt_fd_fp, "1");
                fcnt = 1;
            }else
            {
                fprintf(wdt_fd_fp, "0");
                fcnt = 0;
            }
            fflush(wdt_fd_fp);

            if( 2 == (WDT_FEED_MAX-left))
            {
               /*must feed 1 cycle first , then enable wdt*/
               enable_watchdog();
            }
            printf("Feed watchdog count: %d [OK]\n", (WDT_FEED_MAX-left));


        }else{
            printf("Feed watchdog [STOP]\n");
            printf("System will reboot soon\n");
            fclose(wdt_fd_fp);
            sleep(10);
        }
        usleep(100000);
    }
    fclose(wdt_fd_fp);
    return 0;
}
#else
//for U205.NUC972
int main(void)
{
    int left = 10;
    int fd = open("/ual/wdt", O_RDWR);
    if (fd == -1) {
        perror("open watchdog dvice failed!");
        exit(EXIT_FAILURE);
    }
    printf("Open watchdog [OK]\n");
    printf("Feed watchdog 10 times [START]\n");
    while (1) {
        if(left-- > 0)
        {
            ioctl(fd, WDIOC_KEEPALIVE, 0);
            printf("Feed watchdog count: %d [OK]\n", (10-left));
        }else{
            printf("Feed watchdog [STOP]\n");
            printf("System will reboot soon\n");
            sleep(10);
        }
        sleep(1);
    }
    close(fd);
    return 0;
}
#endif

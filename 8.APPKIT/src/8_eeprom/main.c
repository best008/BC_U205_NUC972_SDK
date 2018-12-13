#include<sys/types.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<stdlib.h>
#define BUF_SIZE 1024

void usage(char *exe_name)
{
    printf("please input correct argument\n");
    printf("please input like this : %s -w helloworld\n",exe_name);
    printf("please input like this : %s -r 5\n",exe_name);
}

int main(int argc, char **argv)
{
    int ret = 0;
    int len = 0;

    int fd  = -1;
    int fd2 = -1;

    char arr[BUF_SIZE] = {'\0'};
    char buf[BUF_SIZE] = {'\0'};

    fd = open("/ual/eeprom", O_RDWR);
    if (fd < 0) {
        perror("open eeprom device:");
        exit(1);
    }

    fd2 = open("/sys/bus/i2c/devices/0-0050/.sysrom", O_RDWR);
    if (fd2 < 0) {
        perror("open sysrom device:");
        exit(1);
    }

    if(argc < 2) {
        usage(argv[0]);
        exit(1);
    }

    if(!strcmp(argv[1],"-w")) {
        if(argc != 3) {
            usage(argv[0]);
            exit(1);
        }
        strcpy(arr,argv[2]);

        len = strlen(arr);
        ret = write(fd, arr, len);
        if(ret != len) {
            fprintf(stderr,"error eeprom write: ret:%d\n", ret);
            exit(1);
        }
        printf("Write eeprom data:\n%s\n", arr);
        printf("Write eeprom len: %d [OK]\n", len);
    }

    if(!strcmp(argv[1],"-e")) {

        len = BUF_SIZE;
        memset(arr, 'e', BUF_SIZE);
        ret = write(fd, arr, len);
        if(ret != len) {
            fprintf(stderr,"error eeprom write: ret:%d\n", ret);
            exit(1);
        }
        printf("Write eeprom data:\n%s\n", arr);
        printf("Write eeprom len: %d [OK]\n", len);
    }


    if(!strcmp(argv[1],"-c")) {

        len = BUF_SIZE;
        memset(arr, 'c', BUF_SIZE);
        ret = write(fd2, arr, len);
        if(ret != len) {
            fprintf(stderr,"error sysrom write: ret:%d\n", ret);
            exit(1);
        }
        printf("Write sysrom data:\n%s\n", arr);
        printf("Write sysrom len: %d [OK]\n", len);
    }

    if(!strcmp(argv[1],"-r")) {
        if(argc != 3) {
            printf("please input like this : %s -r\n", argv[0]);
            exit(1);
        }
        len = atoi(argv[2]);
        if(len > BUF_SIZE)
        {
            printf("error read length: %d (max: %d)\n", len, BUF_SIZE);
            exit(1);
        }
        ret = read(fd,buf,len);
        if(ret != len) {
            fprintf(stderr,"error eeprom read: ret:%d\n",ret);
            exit(1);
        }
        printf("Read eeprom data:\n%s\n",buf);
        printf("Read eeprom len: %d [OK]\n", ret);
    }
    close(fd);
    close(fd2);
    return 0;
}

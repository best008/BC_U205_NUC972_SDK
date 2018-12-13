#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "modem.h"

void usage(char *exe_name)
{
    printf("\nexample: %s 请输入接收短息电话号码\n", exe_name);
    printf("ERR:ARGC\n");
}

FILE *fp;
char file_name_str[256];

int poff_read(void)
{
    char *usrAL="ual";
    char *poffN="poff";
    char buffer[10];
    int  value;

    memset(file_name_str,0,256);
    sprintf(file_name_str,"/%s/%s/value",usrAL,poffN);
    if ((fp = fopen(file_name_str, "rb")) == NULL) {
        printf("Cannot open %s.\n",file_name_str);
        exit(1);
    }
    fread(buffer, sizeof(char), sizeof(buffer) - 1, fp);
    fclose(fp);
    value = atoi(buffer);
    return value;
}

int power_pre_status = -1;
int power_cur_status = -1;

int main(int argc,char *argv[])
{
    int cmax = 10;

    if (argc != 2) {
        printf("input recv phonenumber:\n\n");
        usage(argv[0]);
        return -1;
    }
    modem_t *modem = modem_create();
    while(cmax > 0) {
        if (modem->connected(modem)) {
            if(cmax > 0) {
                printf("Try connect ...\n");
                cmax--;
            } else {
                printf("AT connect [ERROR]\n");
                return -1;
            }
        } else {
            printf("AT connect [OK]\n");
            break;
        }
    }
    char sca_code[32] = {0};
    if (modem->get_sca(modem, sca_code)) {
        printf("AT+CSCA action [ERROR]\n");
        return -1;
    } else {
        printf("AT+CSCA action [OK]\n");
    }

    if (modem->set_mode(modem, SMS_MODE_PDU)) {
        printf("SET PDU mode [ERROR]\n");
        return -1;
    } else {
        printf("SET PDU mode [OK]\n");
    }

    char *string = "power check: start";
    char *phone_num = argv[1];
    if (modem->send_sms(modem, phone_num, string)) {
        printf("Send SMS [ERROR]\n");
        return -1;
    } else {
        printf("Send SMS [OK]\n");
    }

    while(1) {
        power_cur_status = poff_read();
        if(power_cur_status == power_pre_status)
        {
            usleep(200*1000);
            continue;
        }
        power_pre_status = power_cur_status;
        if(1 == power_cur_status) {
            printf("Power [STOP]\n");
            char *string = "power check: down";
            char *phone_num = argv[1];

            if (modem->send_sms(modem, phone_num, string)) {
                printf("Send SMS [ERROR]\n");
                return -1;
            } else {
                printf("Send SMS [OK]\n");
            }
            break;
        } else {
            printf("Power [OK]\n");
        }
        usleep(500*1000);
    }
    modem->destroy(modem);
    return 0;
}

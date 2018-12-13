#include <stdlib.h>
#include <stdio.h>

#include "modem.h"
void usage(char *exe_name)
{
    printf("\nexample: %s 请输入接收短息电话号码\n", exe_name);
    printf("ERR:ARGC\n");
}

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
        return -1;
    }

    if (modem->set_mode(modem, SMS_MODE_PDU)) {
        return -1;
    }

    char *string = "短信测试,当前温度：+023.4度";
    char *phone_num = argv[1];
    if (modem->send_sms(modem, phone_num, string)) {
        return -1;
    }

    return 0;
}

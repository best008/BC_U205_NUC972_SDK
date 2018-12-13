#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "uart.h"

void usage(char *exe_name)
{
    printf("usage: %s uart_port_index baud_rate_index\n", exe_name);
    printf("\tuart_port_index: 1:com1, 2:com2, 3:com3, 4:com4\n");
    printf("\tbaud_rate_index: 0:2400, 1:4800, 2:9600, 3:19200, 4:38400, 5:57600, 6:115200\n");
    printf("\texample: %s 2 3, 表示使用串口2,波特率为9600\n", exe_name);
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        printf("ERR:argc\n");
        usage(argv[0]);
        return -1;
    }

    int port = atoi(argv[1]);
    if ((port < 1) || (port > 4)) {
        printf("ERR:port\n");
        return -1;
    }

    int baud_rate = atoi(argv[2]);
    if ((baud_rate < 0) || (baud_rate > 6)) {
        printf("ERR:baud\n");
        return -1;
    }

    uart_param_t uart_param;
    uart_param.device_index = port-1;
    uart_param.baud         = baud_rate;
    uart_param.bits         = UART_BITS_8;
    uart_param.stops        = UART_STOP_1;
    uart_param.parity       = UART_PARITY_NONE;

    char buf[128] = {0};
    int ret = 0;
    uart_t *uart = uart_create(&uart_param);
    if (uart != NULL) {
        uart->open(uart);
        ret=uart->read(uart, buf, 128, 10);
        if(ret >0) {
            printf("recv:%s\n",buf);
            char tmp[256]= {0};
            sprintf(tmp,"device total %d recved: %s",ret,buf);
            uart->write(uart, tmp, strlen(tmp), 10);
        }
    } else {
        printf("ERR:CREAT \n");
    }
    return 0;
}

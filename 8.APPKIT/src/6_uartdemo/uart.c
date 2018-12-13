#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <time.h>
#include <errno.h>

#include "uart.h"

static char *device_name[] = {
    "/ual/com1",
    "/ual/com2",
    "/ual/com3",
    "/ual/com4",
};

typedef struct {
    int             fd;
    uart_param_t    param;
} priv_info_t;

/**
 * [set_uart_param 串口初始化时，设置相应的参数]
 * @param priv [实例私有成员数据]
 */
static void set_uart_param(priv_info_t *priv)
{
    struct termios options;
    bzero(&options, sizeof(options));

    options.c_cflag     |= (CLOCAL | CREAD);
    options.c_lflag     = 0;    /* 非标准模式 */
    options.c_oflag     = 0;
    options.c_iflag     |= IGNPAR;

    /* 设置波特率 */
    speed_t speed;
    switch (priv->param.baud) {
    case UART_BAUD_2400:
        speed = B2400;
        break;
    case UART_BAUD_4800:
        speed = B4800;
        break;
    case UART_BAUD_9600:
        speed = B9600;
        break;
    case UART_BAUD_19200:
        speed = B19200;
        break;
    case UART_BAUD_38400:
        speed = B38400;
        break;
    case UART_BAUD_57600:
        speed = B57600;
        break;
    case UART_BAUD_115200:
        speed = B115200;
        break;
    default:
        speed = B9600;
        break;
    }
    cfsetispeed(&options, speed);
    cfsetospeed(&options, speed);

    /* 设置数据位 */
    switch (priv->param.bits) {
    case UART_BITS_5:
        options.c_cflag |= CS5;
        break;
    case UART_BITS_6:
        options.c_cflag |= CS6;
        break;
    case UART_BITS_7:
        options.c_cflag |= CS7;
        break;
    case UART_BITS_8:
    default:
        options.c_cflag |= CS8;
        break;
    }

    /* 设置校验方式 */
    switch (priv->param.parity) {
    case UART_PARITY_ODD:   /* 奇校验 */
        options.c_cflag |= (PARENB | PARODD);
        options.c_iflag |= (INPCK | ISTRIP);
        break;
    case UART_PARITY_EVEN:  /* 偶校验 */
        options.c_cflag &= ~PARODD;
        options.c_cflag |= PARENB;
        options.c_iflag |= (INPCK | ISTRIP);
        break;
    case UART_PARITY_NONE:
    default:
        options.c_cflag &= ~PARENB;
        break;
    }

    /* 设置停止位 */
    switch (priv->param.stops) {
    case UART_STOP_2:
        options.c_cflag |= CSTOPB;
        break;
    case UART_STOP_1:
    default:
        options.c_cflag &=~CSTOPB;
        break;
    }

    tcflush(priv->fd, TCIFLUSH);
    tcsetattr(priv->fd, TCSANOW, &options);
}

/**
 * [uart_open 打开串口设备文件]
 * @param  thiz [实例本身]
 * @return      [是否打开成功]
 */
static int uart_open(uart_t *thiz)
{
    priv_info_t *priv = (priv_info_t *)thiz->priv;
    char *device = device_name[priv->param.device_index];
    priv->fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY);
    if (priv->fd < 0) {
        printf("open device %s failed\n", device);
        return -1;
    }

    tcflush(priv->fd, TCIOFLUSH);

    set_uart_param(priv);

    return 0;
}

/**
 * [uart_read 读串口数据]
 * @param  thiz    [实例本身]
 * @param  buf     [读数据缓存]
 * @param  len     [期望读取数据长度]
 * @param  timeout [超时时间]
 * @return         [是否读取成功或读到的数据长度]
 */
static int uart_read(uart_t *thiz, char *buf, int len, int timeout)
{
    if (thiz == NULL) {
        return -1;
    }

    priv_info_t *priv = (priv_info_t *)thiz->priv;

    unsigned int processed = 0;
    time_t start = time(NULL);
    while (processed < len) {
        if (difftime(time(NULL), start) > timeout) {
            break;
        }

        int num = read(priv->fd, (buf + processed), (len - processed));
        if (num == -1) {
            fprintf(stderr, "Error while reading: %s\n", strerror(errno));
            break;
        } else {
            processed += num;
        }
    }

    return processed;
}

/**
 * [uart_write 往串口中写数据]
 * @param  thiz [实例本身]
 * @param  buf  [写数据缓存]
 * @param  len  [数据长度]
 * @return      [是否写入成功]
 */
static int uart_write(uart_t *thiz, const char *buf, int len, int timeout)
{
    if (thiz == NULL) {
        return -1;
    }

    priv_info_t *priv = (priv_info_t *)thiz->priv;

    unsigned int processed = 0;
    time_t start = time(NULL);
    while (processed < len) {
        if (difftime(time(NULL), start) > timeout) {
            break;
        }

        int num = write(priv->fd, (buf + processed), (len - processed));
        if (num == -1) {
            fprintf(stderr, "Error while writing: %s\n", strerror(errno));
            break;
        } else {
            processed += num;
        }
    }

    return processed;
}

/**
 * [uart_destroy 释放实例内存并关闭串口设备文件]
 * @param thiz [实例本身]
 */
static void uart_destroy(uart_t *thiz)
{
    if (thiz != NULL) {
        priv_info_t *priv = (priv_info_t *)thiz->priv;
        tcflush(priv->fd, TCIOFLUSH);
        usleep(100000);
        close(priv->fd);

        memset(thiz, 0, sizeof(uart_t) + sizeof(priv_info_t));
        free(thiz);
        thiz = NULL;
    }
}

uart_t *uart_create(uart_param_t *param)
{
    uart_t *thiz = calloc(1, sizeof(uart_t) + sizeof(priv_info_t));
    if (thiz != NULL) {
        thiz->open      = uart_open;
        thiz->read      = uart_read;
        thiz->write     = uart_write;
        thiz->destroy   = uart_destroy;

        priv_info_t *priv = (priv_info_t *)thiz->priv;
        priv->fd = -1;
        priv->param.device_index = param->device_index;
        priv->param.baud         = param->baud;
        priv->param.bits         = param->bits;
        priv->param.stops        = param->stops;
        priv->param.parity       = param->parity;
    }

    return thiz;
}

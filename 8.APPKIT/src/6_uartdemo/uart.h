#ifndef _UART_H_
#define _UART_H_

/**
 * 串口波特率枚举
 */
typedef enum
{
    UART_BAUD_2400 = 0,
    UART_BAUD_4800,
    UART_BAUD_9600,
    UART_BAUD_19200,
    UART_BAUD_38400,
    UART_BAUD_57600,
    UART_BAUD_115200,
    UART_BAUD_MAX
} uart_baud_t;

/**
 * 串口数据位枚举
 */
typedef enum
{
    UART_BITS_5 = 0,
    UART_BITS_6,
    UART_BITS_7,
    UART_BITS_8,
    UART_BITS_MAX
} uart_data_bits_t;

/**
 * 串口停止位枚举
 */
typedef enum
{
    UART_STOP_1 = 0,
    UART_STOP_2,
    UART_STOP_MAX
} uart_stop_bits_t;

/**
 * 串口校验方式枚举
 */
typedef enum
{
    UART_PARITY_NONE = 0,
    UART_PARITY_ODD,
    UART_PARITY_EVEN,
    UART_PARITY_MAX
} uart_parity_t;

/**
 * 串口设置参数
 */
typedef struct
{
    unsigned int        device_index;
    uart_baud_t         baud;
    uart_data_bits_t    bits;
    uart_parity_t       parity;
    uart_stop_bits_t    stops;
} uart_param_t;

typedef struct _uart uart_t;

typedef int (*_uart_open)(uart_t *thiz);
typedef int (*_uart_read)(uart_t *thiz, char *buf, int len, int timeout);
typedef int (*_uart_write)(uart_t *thiz, const char *buf, int len, int timeout);
typedef void (*_uart_destroy)(uart_t *thiz);

/**
 * 串口类定义
 */
struct _uart
{
    _uart_open      open;
    _uart_read      read;
    _uart_write     write;
    _uart_destroy   destroy;

    char            priv[1];
};

/**
 * [uart_create 创建一个串口实例]
 * @param  param [串口初始化参数]
 * @return       [是否创建成功]
 */
uart_t *uart_create(uart_param_t *param);

#endif

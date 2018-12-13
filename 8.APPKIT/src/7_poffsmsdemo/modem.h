#ifndef _MODEM_H_
#define _MODEM_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    SMS_MODE_PDU = 0,
    SMS_MODE_ASCII,
} sms_mode_t;

typedef struct _modem modem_t;

typedef int (*_modem_connected)(modem_t *thiz);
typedef int (*_modem_send_sms)(modem_t *thiz, char *phone_num, char *content);
typedef int (*_modem_get_sca)(modem_t *thiz, char *sca);
typedef int (*_modem_set_mode)(modem_t *thiz, sms_mode_t sms_mode);
typedef void (*_modem_destroy)(modem_t *thiz);

struct _modem
{
    _modem_connected    connected;
    _modem_send_sms     send_sms;
    _modem_get_sca      get_sca;
    _modem_set_mode     set_mode;
    _modem_destroy      destroy;

    char                priv[1];
};

modem_t *modem_create(void);

#ifdef __cplusplus
}
#endif
#endif

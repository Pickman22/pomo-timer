#ifndef POMO_H
#define POMO_H

#include <stddef.h>

#define POMO_SHORT_TIMEOUT    (5u)
#define POMO_LONG_TIMEOUT     (15u)
#define POMO_WORK_TIMEOUT     (25u)
#define POMO_NUM_SHORT_BREAKS (3u)

#define POMO_DEFAULT_CONFIG                                                 \
    (pomo_config)                                                           \
    {                                                                       \
        .short_timeout = POMO_SHORT_TIMEOUT,                                \
        .long_timeout = POMO_LONG_TIMEOUT,                                  \
        .num_short_breaks = POMO_NUM_SHORT_BREAKS, .exec_rate = (1. / 60.), \
        .work_timeout = POMO_WORK_TIMEOUT,                                  \
    }

typedef enum
{
    POMO_STATE_INIT = 0,
    POMO_STATE_WAIT,
    POMO_STATE_WORK,
    POMO_STATE_SHORT_BREAK,
    POMO_STATE_LONG_BREAK,
    POMO_STATE_PAUSE,
    POMO_STATE_NUM,
} pomo_state;

typedef enum
{
    POMO_BREAK_SHORT = 0,
    POMO_BREAK_LONG,
    POMO_BREAK_NUM,
} pomo_break;

typedef enum
{
    POMO_EVT_NONE = 0,
    POMO_EVT_WORK,
    POMO_EVT_SHORT_BREAK,
    POMO_EVT_LONG_BREAK,
    POMO_EVT_NUM
} pomo_evt;

typedef struct pomo_notif
{
    void (*notif)(const struct pomo_notif *notif);
    pomo_evt evt;
} pomo_notif;

typedef struct pomo_config
{
    unsigned int short_timeout;
    unsigned int long_timeout;
    unsigned int num_short_breaks;
    unsigned int work_timeout;
    double exec_rate;
} pomo_config;

typedef struct pomo_time
{
    unsigned int min;
    unsigned int secs;
} pomo_time;

void pomo_init(pomo_config config);
pomo_time pomo_get_time(void);
void pomo_run(void);
pomo_state pomo_get_state(void);
void pomo_deinit(void);
void pomo_pause(void);
void pomo_resume(void);
void pomo_toggle(void);
void pomo_start(void);
void pomo_long(void);
void pomo_short(void);
void pomo_work(void);
void pomo_attach(const pomo_notif *notif);

#endif

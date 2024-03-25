#include "pomo.h"
#include "ui_pomo.h"
#include <stdbool.h>
#include <stdio.h>

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr)-offsetof(type, member)))

#define MIN_TO_SECS             (60)
#define MAX_POMO_NOTIF_EVT      (8u)
#define MIN_TO_TICKS(min, rate) ((min) * MIN_TO_SECS / (rate))

typedef struct pomo_timer
{
    void (*notify)(struct pomo_timer *);
    unsigned int ticks;
    unsigned int timeout;
} pomo_timer;

typedef void (*pomo_timer_notif)(struct pomo_timer *);

typedef struct pomo_ctx
{
    pomo_config config;
    unsigned int long_timeout_ticks;
    unsigned int short_timeout_ticks;
    unsigned int work_timeout_ticks;
    unsigned int short_break_counts;
    unsigned int pomo_cycles;
    pomo_state state;
    pomo_state prev_state;
    pomo_timer timer;
} pomo_ctx;

static pomo_ctx pomo_ctx_;

static pomo_time ticks_to_pomotime(unsigned int ticks);
static void long_break(pomo_ctx *ctx);
static void short_break(pomo_ctx *ctx);
static void work(pomo_ctx *ctx);
void pause(pomo_ctx *ctx);
void resume(pomo_ctx *ctx);
void toggle(pomo_ctx *ctx);
static void timer_init(pomo_timer *tmr, unsigned int tout,
                       pomo_timer_notif notif);
static void timer_tick(pomo_timer *tmr);
static void tmr_work_tout_notif(struct pomo_timer *timer);
static void tmr_brk_tout_notif(struct pomo_timer *timer);

void pomo_init(pomo_config config)
{
    pomo_ctx_ = (pomo_ctx){.config = config};
    pomo_ctx_.long_timeout_ticks =
        MIN_TO_TICKS(config.long_timeout, config.exec_rate);
    pomo_ctx_.short_timeout_ticks =
        MIN_TO_TICKS(config.short_timeout, config.exec_rate);
    pomo_ctx_.work_timeout_ticks =
        MIN_TO_TICKS(config.work_timeout, config.exec_rate);

    /* Prepare to work. */
    pomo_ctx_.state = POMO_STATE_WAIT;
    timer_init(&pomo_ctx_.timer, pomo_ctx_.work_timeout_ticks,
               tmr_work_tout_notif);
}

void pomo_run(void)
{
    switch (pomo_ctx_.state) {
    case POMO_STATE_INIT:
        /* Init function was not called. */
        pomo_init(POMO_DEFAULT_CONFIG);
        pomo_ctx_.state = POMO_STATE_WAIT;
        break;

    case POMO_STATE_WAIT:
    case POMO_STATE_PAUSE:
        /* Do nothing during wait or pause */
        break;

    case POMO_STATE_WORK:
    case POMO_STATE_SHORT_BREAK:
    case POMO_STATE_LONG_BREAK:
        timer_tick(&pomo_ctx_.timer);
        break;

    default:
        pomo_ctx_.state = POMO_STATE_INIT;
    }
}

pomo_time pomo_get_time(void)
{
    return ticks_to_pomotime(pomo_ctx_.timer.ticks);
}

pomo_state pomo_get_state(void)
{
    return pomo_ctx_.state;
}

void pomo_set_state(pomo_state state)
{
    switch (state) {
    case POMO_STATE_WORK:
        work(&pomo_ctx_);
        break;
    case POMO_STATE_SHORT_BREAK:
        short_break(&pomo_ctx_);
        break;
    case POMO_STATE_LONG_BREAK:
        long_break(&pomo_ctx_);
        break;
    case POMO_STATE_PAUSE:
        toggle(&pomo_ctx_);
        break;

    default:
        break;
    }
}

void pause(pomo_ctx *ctx)
{
    if (ctx->state == POMO_STATE_WORK || ctx->state == POMO_STATE_SHORT_BREAK ||
        ctx->state == POMO_STATE_LONG_BREAK) {
        ctx->prev_state = ctx->state;
        ctx->state = POMO_STATE_PAUSE;
    }
}

void resume(pomo_ctx *ctx)
{
    if (ctx->state == POMO_STATE_PAUSE) {
        ctx->state = ctx->prev_state;
    }
}

void pomo_start(void)
{
    if (pomo_ctx_.state == POMO_STATE_WAIT) {
        work(&pomo_ctx_);
    }
}

void toggle(pomo_ctx *ctx)
{
    if (ctx->state != POMO_STATE_PAUSE) {
        pause(ctx);
    }
    else {
        resume(ctx);
    }
}

void pomo_deinit(void)
{
    pomo_ctx_ = (pomo_ctx){};
}

void long_break(pomo_ctx *ctx)
{
    ctx->state = POMO_STATE_LONG_BREAK;
    ctx->short_break_counts = 0u;
    timer_init(&ctx->timer, ctx->long_timeout_ticks, tmr_brk_tout_notif);
}

void short_break(pomo_ctx *ctx)
{
    ctx->state = POMO_STATE_SHORT_BREAK;
    ctx->short_break_counts++;
    timer_init(&ctx->timer, ctx->short_timeout_ticks, tmr_brk_tout_notif);
}

void work(pomo_ctx *ctx)
{
    timer_init(&ctx->timer, ctx->work_timeout_ticks, tmr_work_tout_notif);
    ctx->state = POMO_STATE_WORK;
}

static pomo_time ticks_to_pomotime(unsigned int ticks)
{
    unsigned int total_secs = ticks / UI_FPS;
    return (pomo_time){
        .secs = total_secs % 60,
        .min = total_secs / 60,
    };
}

static void timer_init(pomo_timer *tmr, unsigned int tout,
                       pomo_timer_notif notif)
{
    tmr->timeout = tout;
    tmr->ticks = tout;
    tmr->notify = notif;
}

static void timer_tick(pomo_timer *timer)
{
    if (timer->ticks-- == 0u) {
        timer->ticks = timer->timeout;
        timer->notify(timer);
    }
}

static void tmr_work_tout_notif(struct pomo_timer *timer)
{
    pomo_ctx *ctx = CONTAINER_OF(timer, pomo_ctx, timer);

    if (ctx->short_break_counts < ctx->config.num_short_breaks) {
        short_break(ctx);
    }
    else {
        long_break(ctx);
    }
}

static void tmr_brk_tout_notif(struct pomo_timer *timer)
{
    pomo_ctx *ctx = CONTAINER_OF(timer, pomo_ctx, timer);
    work(ctx);
}

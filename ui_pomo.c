#include "ui_pomo.h"
#include "pomo.h"
#include "raylib.h"
#include "ui_keys.h"
#include <stdint.h>
#include <stdio.h>

#define CONTAINER_OF(ptr, type, member) \
    ((type *)((char *)(ptr)-offsetof(type, member)))

#define UI_WINDOW_CENTER_X(w) ((UI_WIDTH - (w)) / 2)
#define UI_WINDOW_CENTER_Y(h) ((UI_HEIGHT - (h)) / 2)

typedef enum
{
    UI_POMO_EVT_START = 0,
    UI_POMO_EVT_WORK,
    UI_POMO_EVT_SHORT_BREAK,
    UI_POMO_EVT_LONG_BREAK,
    UI_POMO_EVT_PAUSE,
    UI_POMO_EVT_NUM
} ui_pomo_evt;

typedef struct ui_notif
{
    Sound sound;
    ui_pomo_evt evt;
    keys_notif notif;
} ui_notif;

typedef struct ui_ctx
{
    Sound work_bell;
    Sound long_break_bell;
    Texture2D tomato;
    Color tomato_color;
    ui_notif notif_short_break;
    ui_notif notif_long_break;
    ui_notif notif_work;
    ui_notif notif_pause;
} ui_ctx;

static void ui_begin(void);
static void ui_end(void);
static void ui_draw(void);
static void draw_minutes(void);
static void draw_status_text(void);
static void draw_pomo(void);
static void draw_help_button(void);
static void draw_help_message(void);
static void ui_pomo_evt_cbk(keys_notif *notif);
static void ui_pomo_start_cbk(keys_notif *notif);

static ui_ctx ui_ctx_;

const char *pomo_state_to_text_map[POMO_STATE_NUM] = {
    [POMO_STATE_INIT] = UI_INIT_MSG,
    [POMO_STATE_WAIT] = UI_INIT_MSG,
    [POMO_STATE_WORK] = UI_WORK_MSG,
    [POMO_STATE_SHORT_BREAK] = UI_SHORT_BREAK_MSG,
    [POMO_STATE_LONG_BREAK] = UI_LONG_BREAK_MSG,
    [POMO_STATE_PAUSE] = UI_UNKNOWN_MSG,
};

void ui_init(void)
{
    pomo_config config = POMO_DEFAULT_CONFIG;
    config.exec_rate = 1. / UI_FPS;

    InitWindow(UI_WIDTH, UI_HEIGHT, UI_TITLE);
    SetTargetFPS(UI_FPS);
    pomo_init(config);
    Image tomato_image = LoadImage("resources/tomato.png");
    InitAudioDevice();
    SetMasterVolume(0.8f);
    ui_ctx_.work_bell = LoadSound("resources/bell.wav");
    ImageResize(&tomato_image, UI_POMO_IMG_SIZE, UI_POMO_IMG_SIZE);
    ui_ctx_.tomato_color =
        GetImageColor(tomato_image, UI_POMO_IMG_SIZE / 2, UI_POMO_IMG_SIZE / 2);
    ui_ctx_.tomato = LoadTextureFromImage(tomato_image);
    UnloadImage(tomato_image);
    SetExitKey(UI_KEY_QUIT);

    ui_ctx_.notif_work = (ui_notif){
        .evt = UI_POMO_EVT_WORK,
        .sound = LoadSound("resources/bell.wav"),
        .notif = (keys_notif){.key = UI_KEY_WORK, .notify = ui_pomo_evt_cbk},
    };
    ui_ctx_.notif_pause = (ui_notif){
        .evt = UI_POMO_EVT_PAUSE,
        .sound = LoadSound("resources/pause.wav"),
        .notif =
            (keys_notif){
                .key = UI_KEY_PAUSE,
                .notify = ui_pomo_evt_cbk,
            },
    };
    ui_ctx_.notif_short_break = (ui_notif){
        .evt = UI_POMO_EVT_SHORT_BREAK,
        .sound = LoadSound("resources/bell.wav"),
        .notif =
            (keys_notif){
                .key = UI_KEY_SHORT_BREAK,
                .notify = ui_pomo_evt_cbk,
            },
    };
    ui_ctx_.notif_long_break = (ui_notif){
        .evt = UI_POMO_EVT_LONG_BREAK,
        .sound = LoadSound("resources/bell.wav"),
        .notif =
            (keys_notif){
                .key = UI_KEY_LONG_BREAK,
                .notify = ui_pomo_evt_cbk,
            },
    };
}

void ui_run(void)
{
    keys_attach(&ui_ctx_.notif_work.notif);
    keys_attach(&ui_ctx_.notif_pause.notif);
    keys_attach(&ui_ctx_.notif_short_break.notif);
    keys_attach(&ui_ctx_.notif_long_break.notif);
    keys_attach(&(keys_notif){
        .notify = ui_pomo_start_cbk,
        .key = UI_KEY_START,
    });

    while (!WindowShouldClose()) {
        ui_begin();
        keys_run();
        pomo_run();
        ui_draw();
        ui_end();
    }
}

void ui_exit(void)
{
    pomo_deinit();
    UnloadSound(ui_ctx_.notif_work.sound);
    UnloadSound(ui_ctx_.notif_long_break.sound);
    UnloadSound(ui_ctx_.notif_short_break.sound);
    UnloadSound(ui_ctx_.notif_pause.sound);
    UnloadTexture(ui_ctx_.tomato);
    CloseAudioDevice();
    CloseWindow();
}

static void ui_begin(void)
{
    BeginDrawing();
    ClearBackground(RAYWHITE);
}

static void ui_end(void)
{
    EndDrawing();
}

static void ui_draw(void)
{
    draw_minutes();
    draw_status_text();
    draw_pomo();
    draw_help_button();
}

static void draw_status_text(void)
{
    static const char *text = UI_UNKNOWN_MSG;
    pomo_state pomo_state = pomo_get_state();
    int width, pos_x, pos_y;
    if (pomo_state < POMO_STATE_NUM && pomo_state != POMO_STATE_PAUSE) {
        /* In pause state, do not update text message. */
        text = pomo_state_to_text_map[pomo_state];
    }
    width = MeasureText(text, UI_MSG_FONT_SIZE);
    pos_x = UI_WINDOW_CENTER_X(width);
    pos_y = UI_HEIGHT - UI_CLOCK_FONT_SIZE - 20 - UI_MSG_FONT_SIZE - 10;
    DrawText(text, pos_x, pos_y, UI_MSG_FONT_SIZE, UI_MSG_COLOR);
}

static void draw_minutes(void)
{
    char strbuf[64u] = {0};
    int width, pos_x, pos_y;
    pomo_time time = pomo_get_time();
    pomo_state pomo_state = pomo_get_state();
    Color text_color = ui_ctx_.tomato_color;
    if (pomo_state == POMO_STATE_PAUSE) {
        text_color = GRAY;
    }
    snprintf(strbuf, sizeof(strbuf), "%02u:%02u", time.min, time.secs);
    width = MeasureText(strbuf, UI_CLOCK_FONT_SIZE);
    pos_x = UI_WINDOW_CENTER_X(width);
    pos_y = UI_HEIGHT - UI_CLOCK_FONT_SIZE - 20;
    DrawText(strbuf, pos_x, pos_y, UI_CLOCK_FONT_SIZE, text_color);
}

static void draw_pomo(void)
{
    int pos_x = UI_WINDOW_CENTER_X(UI_POMO_IMG_SIZE);
    DrawTexture(ui_ctx_.tomato, pos_x, 40u, WHITE);
}

static void draw_help_button(void)
{
    int width = MeasureText(UI_HELP_BUTTON_MSG, UI_HELP_BUTTON_FONT_SIZE);
    int button_pos_y = UI_HELP_POPUP_PADDING;
    int button_pos_x = UI_HELP_POPUP_PADDING;
    Rectangle button = (Rectangle){
        .height = UI_HELP_BUTTON_SIZE,
        .width = UI_HELP_BUTTON_SIZE,
        .y = button_pos_y,
        .x = button_pos_x,
    };
    if (CheckCollisionPointRec(GetMousePosition(), button)) {
        DrawRectangleRounded(button, 0.3, 8, ui_ctx_.tomato_color);
        draw_help_message();
    }
    else {
        DrawRectangleRounded(button, 0.3, 8, UI_HELP_BUTTON_COLOR);
    }
    DrawText(UI_HELP_BUTTON_MSG, button_pos_x + width / 2., button_pos_y,
             UI_HELP_BUTTON_FONT_SIZE + 5, RAYWHITE);
}

static void draw_help_message(void)
{
    Rectangle poup_box = {
        .height = UI_HELP_POP_HEIGHT,
        .width = MeasureText(UI_HELP_MSG, UI_HELP_MSG_FONT_SIZE) +
                 UI_HELP_MSG_PADDING,
        .y = 2 * UI_HELP_POPUP_PADDING + UI_HELP_BUTTON_SIZE,
        .x = UI_HELP_MSG_TEXT_PADDING,
    };
    DrawRectangleRounded(poup_box, 0.05, 8, UI_HELP_POPUP_COLOR);
    DrawText(UI_HELP_MSG, poup_box.x + UI_HELP_MSG_TEXT_PADDING,
             poup_box.y + UI_HELP_MSG_TEXT_PADDING, UI_HELP_MSG_FONT_SIZE,
             UI_HELP_MSG_COLOR);
}

static void ui_pomo_start_cbk(keys_notif *notif)
{
    (void)notif;
    pomo_start();
}

static void ui_pomo_evt_cbk(keys_notif *notif)
{
    ui_notif *notif_ = CONTAINER_OF(notif, ui_notif, notif);
    PlaySound(notif_->sound);
    switch (notif_->evt) {
    case UI_POMO_EVT_WORK:
        pomo_set_state(POMO_STATE_WORK);
        break;
    case UI_POMO_EVT_PAUSE:
        pomo_set_state(POMO_STATE_PAUSE);
        break;
    case UI_POMO_EVT_SHORT_BREAK:
        pomo_set_state(POMO_STATE_SHORT_BREAK);
        break;
    case UI_POMO_EVT_LONG_BREAK:
        pomo_set_state(POMO_STATE_LONG_BREAK);
        break;
    default:
        break;
    }
}

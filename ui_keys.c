#include "ui_keys.h"
#include "raylib.h"
#include <stddef.h>
#include <stdio.h>

#define MAX_LISTENERS (64u)

typedef struct
{
    keys_notif notify;
    unsigned int flag;
} input_flag_listener;

static input_flag_listener input_listeners[MAX_LISTENERS];

unsigned int keys_get(void)
{
    unsigned int input_flags = 0u;
    input_flags |= INPUT_FLAG_START * IsKeyPressed(UI_KEY_START);
    input_flags |= INPUT_FLAG_RESTART * IsKeyPressed(UI_KEY_RESTART);
    input_flags |= INPUT_FLAG_PAUSE * IsKeyPressed(UI_KEY_PAUSE);
    input_flags |= INPUT_FLAG_RESUME * IsKeyPressed(UI_KEY_RESUME);
    input_flags |= INPUT_FLAG_SHORT_BREAK * IsKeyPressed(UI_KEY_SHORT_BREAK);
    input_flags |= INPUT_FLAG_LONG_BREAK * IsKeyPressed(UI_KEY_LONG_BREAK);
    input_flags |= INPUT_FLAG_QUIT * IsKeyPressed(UI_KEY_QUIT);
    input_flags |= INPUT_FLAG_WORK * IsKeyPressed(UI_KEY_WORK);
    return input_flags;
}

void keys_attach(keys_notif notif, unsigned int flag)
{
    for (size_t i = 0u; i < MAX_LISTENERS; i++) {
        if (input_listeners[i].notify == NULL) {
            input_listeners[i].notify = notif;
            input_listeners[i].flag = flag;
            break;
        }
    }
}

void keys_run()
{
    unsigned int input_flags = keys_get();
    for (size_t i = 0u; i < MAX_LISTENERS; i++) {
        if (input_flags & input_listeners[i].flag) {
            input_listeners[i].notify();
        }
    }
}

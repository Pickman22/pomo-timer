#include "ui_keys.h"
#include <stddef.h>
#include <stdio.h>

#define MAX_LISTENERS (64u)

typedef struct
{
    keys_notif notify;
    KeyboardKey key;
} key_listener;

static key_listener key_listeners[MAX_LISTENERS];

void keys_attach(keys_notif notif, KeyboardKey key)
{
    for (size_t i = 0u; i < MAX_LISTENERS; i++) {
        if (key_listeners[i].notify == NULL) {
            key_listeners[i].notify = notif;
            key_listeners[i].key = key;
            break;
        }
    }
}

void keys_run()
{
    // unsigned int input_flags = keys_get();
    for (size_t i = 0u; i < MAX_LISTENERS; i++) {
        if (IsKeyPressed(key_listeners[i].key)) {
            key_listeners[i].notify();
        }
    }
}

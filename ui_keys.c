#include "ui_keys.h"
#include <stddef.h>
#include <stdio.h>

#define MAX_LISTENERS (64u)

static keys_notif *key_notifiers[MAX_LISTENERS];

void keys_attach(keys_notif *notif)
{
    for (size_t i = 0u; i < MAX_LISTENERS; i++) {
        if (key_notifiers[i] == NULL) {
            key_notifiers[i] = notif;
            break;
        }
    }
}

void keys_run()
{
    for (size_t i = 0u; i < MAX_LISTENERS; i++) {
        if (key_notifiers[i] && IsKeyPressed(key_notifiers[i]->key) &&
            key_notifiers[i]->notify) {
            key_notifiers[i]->notify(key_notifiers[i]);
        }
    }
}

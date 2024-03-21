#ifndef UI_KEYS
#define UI_KEYS

#define INPUT_FLAG_START       (1u)
#define INPUT_FLAG_RESTART     (2u)
#define INPUT_FLAG_PAUSE       (4u)
#define INPUT_FLAG_SHORT_BREAK (8u)
#define INPUT_FLAG_LONG_BREAK  (16u)
#define INPUT_FLAG_RESUME      (32u)
#define INPUT_FLAG_WORK        (64u)
#define INPUT_FLAG_QUIT        (128u)

#define UI_KEY_START       (KEY_ENTER)
#define UI_KEY_PAUSE       (KEY_SPACE)
#define UI_KEY_RESUME      (KEY_SPACE)
#define UI_KEY_RESTART     (KEY_R)
#define UI_KEY_SHORT_BREAK (KEY_S)
#define UI_KEY_LONG_BREAK  (KEY_L)
#define UI_KEY_QUIT        (KEY_Q)
#define UI_KEY_WORK        (KEY_W)

typedef void (*keys_notif)(void);

unsigned int keys_get(void);
void keys_attach(keys_notif notif, unsigned int flag);
void keys_run(void);

#endif
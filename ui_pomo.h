#ifndef UI_POMO_H
#define UI_POMO_H

#define UI_WIDTH           (600)
#define UI_HEIGHT          (400)
#define UI_FPS             (60)
#define UI_CLOCK_FONT_SIZE (100)
#define UI_MSG_FONT_SIZE   (30)
#define UI_MSG_COLOR       (GRAY)
#define UI_TITLE           ("Pomotimer")
#define UI_WORK_MSG        ("Time to get to work!")
#define UI_SHORT_BREAK_MSG ("Time for a short break")
#define UI_LONG_BREAK_MSG  ("Time to relax!")
#define UI_INIT_MSG        ("Press ENTER to start.")
#define UI_UNKNOWN_MSG     ("?")
#define UI_POMO_IMG_SIZE   (170u)

void ui_init(void);

void ui_run(void);

void ui_exit(void);

#endif

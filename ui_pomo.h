#ifndef UI_POMO_H
#define UI_POMO_H

#define UI_WIDTH                 (600)
#define UI_HEIGHT                (400)
#define UI_FPS                   (60)
#define UI_CLOCK_FONT_SIZE       (100)
#define UI_MSG_FONT_SIZE         (30)
#define UI_MSG_COLOR             (GRAY)
#define UI_TITLE                 ("Pomotimer")
#define UI_WORK_MSG              ("Time to get to work!")
#define UI_SHORT_BREAK_MSG       ("Time for a short break")
#define UI_LONG_BREAK_MSG        ("Time to relax!")
#define UI_INIT_MSG              ("Press ENTER to start.")
#define UI_UNKNOWN_MSG           ("?")
#define UI_POMO_IMG_SIZE         (170u)
#define UI_HELP_MSG_PADDING      (30u)
#define UI_HELP_POPUP_COLOR      (LIGHTGRAY)
#define UI_HELP_MSG_COLOR        (RAYWHITE)
#define UI_HELP_MSG_TEXT_PADDING (10u)
#define UI_HELP_MSG_FONT_SIZE    (18)
#define UI_HELP_BUTTON_FONT_SIZE (15)
#define UI_HELP_POPUP_PADDING    (10u)
#define UI_HELP_POP_HEIGHT       (190u)
#define UI_HELP_BUTTON_SIZE      (20u)
#define UI_HELP_BUTTON_MSG       ("?")
#define UI_HELP_BUTTON_COLOR     (GRAY)
#
#define UI_HELP_MSG      \
    ("- keys -\n\n"      \
     "get to [w]ork\n\n" \
     "[l]ong break\n\n"  \
     "[s]hort break\n\n" \
     "[space] pause\n\n" \
     "[q]uit\n\n")

void ui_init(void);

void ui_run(void);

void ui_exit(void);

#endif

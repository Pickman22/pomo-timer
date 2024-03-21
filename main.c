#include "ui_pomo.h"
#include <stdio.h>

int main(void)
{
    puts("Pomodoro timer!");
    ui_init();
    ui_run();
    ui_exit();
    return 0;
}

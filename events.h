#ifndef EVENTS_H
#define EVENTS_H

#include <lvgl.h>

extern lv_obj_t * player_label;
extern int board[9];
extern bool win;
extern const int win_combinations[8][3];
extern bool firstPlayer;
extern lv_obj_t * cont;

void grid_button_event_handler(lv_event_t * e);
void event_handler_reset(lv_event_t * e);

void checkWin();

#endif
#include "HardwareSerial.h"
#include <string.h>
#include "events.h"
#include <Arduino.h> 

void checkWin(){
    
    for (int i = 0 ; i < 8; i++){
        int a = board[win_combinations[i][0]];
        int b = board[win_combinations[i][1]];
        int c = board[win_combinations[i][2]];
        if (a != 0 && a == b && b == c){
            Serial.println("Win");
            if  (a == 1){
                lv_label_set_text(player_label, "Player one wins!");
            } else {
                lv_label_set_text(player_label, "Player two wins!");
            }
            win = true;
        }    
    }
}

void event_handler_reset(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_CLICKED) {

        for (int i = 0; i < 9; i++) { 
            board[i] = 0; 
        }
        win = false;
        firstPlayer = true;
        for (int i = 0; i < 9; i++) {
            lv_obj_t * btn = lv_obj_get_child(cont, i);
            lv_obj_t * label = lv_obj_get_child(btn, 0);
            lv_label_set_text(label, "");
        }

        lv_label_set_text(player_label, "Player ones turn. X");
        
        Serial.println("Game Reset!");
    }
}
    

void grid_button_event_handler(lv_event_t * e) {
    lv_obj_t * btn = (lv_obj_t *)lv_event_get_target(e);
    lv_obj_t * label = lv_obj_get_child(btn, 0);
    uintptr_t index = (uintptr_t)lv_obj_get_user_data(btn);
    const char * txt = lv_label_get_text(label);           


    if (win){
        return;
    }
    if (board[index] != 0){
        return;
    }

    if (firstPlayer){
        lv_label_set_text(player_label, "Player two's turn. O");
        lv_label_set_text(label,"X");
        board[index] = 1;
        firstPlayer = false;
    } else{
        lv_label_set_text(player_label, "Player ones turn. X");
        lv_label_set_text(label,"O");
        board[index] = 2;
        firstPlayer = true;
    }
    checkWin();
}





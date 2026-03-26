#include <lvgl.h>
#include "events.h"
#include <TFT_eSPI.h>


#include <XPT2046_Touchscreen.h>

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

lv_obj_t * main_screen;
lv_obj_t * cont;
lv_obj_t * player_label;

bool firstPlayer = true;
bool win = false;
int board[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

const int win_combinations[8][3] = {
    {0, 1, 2}, {3, 4, 5}, {6, 7, 8}, 
    {0, 3, 6}, {1, 4, 7}, {2, 5, 8}, 
    {0, 4, 8}, {2, 4, 6}             
};


#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// If logging is enabled, it will inform the user about what is happening in the library
void log_print(lv_log_level_t level, const char * buf) {
  LV_UNUSED(level);
  Serial.println(buf);
  Serial.flush();
}

// Get the Touchscreen data
void touchscreen_read(lv_indev_t * indev, lv_indev_data_t * data) {
  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z)
  if(touchscreen.tirqTouched() && touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    data->state = LV_INDEV_STATE_PRESSED;

    // Set the coordinates
    data->point.x = x;
    data->point.y = y;

    // Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
    /* Serial.print("X = ");
    Serial.print(x);
    Serial.print(" | Y = ");
    Serial.print(y);
    Serial.print(" | Pressure = ");
    Serial.print(z);
    Serial.println();*/
  }
  else {
    data->state = LV_INDEV_STATE_RELEASED;
  }
}





void setup_grid_example() {

    static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {60, 60, 60, LV_GRID_TEMPLATE_LAST};

    cont = lv_obj_create(lv_screen_active());
    
    player_label = lv_label_create(lv_screen_active());

    lv_label_set_long_mode(player_label, LV_LABEL_LONG_WRAP);   
    lv_label_set_text(player_label, "Player ones turn. X");
    lv_obj_set_width(player_label, 150);    
    lv_obj_set_style_text_align(player_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_align(player_label, LV_ALIGN_TOP_MID, 0, 10);


    lv_obj_set_size(cont, lv_pct(95), 220); 
    lv_obj_center(cont);
    
   
    lv_obj_set_style_pad_all(cont, 5, 0); 
    lv_obj_set_style_pad_gap(cont, 5, 0); 

    lv_obj_set_style_grid_column_dsc_array(cont, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(cont, row_dsc, 0);
    lv_obj_set_layout(cont, LV_LAYOUT_GRID);

    for(uint8_t i = 0; i < 9; i++) {
        uint8_t col = i % 3;
        uint8_t row = i / 3;

        lv_obj_t * obj = lv_button_create(cont);
        lv_obj_add_event_cb(obj, grid_button_event_handler, LV_EVENT_CLICKED, NULL);
        lv_obj_set_user_data(obj, (void*)(uintptr_t)i);

        lv_obj_set_grid_cell(obj, 
                            LV_GRID_ALIGN_STRETCH, col, 1, 
                            LV_GRID_ALIGN_STRETCH, row, 1);

        lv_obj_t * label = lv_label_create(obj);
        lv_label_set_text(label, "");
        lv_obj_center(label);
    }

      lv_obj_t * btn_label;

      lv_obj_t * btn1 = lv_button_create(lv_screen_active());
      lv_obj_add_event_cb(btn1, event_handler_reset, LV_EVENT_ALL, NULL);
      lv_obj_align(btn1, LV_ALIGN_BOTTOM_MID, 0, 0);
      lv_obj_remove_flag(btn1, LV_OBJ_FLAG_PRESS_LOCK);

      btn_label = lv_label_create(btn1);
      lv_label_set_text(btn_label, "Reset");
      lv_obj_center(btn_label);
}







void setup() {
  String LVGL_Arduino = String("LVGL Library Version: ") + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.begin(115200);
  Serial.println(LVGL_Arduino);
  

  lv_init();

  lv_log_register_print_cb(log_print);

  // Start the SPI for the touchscreen and init the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);

  touchscreen.setRotation(0);


  lv_display_t * disp;

  disp = lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, LV_DISPLAY_ROTATION_0);


  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);

  lv_indev_set_read_cb(indev, touchscreen_read);


  main_screen = lv_screen_active();
  setup_grid_example();
}

void loop() {
  lv_task_handler();  
  lv_tick_inc(5);   
  delay(5);          
}
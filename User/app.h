#ifndef APP_H
#define APP_H

#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "stm32f405xx.h"
#include "lvgl.h"
#include "lv_port_disp_template.h"
#include "lv_port_indev_template.h"
#include "ui.h"
#include "slide_window.h"
#include "ui_events.h"

extern lv_chart_series_t * ui_Chart1_series_1;
extern lv_coord_t ui_Chart1_series_1_array[SLIDE_WINDOW_MAX_SIZE];
extern Slide_Window adc_window;
extern lv_disp_t * dispp;
extern lv_theme_t * theme;

extern TaskHandle_t DisplayTaskHandle;
extern TaskHandle_t Chart1RefreshTaskHandle;

void NNTask(void* param);
void Chart1RefreshTask(void* param);
void DisplayTask(void* param);
void AppInit(void);

#endif //APP_H
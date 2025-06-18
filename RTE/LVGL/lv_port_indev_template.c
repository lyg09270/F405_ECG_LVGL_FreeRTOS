/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev_template.h"
#include "stm32f405xx.h"
#include "gpio.h"
#include "tim.h"
#include <stdio.h>
/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static void keypad_init(void);
static void keypad_read(lv_indev_t * indev, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_keypad;

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;

#define NUM_KEYS 4
#define DEBOUNCE_THRESHOLD 2
uint8_t key_state[NUM_KEYS] = {0};
uint8_t key_counter[NUM_KEYS] = {0};
uint8_t key_pressed[NUM_KEYS] = {0};

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    /*------------------
     * Keypad
     * -----------------*/

    /*Initialize your keypad or keyboard if you have*/
    keypad_init();

    /*Register a keypad input device*/
    indev_keypad = lv_indev_create();
    lv_indev_set_type(indev_keypad, LV_INDEV_TYPE_KEYPAD);
    lv_indev_set_read_cb(indev_keypad, keypad_read);

    /*Later you should create group(s) with `lv_group_t * group = lv_group_create()`,
     *add objects to the group with `lv_group_add_obj(group, obj)`
     *and assign this input device to group to navigate in it:
     *`lv_indev_set_group(indev_keypad, group);`*/

}

/**********************
 *   STATIC FUNCTIONS
 **********************/
/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
	HAL_TIM_Base_Start_IT(&htim7);
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PRESSED;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_PREV;
				//act_key = LV_KEY_UP;
                break;
            case 2:
                act_key = LV_KEY_NEXT;
				//act_key = LV_KEY_DOWN;
                break;
            case 3:
                act_key = LV_KEY_ENTER;
                break;
			case 4:
                act_key = LV_KEY_ESC;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_RELEASED;
    }

    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    for(uint8_t i = 0; i < NUM_KEYS;i++)
	{
		if(key_pressed[i] == 1)
		{
			key_pressed[i] = 0;
			return i+1;
		}
	}


    return 0;
}

static void keypad_scan(void)
{
	GPIO_PinState pin_state[NUM_KEYS] = {0};
	pin_state[0] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_6);
	pin_state[1] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_7);
	pin_state[2] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_8);
	pin_state[3] = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_9);
	
    for (uint8_t i = 0; i < NUM_KEYS; i++)
    {
        if (pin_state[i] == GPIO_PIN_RESET)
        {
            if (key_counter[i] < DEBOUNCE_THRESHOLD)
            {
                key_counter[i]++;
            }
            else if (key_counter[i] >= DEBOUNCE_THRESHOLD)
            {
                if (key_state[i] == 0)
                {
                    key_state[i] = 1;
                    key_pressed[i] = 1;
                }
            }
        }
        else
        {
            key_counter[i] = 0;
            key_state[i] = 0;
			key_pressed[i] = 0;
        }
    }
}


void TIM7_IRQHandler(void)
{
        if (__HAL_TIM_GET_FLAG(&htim7, TIM_FLAG_UPDATE) != RESET)
    {
        __HAL_TIM_CLEAR_FLAG(&htim7, TIM_FLAG_UPDATE);
		keypad_scan();
	}
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif

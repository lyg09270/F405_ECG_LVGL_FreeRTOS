#ifndef __LCD_H
#define __LCD_H		
#include "stm32f405xx.h"
#include "stdlib.h"

#define USE_HORIZONTAL  	 1 //1-90 2-180 3-270

#define LCD_W 240
#define LCD_H 320

#define LCD_SET_X 0x2A
#define LCD_SET_Y 0x2B
#define LCD_WARM  0x2C
   
extern uint16_t  POINT_COLOR;
extern uint16_t  BACK_COLOR; 


#define LCD_LED(x)  ((x) ? (GPIOC->BSRR = GPIO_PIN_5) : (GPIOC->BSRR = GPIO_PIN_5 << 16))
#define LCD_CS(x)   ((x) ? (GPIOA->BSRR = GPIO_PIN_4) : (GPIOA->BSRR = GPIO_PIN_4 << 16))
#define LCD_RS(x)   ((x) ? (GPIOC->BSRR = GPIO_PIN_4) : (GPIOC->BSRR = GPIO_PIN_4 << 16))
#define LCD_RST(x)  ((x) ? (GPIOA->BSRR = GPIO_PIN_6) : (GPIOA->BSRR = GPIO_PIN_6 << 16))
 

#define	LCD_CS_SET  LCD_CS(1)
#define	LCD_RS_SET	LCD_RS(1)  
#define	LCD_RST_SET	LCD_RST(1)

 							    
#define	LCD_CS_CLR  LCD_CS(0)
#define	LCD_RS_CLR	LCD_RS(0)
#define	LCD_RST_CLR	LCD_RST(0)


void LCD_Init(void);
void LCD_SetWindows(uint16_t xStar, uint16_t yStar,uint16_t xEnd,uint16_t yEnd);
void LCD_direction(uint8_t direction);
#endif  
	 
	 




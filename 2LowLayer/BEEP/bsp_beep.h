#ifndef __bsp_BEEP_H
#define __bsp_BEEP_H	 
#include "sys.h"
#include "stdlib.h"	  
#include "delay.h"

//LED端口定义

#define RCC_ALL_BEEP     (RCC_AHB1Periph_GPIOF)

#define GPIO_PORT_BEEP    GPIOF
#define GPIO_PIN_BEEP    GPIO_Pin_6




//LED端口定义
#define BEEP PFout(6)	// 蜂鸣器控制IO 


void bsp_beep_init(void);//初始化		 		


void Sound(u16 frq);
void Sound2(u16 time);
void play_music(void);
void play_successful(void);
void play_failed(void);


#endif


















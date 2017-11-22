#include "sys.h"   
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK Mini STM32开发板
//LED驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/3/05
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//初始化PF6/PF7输出口,使能时钟		    
//LED IO初始化
void LED_Init(void)
{
	RCC->APB2ENR|=1<<7;    //使能PORTF时钟	   	 
   	 
	GPIOF->CRL&=0x00FFFFFF; 
	GPIOF->CRL|=0x33000000;//PF6/PF7 推挽输出(push-pull)   	 
  GPIOF->ODR&=~(1<<6);      //PF6=L
	GPIOF->ODR&=~(1<<7);      //PF7=L
}







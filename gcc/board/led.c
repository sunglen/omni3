#include "sys.h"   
#include "led.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ���������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//LED��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/05
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) �������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

//��ʼ��PF6/PF7�����,ʹ��ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{
	RCC->APB2ENR|=1<<7;    //ʹ��PORTFʱ��	   	 
   	 
	GPIOF->CRL&=0x00FFFFFF; 
	GPIOF->CRL|=0x33000000;//PF6/PF7 �������(push-pull)   	 
  GPIOF->ODR&=~(1<<6);      //PF6=L
	GPIOF->ODR&=~(1<<7);      //PF7=L
}






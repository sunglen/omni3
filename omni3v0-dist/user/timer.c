/* 
 * Use Timer to capture radio signal and generate PWM for LED, and etc.
 * SUN GE <sun.ge@126.com>
*/

#include "timer.h"
#include "led.h" 	  
 
//��ʱ��3�жϷ������	 
//void TIM3_IRQHandler(void)
//{ 		    		  			    
//	if(TIM3->SR&0X0001)//����ж�
//	{
//		LED1^=0x01;			    				   				     	    	
//	}				   
//	TIM3->SR&=~(1<<0);//����жϱ�־λ 	    
//}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
//����ʹ�õ��Ƕ�ʱ��3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3ʱ��ʹ��    
 	TIM3->ARR=arr;  	//�趨�������Զ���װֵ 
	TIM3->PSC=psc;  	//Ԥ��Ƶ������
	TIM3->DIER|=1<<0;   //��������ж�				
	TIM3->CR1|=0x01;    //ʹ�ܶ�ʱ��3
	MY_NVIC_Init(1,3,TIM3_IRQn,2);//��ռ1�������ȼ�3����2									 
}

//TIM3_CH2 PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_PWM_Init(u16 arr,u16 psc)
{		 					 
	RCC->APB1ENR |= (1<<1); 	//TIM3ʱ��ʹ��
	
	//TIM3_CH2 remap to PB5, partial remap.
	//CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1
	RCC->APB2ENR |= (1<<0); //enable AFIO
	AFIO->MAPR &= ~(1<<10);
	AFIO->MAPR |= (1<<11);
	
	RCC->APB2ENR|=1<<3;    //enable PORTB clock

	GPIOB->CRL&=0xFF0FFFFF;	//PB5���֮ǰ������
	GPIOB->CRL|=0x00B00000;	//���ù���������˿ڵĵڶ����ܣ�

	TIM3->ARR=arr;			//�趨�������Զ���װֵ 
	TIM3->PSC=psc;			//Ԥ��Ƶ������
  
	TIM3->CCMR1 &= ~(1<<12);
	TIM3->CCMR1|=6<<12;  	//CH2 PWM1ģʽ	
	TIM3->CCMR1|=1<<11; 		//CH2Ԥװ��ʹ��	 
 	TIM3->CCER|=1<<4;   	//OC2 ���ʹ��	     

	//TIM3->CR1=0x0080;   	//ARPEʹ�� upcounter
	TIM3->CR1|=0x01;    	//ʹ�ܶ�ʱ�� 										  
}

//Timer 3 ch3&ch4 Capture Initialize
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM3_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB1ENR |= (1<<1); 	//TIM3ʱ��ʹ��
	
	RCC->APB2ENR |= (1<<0); //enable AFIO
	//TIM3_CH3/CH4 remap to PB0/PB1, partial remap:
	//CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1
	AFIO->MAPR &= ~(1<<10);
	AFIO->MAPR |= (1<<11);
	
	RCC->APB2ENR|=1<<3;    	//ʹ��PORTBʱ��  
	 
	GPIOB->CRL&=0xFFFFFF00;	//PB0/PB1 ���֮ǰ����
	
	//GPIOB->CRL|=0x00000008;	//PB0 input with pull-up/pull-down
	//GPIOB->ODR&=~(1<<0);		//PB0 pull-down
	//GPIOB->ODR|=1<<0;		//PB0 pull-up
	
	GPIOB->CRL|=0x00000044; //PB0 PB1 floating input 
	  
 	TIM3->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM3->PSC=psc;  		//Ԥ��Ƶ�� 

	TIM3->CCMR2|=1<<0;		//CC3S=01 	ѡ������� IC3ӳ�䵽TI3��
	TIM3->CCMR2&=~(1<<1);
	
	TIM3->CCMR2&=~(1<<2); 	//IC3PSC=00 	���������Ƶ,����Ƶ
	TIM3->CCMR2&=~(1<<3);
	
 	TIM3->CCMR2&=0xff0f; 		//IC3F=0000 ���������˲��� ���˲�
	
	TIM3->CCMR2|=1<<8;		//CC4S=01 	ѡ������� IC4ӳ�䵽TI4��
	TIM3->CCMR2&=~(1<<9);
	
 	TIM3->CCMR2&=~(1<<10); 	//IC4PSC=00 	���������Ƶ,����Ƶ
	TIM3->CCMR2&=~(1<<11);
	
	TIM3->CCMR2&=0x0fff; 		//IC4F=0000 ���������˲��� ���˲�

	TIM3->CCER&=~(1<<9); 		//CC3P=0	�����ز���
	//TIM3->CCER|=1<<9; //CC3P=1 falling edge
	TIM3->CCER|=1<<8; 		//CC3E=1 	�������������ֵ������Ĵ�����

	TIM3->CCER&=~(1<<13); 		//CC4P=0	�����ز���
	//TIM3->CCER|=1<<13; //CC4P=1 falling edge
	TIM3->CCER|=1<<12; 		//CC4E=1 	�������������ֵ������Ĵ�����

	TIM3->DIER|=1<<3;   	//������3�ж�		
	TIM3->DIER|=1<<4;   	//������4�ж�
	TIM3->DIER|=1<<0;   	//��������ж�	
	TIM3->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
	MY_NVIC_Init(2,1,TIM3_IRQn,2);//��ռ2�������ȼ�0����2	   
}


u16 pluse_width_tim3_ch3=0;
u16 pluse_width_tim3_ch4=0;
u8 pluse_obtained_tim3=0; //bit0:ch3, bit1:ch4
u16 c3d_tim3;
u16 c4d_tim3;
u8 in_capture_tim3=0; //whether timer is updated in capture. bit0:ch3, bit1:ch4
u8 pluse_high_tim3=0; //mark the level, low(0) or high(1). bit0:ch3, bit1:ch4.

//��ʱ��5�жϷ������	 
void TIM3_IRQHandler(void)
{ 		    
	u16 tsr;
	
	tsr=TIM3->SR;
	
	if(tsr&0x01){ //update interrupt
		if(pluse_high_tim3&0x01)
			in_capture_tim3|=0x01;
		else
			in_capture_tim3&=~0x01;
		
		if(pluse_high_tim3&0x02)
			in_capture_tim3|=0x02;
		else
			in_capture_tim3&=~0x02;						
	}
	
	if(tsr&0x08)//ch3 capture
	{
		if(!(pluse_high_tim3 & 0x01)){ //raising edge captured
			c3d_tim3=TIM3->CCR3;
			pluse_high_tim3|=0x01;
			TIM3->CCER|=1<<9; //CC3P=1 falling edge
		}else{ //falling edge captured
			if(in_capture_tim3&0x01){
				pluse_width_tim3_ch3=(1<<16)-c3d_tim3+TIM3->CCR3;
				in_capture_tim3&=~0x01;
			}else{				
				pluse_width_tim3_ch3=TIM3->CCR3-c3d_tim3;
			}
			pluse_obtained_tim3|=0x01;
			pluse_high_tim3&=~0x01;
			TIM3->CCER&=~(1<<9); 		//CC3P=0			
		}
 	}
	
	if(tsr&0x10)//ch4 capture
	{
		if(!(pluse_high_tim3 & 0x02)){ //raising edge captured
			c4d_tim3=TIM3->CCR4;
			pluse_high_tim3|=0x02;
			TIM3->CCER|=1<<13; //CC4P=1 falling edge
		}else{ //falling edge captured
			if(in_capture_tim3&0x02){
				pluse_width_tim3_ch4=(1<<16)-c4d_tim3+TIM3->CCR4;
				in_capture_tim3&=~0x02;
			}else{				
				pluse_width_tim3_ch4=TIM3->CCR4-c4d_tim3;
			}
			pluse_obtained_tim3|=0x02;
			pluse_high_tim3&=~0x02;
			TIM3->CCER&=~(1<<13); 		//CC4P=0			
		}
 	}
	
	TIM3->SR=0;//����жϱ�־λ 	    
}


//��ʱ��5ͨ��1&2���벶��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM5_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<3;   	//TIM5 ʱ��ʹ�� 
	RCC->APB2ENR|=1<<2;    	//ʹ��PORTAʱ��  
	 
	GPIOA->CRL&=0xFFFFFF00;	//PA0/PA1 ���֮ǰ����
	
	//GPIOA->CRL|=0x00000008;	//PA0 input with pull-up/pull-down
	//GPIOA->ODR&=~(1<<0);		//PA0 pull-down
	//GPIOA->ODR|=1<<0;		//PA0 pull-up
	
	GPIOA->CRL|=0x00000044; //PA0 PA1 floating input 
	  
 	TIM5->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM5->PSC=psc;  		//Ԥ��Ƶ�� 

	TIM5->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM5->CCMR1&=~(1<<1);
	
	TIM5->CCMR1&=~(1<<2); 	//IC1PSC=00 	���������Ƶ,����Ƶ
	TIM5->CCMR1&=~(1<<3);
	
 	TIM5->CCMR1&=0xff0f; 		//IC1F=0000 ���������˲��� ���˲�
	
	TIM5->CCMR1|=1<<8;		//CC2S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM5->CCMR1&=~(1<<9);
	
 	TIM5->CCMR1&=~(1<<10); 	//IC2PSC=00 	���������Ƶ,����Ƶ
	TIM5->CCMR1&=~(1<<11);
	
	TIM5->CCMR1&=0x0fff; 		//IC2F=0000 ���������˲��� ���˲�

	TIM5->CCER&=~(1<<1); 		//CC1P=0	�����ز���
	//TIM5->CCER|=1<<1; //CC1P=1 falling edge
	TIM5->CCER|=1<<0; 		//CC1E=1 	�������������ֵ������Ĵ�����

	TIM5->CCER&=~(1<<5); 		//CC2P=0	�����ز���
	//TIM5->CCER|=1<<5; //CC2P=1 falling edge
	TIM5->CCER|=1<<4; 		//CC2E=1 	�������������ֵ������Ĵ�����

	TIM5->DIER|=1<<1;   	//������1�ж�		
	TIM5->DIER|=1<<2;   	//������2�ж�
	TIM5->DIER|=1<<0;   	//��������ж�	
	TIM5->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//��ռ2�������ȼ�0����2	   
}


u16 pluse_width_tim5_ch1=0;
u16 pluse_width_tim5_ch2=0;
u8 pluse_obtained_tim5=0; //bit0:ch1, bit1:ch2
u16 c1d_tim5;
u16 c2d_tim5;
u8 in_capture_tim5=0; //whether timer is updated in capture. bit0:ch1, bit1:ch2
u8 pluse_high_tim5=0; //mark the level, low(0) or high(1). bit0:ch1, bit1:ch2.

//��ʱ��5�жϷ������	 
void TIM5_IRQHandler(void)
{ 		    
	u16 tsr;
	
	tsr=TIM5->SR;
	
	if(tsr&0x01){ //update interrupt
		if(pluse_high_tim5&0x01)
			in_capture_tim5|=0x01;
		else
			in_capture_tim5&=~0x01;
		
		if(pluse_high_tim5&0x02)
			in_capture_tim5|=0x02;
		else
			in_capture_tim5&=~0x02;						
	}
	
//	if(tsr&0x02)//����1���������¼�
//	{
//		if(!(pluse_high & 0x01)){ //raising edge captured
//			pluse_width_ch1=0;
//			TIM5->CNT=0;
//			pluse_high|=0x01;
//			TIM5->CCER|=1<<1; //CC1P=1 falling edge
//		}else{ //falling edge captured
//			pluse_width_ch1=TIM5->CCR1;
//			pluse_obtained|=0x01;
//			pluse_high&=~0x01;
//			TIM5->CCER&=~(1<<1);			
//		}
// 	}
	
	if(tsr&0x02)//ch1 capture
	{
		if(!(pluse_high_tim5 & 0x01)){ //raising edge captured
			c1d_tim5=TIM5->CCR1;
			pluse_high_tim5|=0x01;
			TIM5->CCER|=1<<1; //CC1P=1 falling edge
		}else{ //falling edge captured
			if(in_capture_tim5&0x01){
				pluse_width_tim5_ch1=(1<<16)-c1d_tim5+TIM5->CCR1;
				in_capture_tim5&=~0x01;
			}else{				
				pluse_width_tim5_ch1=TIM5->CCR1-c1d_tim5;
			}
			pluse_obtained_tim5|=0x01;
			pluse_high_tim5&=~0x01;
			TIM5->CCER&=~(1<<1);			
		}
 	}
	
	if(tsr&0x04)//ch2 capture
	{
		if(!(pluse_high_tim5 & 0x02)){ //raising edge captured
			c2d_tim5=TIM5->CCR2;
			pluse_high_tim5|=0x02;
			TIM5->CCER|=1<<5; //CC2P=1 falling edge
		}else{ //falling edge captured
			if(in_capture_tim5&0x02){
				pluse_width_tim5_ch2=(1<<16)-c2d_tim5+TIM5->CCR2;
				in_capture_tim5&=~0x02;
			}else{				
				pluse_width_tim5_ch2=TIM5->CCR2-c2d_tim5;
			}
			pluse_obtained_tim5|=0x02;
			pluse_high_tim5&=~0x02;
			TIM5->CCER&=~(1<<5);			
		}
 	}
	
	TIM5->SR=0;//����жϱ�־λ 	    
}


//TIM1_CH1 PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO������
	RCC->APB2ENR|=1<<11; 	//TIM1ʱ��ʹ��    
	RCC->APB2ENR|=1<<2;    	//ʹ��PORTAʱ��  
	GPIOA->CRH&=0xFFFFFFF0;	//PA8���֮ǰ������
	GPIOA->CRH|=0x0000000B;	//���ù���������˿ڵĵڶ����ܣ�
	
	TIM1->ARR=arr;			//�趨�������Զ���װֵ 
	TIM1->PSC=psc;			//Ԥ��Ƶ������
  
	//TIM1->CCMR1|=7<<4;  	//CH1 PWM2ģʽ
	TIM1->CCMR1|=6<<4;  	//CH1 PWM1ģʽ
	TIM1->CCMR1&=~(1<<4);
	
	TIM1->CCMR1|=1<<3; 		//CH1Ԥװ��ʹ��	 
 	TIM1->CCER|=1<<0;   	//OC1 ���ʹ��	   
	TIM1->BDTR|=1<<15;   	//MOE �����ʹ��	   

	TIM1->CR1=0x0080;   	//ARPEʹ�� 
	TIM1->CR1|=0x01;    	//ʹ�ܶ�ʱ��1 										  
}  

//TIM8_CH1 PWM�����ʼ��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM8_PWM_Init(u16 arr,u16 psc)
{		 					 
	//�˲������ֶ��޸�IO�����ã� default is PC6
	RCC->APB2ENR|=1<<13; 	//TIM8ʱ��ʹ��    
	GPIOC->CRL&=0xF0FFFFFF;	//PC6���֮ǰ������
	GPIOC->CRL|=0x0B000000;	//���ù���������˿ڵĵڶ����ܣ�
	
	TIM8->ARR=arr;			//�趨�������Զ���װֵ 
	TIM8->PSC=psc;			//Ԥ��Ƶ������
  
	//TIM8->CCMR1|=7<<4;  	//CH1 PWM2ģʽ
	TIM8->CCMR1|=6<<4;  	//CH1 PWM1ģʽ	
	TIM8->CCMR1|=1<<3; 		//CH1Ԥװ��ʹ��	 
 	TIM8->CCER|=1<<0;   	//OC1 ���ʹ��	   
	TIM8->BDTR|=1<<15;   	//MOE �����ʹ��	   

	TIM8->CR1=0x0080;   	//ARPEʹ�� 
	TIM8->CR1|=0x01;    	//ʹ�ܶ�ʱ��1 										  
}

//��ʱ��8ͨ��1&2���벶��
//arr���Զ���װֵ
//psc��ʱ��Ԥ��Ƶ��
void TIM8_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB2ENR|=1<<13;   	//TIM8 ʱ��ʹ�� 
	RCC->APB2ENR|=1<<4;    	//ʹ��PORTCʱ��  
	 
	GPIOC->CRL&=0x00FFFFFF;	//PC6/PC7 ���֮ǰ����
	
	//GPIOC->CRL|=0x08000000;	//PC6 input with pull-up/pull-down
	//GPIOC->ODR&=~(1<<6);		//PC6 pull-down
	//GPIOC->ODR|=1<<6;		//PC6 pull-up
	
	GPIOC->CRL|=0x44000000; //PC6 PC7 floating input 
	  
 	TIM8->ARR=arr;  		//�趨�������Զ���װֵ   
	TIM8->PSC=psc;  		//Ԥ��Ƶ�� 

	TIM8->CCMR1|=1<<0;		//CC1S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM8->CCMR1&=~(1<<1);
	
	TIM8->CCMR1&=~(1<<2); 	//IC1PSC=00 	���������Ƶ,����Ƶ
	TIM8->CCMR1&=~(1<<3);
	
 	TIM8->CCMR1&=0xff0f; 		//IC1F=0000 ���������˲��� ���˲�
	
	TIM8->CCMR1|=1<<8;		//CC2S=01 	ѡ������� IC1ӳ�䵽TI1��
	TIM8->CCMR1&=~(1<<9);
	
 	TIM8->CCMR1&=~(1<<10); 	//IC2PSC=00 	���������Ƶ,����Ƶ
	TIM8->CCMR1&=~(1<<11);
	
	TIM8->CCMR1&=0x0fff; 		//IC2F=0000 ���������˲��� ���˲�

	TIM8->CCER&=~(1<<1); 		//CC1P=0	�����ز���
	//TIM8->CCER|=1<<1; //CC1P=1 falling edge
	TIM8->CCER|=1<<0; 		//CC1E=1 	�������������ֵ������Ĵ�����

	TIM8->CCER&=~(1<<5); 		//CC2P=0	�����ز���
	//TIM8->CCER|=1<<5; //CC2P=1 falling edge
	TIM8->CCER|=1<<4; 		//CC2E=1 	�������������ֵ������Ĵ�����

	TIM8->DIER|=1<<1;   	//������1�ж�		
	TIM8->DIER|=1<<2;   	//������2�ж�
	TIM8->DIER|=1<<0;   	//��������ж�	
	TIM8->CR1|=0x01;    	//ʹ�ܶ�ʱ��2
	MY_NVIC_Init(1,2,TIM8_UP_IRQn,1);//
	MY_NVIC_Init(1,1,TIM8_CC_IRQn,1);//		
}


u16 pluse_width_tim8_ch1=0;
u16 pluse_width_tim8_ch2=0;
u8 pluse_obtained_tim8=0; //bit0:ch1, bit1:ch2
u16 c1d_tim8;
u16 c2d_tim8;
u8 in_capture_tim8=0; //whether timer is updated in capture. bit0:ch1, bit1:ch2
u8 pluse_high_tim8=0; //mark the level, low(0) or high(1). bit0:ch1, bit1:ch2.

//��ʱ��8�жϷ������	 
void TIM8_UP_IRQHandler(void)
{ 		    
	u16 tsr;
	
	tsr=TIM8->SR;
	
	if(tsr&0x01){ //update interrupt
		if(pluse_high_tim8&0x01)
			in_capture_tim8|=0x01;
		else
			in_capture_tim8&=~0x01;
		
		if(pluse_high_tim8&0x02)
			in_capture_tim8|=0x02;
		else
			in_capture_tim8&=~0x02;						
	}
	
	TIM8->SR&=~(0x01);
}
	

void TIM8_CC_IRQHandler(void)
{
	u16 tsr;
	
	tsr=TIM8->SR;
	
	if(tsr&0x02)//ch1 capture
	{
		if(!(pluse_high_tim8 & 0x01)){ //raising edge captured
			c1d_tim8=TIM8->CCR1;
			pluse_high_tim8|=0x01;
			TIM8->CCER|=1<<1; //CC1P=1 falling edge
		}else{ //falling edge captured
			if(in_capture_tim8&0x01){
				pluse_width_tim8_ch1=(1<<16)-c1d_tim8+TIM8->CCR1;
				in_capture_tim8&=~0x01;
			}else{				
				pluse_width_tim8_ch1=TIM8->CCR1-c1d_tim8;
			}
			pluse_obtained_tim8|=0x01;
			pluse_high_tim8&=~0x01;
			TIM8->CCER&=~(1<<1);			
		}
		TIM8->SR&=~(0x02);
		
 	}
	
	if(tsr&0x04)//ch2 capture
	{
		if(!(pluse_high_tim8 & 0x02)){ //raising edge captured
			c2d_tim8=TIM8->CCR2;
			pluse_high_tim8|=0x02;
			TIM8->CCER|=1<<5; //CC2P=1 falling edge
		}else{ //falling edge captured
			if(in_capture_tim8&0x02){
				pluse_width_tim8_ch2=(1<<16)-c2d_tim8+TIM8->CCR2;
				in_capture_tim8&=~0x02;
			}else{				
				pluse_width_tim8_ch2=TIM8->CCR2-c2d_tim8;
			}
			pluse_obtained_tim8|=0x02;
			pluse_high_tim8&=~0x02;
			TIM8->CCER&=~(1<<5);			
		}
		
		TIM8->SR&=~(0x04);
 	}
		    
}

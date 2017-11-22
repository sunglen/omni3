/* 
 * Use Timer to capture radio signal and generate PWM for LED, and etc.
 * SUN GE <sun.ge@126.com>
*/

#include "timer.h"
#include "led.h" 	  
 
//定时器3中断服务程序	 
//void TIM3_IRQHandler(void)
//{ 		    		  			    
//	if(TIM3->SR&0X0001)//溢出中断
//	{
//		LED1^=0x01;			    				   				     	    	
//	}				   
//	TIM3->SR&=~(1<<0);//清除中断标志位 	    
//}

//通用定时器中断初始化
//这里时钟选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器3!
void TIM3_Int_Init(u16 arr,u16 psc)
{
	RCC->APB1ENR|=1<<1;	//TIM3时钟使能    
 	TIM3->ARR=arr;  	//设定计数器自动重装值 
	TIM3->PSC=psc;  	//预分频器设置
	TIM3->DIER|=1<<0;   //允许更新中断				
	TIM3->CR1|=0x01;    //使能定时器3
	MY_NVIC_Init(1,3,TIM3_IRQn,2);//抢占1，子优先级3，组2									 
}

//TIM3_CH2 PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM3_PWM_Init(u16 arr,u16 psc)
{		 					 
	RCC->APB1ENR |= (1<<1); 	//TIM3时钟使能
	
	//TIM3_CH2 remap to PB5, partial remap.
	//CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1
	RCC->APB2ENR |= (1<<0); //enable AFIO
	AFIO->MAPR &= ~(1<<10);
	AFIO->MAPR |= (1<<11);
	
	RCC->APB2ENR|=1<<3;    //enable PORTB clock

	GPIOB->CRL&=0xFF0FFFFF;	//PB5清除之前的设置
	GPIOB->CRL|=0x00B00000;	//复用功能输出（端口的第二功能）

	TIM3->ARR=arr;			//设定计数器自动重装值 
	TIM3->PSC=psc;			//预分频器设置
  
	TIM3->CCMR1 &= ~(1<<12);
	TIM3->CCMR1|=6<<12;  	//CH2 PWM1模式	
	TIM3->CCMR1|=1<<11; 		//CH2预装载使能	 
 	TIM3->CCER|=1<<4;   	//OC2 输出使能	     

	//TIM3->CR1=0x0080;   	//ARPE使能 upcounter
	TIM3->CR1|=0x01;    	//使能定时器 										  
}

//Timer 3 ch3&ch4 Capture Initialize
//arr：自动重装值
//psc：时钟预分频数
void TIM3_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB1ENR |= (1<<1); 	//TIM3时钟使能
	
	RCC->APB2ENR |= (1<<0); //enable AFIO
	//TIM3_CH3/CH4 remap to PB0/PB1, partial remap:
	//CH1/PB4, CH2/PB5, CH3/PB0, CH4/PB1
	AFIO->MAPR &= ~(1<<10);
	AFIO->MAPR |= (1<<11);
	
	RCC->APB2ENR|=1<<3;    	//使能PORTB时钟  
	 
	GPIOB->CRL&=0xFFFFFF00;	//PB0/PB1 清除之前设置
	
	//GPIOB->CRL|=0x00000008;	//PB0 input with pull-up/pull-down
	//GPIOB->ODR&=~(1<<0);		//PB0 pull-down
	//GPIOB->ODR|=1<<0;		//PB0 pull-up
	
	GPIOB->CRL|=0x00000044; //PB0 PB1 floating input 
	  
 	TIM3->ARR=arr;  		//设定计数器自动重装值   
	TIM3->PSC=psc;  		//预分频器 

	TIM3->CCMR2|=1<<0;		//CC3S=01 	选择输入端 IC3映射到TI3上
	TIM3->CCMR2&=~(1<<1);
	
	TIM3->CCMR2&=~(1<<2); 	//IC3PSC=00 	配置输入分频,不分频
	TIM3->CCMR2&=~(1<<3);
	
 	TIM3->CCMR2&=0xff0f; 		//IC3F=0000 配置输入滤波器 不滤波
	
	TIM3->CCMR2|=1<<8;		//CC4S=01 	选择输入端 IC4映射到TI4上
	TIM3->CCMR2&=~(1<<9);
	
 	TIM3->CCMR2&=~(1<<10); 	//IC4PSC=00 	配置输入分频,不分频
	TIM3->CCMR2&=~(1<<11);
	
	TIM3->CCMR2&=0x0fff; 		//IC4F=0000 配置输入滤波器 不滤波

	TIM3->CCER&=~(1<<9); 		//CC3P=0	上升沿捕获
	//TIM3->CCER|=1<<9; //CC3P=1 falling edge
	TIM3->CCER|=1<<8; 		//CC3E=1 	允许捕获计数器的值到捕获寄存器中

	TIM3->CCER&=~(1<<13); 		//CC4P=0	上升沿捕获
	//TIM3->CCER|=1<<13; //CC4P=1 falling edge
	TIM3->CCER|=1<<12; 		//CC4E=1 	允许捕获计数器的值到捕获寄存器中

	TIM3->DIER|=1<<3;   	//允许捕获3中断		
	TIM3->DIER|=1<<4;   	//允许捕获4中断
	TIM3->DIER|=1<<0;   	//允许更新中断	
	TIM3->CR1|=0x01;    	//使能定时器2
	MY_NVIC_Init(2,1,TIM3_IRQn,2);//抢占2，子优先级0，组2	   
}


u16 pluse_width_tim3_ch3=0;
u16 pluse_width_tim3_ch4=0;
u8 pluse_obtained_tim3=0; //bit0:ch3, bit1:ch4
u16 c3d_tim3;
u16 c4d_tim3;
u8 in_capture_tim3=0; //whether timer is updated in capture. bit0:ch3, bit1:ch4
u8 pluse_high_tim3=0; //mark the level, low(0) or high(1). bit0:ch3, bit1:ch4.

//定时器5中断服务程序	 
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
	
	TIM3->SR=0;//清除中断标志位 	    
}


//定时器5通道1&2输入捕获
//arr：自动重装值
//psc：时钟预分频数
void TIM5_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB1ENR|=1<<3;   	//TIM5 时钟使能 
	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟  
	 
	GPIOA->CRL&=0xFFFFFF00;	//PA0/PA1 清除之前设置
	
	//GPIOA->CRL|=0x00000008;	//PA0 input with pull-up/pull-down
	//GPIOA->ODR&=~(1<<0);		//PA0 pull-down
	//GPIOA->ODR|=1<<0;		//PA0 pull-up
	
	GPIOA->CRL|=0x00000044; //PA0 PA1 floating input 
	  
 	TIM5->ARR=arr;  		//设定计数器自动重装值   
	TIM5->PSC=psc;  		//预分频器 

	TIM5->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
	TIM5->CCMR1&=~(1<<1);
	
	TIM5->CCMR1&=~(1<<2); 	//IC1PSC=00 	配置输入分频,不分频
	TIM5->CCMR1&=~(1<<3);
	
 	TIM5->CCMR1&=0xff0f; 		//IC1F=0000 配置输入滤波器 不滤波
	
	TIM5->CCMR1|=1<<8;		//CC2S=01 	选择输入端 IC1映射到TI1上
	TIM5->CCMR1&=~(1<<9);
	
 	TIM5->CCMR1&=~(1<<10); 	//IC2PSC=00 	配置输入分频,不分频
	TIM5->CCMR1&=~(1<<11);
	
	TIM5->CCMR1&=0x0fff; 		//IC2F=0000 配置输入滤波器 不滤波

	TIM5->CCER&=~(1<<1); 		//CC1P=0	上升沿捕获
	//TIM5->CCER|=1<<1; //CC1P=1 falling edge
	TIM5->CCER|=1<<0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中

	TIM5->CCER&=~(1<<5); 		//CC2P=0	上升沿捕获
	//TIM5->CCER|=1<<5; //CC2P=1 falling edge
	TIM5->CCER|=1<<4; 		//CC2E=1 	允许捕获计数器的值到捕获寄存器中

	TIM5->DIER|=1<<1;   	//允许捕获1中断		
	TIM5->DIER|=1<<2;   	//允许捕获2中断
	TIM5->DIER|=1<<0;   	//允许更新中断	
	TIM5->CR1|=0x01;    	//使能定时器2
	MY_NVIC_Init(2,0,TIM5_IRQn,2);//抢占2，子优先级0，组2	   
}


u16 pluse_width_tim5_ch1=0;
u16 pluse_width_tim5_ch2=0;
u8 pluse_obtained_tim5=0; //bit0:ch1, bit1:ch2
u16 c1d_tim5;
u16 c2d_tim5;
u8 in_capture_tim5=0; //whether timer is updated in capture. bit0:ch1, bit1:ch2
u8 pluse_high_tim5=0; //mark the level, low(0) or high(1). bit0:ch1, bit1:ch2.

//定时器5中断服务程序	 
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
	
//	if(tsr&0x02)//捕获1发生捕获事件
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
	
	TIM5->SR=0;//清除中断标志位 	    
}


//TIM1_CH1 PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM1_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置
	RCC->APB2ENR|=1<<11; 	//TIM1时钟使能    
	RCC->APB2ENR|=1<<2;    	//使能PORTA时钟  
	GPIOA->CRH&=0xFFFFFFF0;	//PA8清除之前的设置
	GPIOA->CRH|=0x0000000B;	//复用功能输出（端口的第二功能）
	
	TIM1->ARR=arr;			//设定计数器自动重装值 
	TIM1->PSC=psc;			//预分频器设置
  
	//TIM1->CCMR1|=7<<4;  	//CH1 PWM2模式
	TIM1->CCMR1|=6<<4;  	//CH1 PWM1模式
	TIM1->CCMR1&=~(1<<4);
	
	TIM1->CCMR1|=1<<3; 		//CH1预装载使能	 
 	TIM1->CCER|=1<<0;   	//OC1 输出使能	   
	TIM1->BDTR|=1<<15;   	//MOE 主输出使能	   

	TIM1->CR1=0x0080;   	//ARPE使能 
	TIM1->CR1|=0x01;    	//使能定时器1 										  
}  

//TIM8_CH1 PWM输出初始化
//arr：自动重装值
//psc：时钟预分频数
void TIM8_PWM_Init(u16 arr,u16 psc)
{		 					 
	//此部分需手动修改IO口设置， default is PC6
	RCC->APB2ENR|=1<<13; 	//TIM8时钟使能    
	GPIOC->CRL&=0xF0FFFFFF;	//PC6清除之前的设置
	GPIOC->CRL|=0x0B000000;	//复用功能输出（端口的第二功能）
	
	TIM8->ARR=arr;			//设定计数器自动重装值 
	TIM8->PSC=psc;			//预分频器设置
  
	//TIM8->CCMR1|=7<<4;  	//CH1 PWM2模式
	TIM8->CCMR1|=6<<4;  	//CH1 PWM1模式	
	TIM8->CCMR1|=1<<3; 		//CH1预装载使能	 
 	TIM8->CCER|=1<<0;   	//OC1 输出使能	   
	TIM8->BDTR|=1<<15;   	//MOE 主输出使能	   

	TIM8->CR1=0x0080;   	//ARPE使能 
	TIM8->CR1|=0x01;    	//使能定时器1 										  
}

//定时器8通道1&2输入捕获
//arr：自动重装值
//psc：时钟预分频数
void TIM8_Cap_Init(u16 arr,u16 psc)
{		 
	RCC->APB2ENR|=1<<13;   	//TIM8 时钟使能 
	RCC->APB2ENR|=1<<4;    	//使能PORTC时钟  
	 
	GPIOC->CRL&=0x00FFFFFF;	//PC6/PC7 清除之前设置
	
	//GPIOC->CRL|=0x08000000;	//PC6 input with pull-up/pull-down
	//GPIOC->ODR&=~(1<<6);		//PC6 pull-down
	//GPIOC->ODR|=1<<6;		//PC6 pull-up
	
	GPIOC->CRL|=0x44000000; //PC6 PC7 floating input 
	  
 	TIM8->ARR=arr;  		//设定计数器自动重装值   
	TIM8->PSC=psc;  		//预分频器 

	TIM8->CCMR1|=1<<0;		//CC1S=01 	选择输入端 IC1映射到TI1上
	TIM8->CCMR1&=~(1<<1);
	
	TIM8->CCMR1&=~(1<<2); 	//IC1PSC=00 	配置输入分频,不分频
	TIM8->CCMR1&=~(1<<3);
	
 	TIM8->CCMR1&=0xff0f; 		//IC1F=0000 配置输入滤波器 不滤波
	
	TIM8->CCMR1|=1<<8;		//CC2S=01 	选择输入端 IC1映射到TI1上
	TIM8->CCMR1&=~(1<<9);
	
 	TIM8->CCMR1&=~(1<<10); 	//IC2PSC=00 	配置输入分频,不分频
	TIM8->CCMR1&=~(1<<11);
	
	TIM8->CCMR1&=0x0fff; 		//IC2F=0000 配置输入滤波器 不滤波

	TIM8->CCER&=~(1<<1); 		//CC1P=0	上升沿捕获
	//TIM8->CCER|=1<<1; //CC1P=1 falling edge
	TIM8->CCER|=1<<0; 		//CC1E=1 	允许捕获计数器的值到捕获寄存器中

	TIM8->CCER&=~(1<<5); 		//CC2P=0	上升沿捕获
	//TIM8->CCER|=1<<5; //CC2P=1 falling edge
	TIM8->CCER|=1<<4; 		//CC2E=1 	允许捕获计数器的值到捕获寄存器中

	TIM8->DIER|=1<<1;   	//允许捕获1中断		
	TIM8->DIER|=1<<2;   	//允许捕获2中断
	TIM8->DIER|=1<<0;   	//允许更新中断	
	TIM8->CR1|=0x01;    	//使能定时器2
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

//定时器8中断服务程序	 
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

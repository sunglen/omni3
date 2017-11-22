/*
 * Universal control system of multiple motors
 * with motor controllers of CAN I/F and NT-SHELL UI.
 * SUN GE <sun.ge@126.com>
*/

#include "sys.h"
#include "usart.h"		
#include "delay.h"	
#include "led.h"   
#include "timer.h"
#include "sram.h"
#include "can.h"
#include "math.h"


//test program 1: toggle PF7 by ISR and toggle PF6 in main loop. 
//test OK. 20150611 14:00
//int main(void)
//{			
// 	Stm32_Clock_Init(9); //系统时钟设置
//	delay_init(72);	     //延时初始化
//	uart_init(72,9600);  //串口初始化
//	LED_Init();		  	 //初始化与LED连接的硬件接口
//	TIM3_Int_Init(5000,7199);//10Khz的计数频率，计数到5000为500ms
//  while(1)
//	{
//		LED0^=0x01;
//		delay_ms(200);		   
//	}
//}

//test program 2: drive PB5 by TIM3(CH2, remap) with pwm.
//PB5 is connected to an LED.
//int main(void)
//{			
// 	u16 ccvalue=0;
//	//u16 ccvalue=450;	
//	u8 dir=1;	
//	
//	Stm32_Clock_Init(9); //系统时钟设置
//	delay_init(72);	     //延时初始化
//	
//	uart_init(72,9600);  //串口初始化
//	LED_Init();		  	 //初始化与LED连接的硬件接口
//	
//	RCC->APB2ENR|=1<<3;    //使能PORTB时钟 this is a must
//	TIM3_PWM_Init(899,0);//不分频。PWM频率=72000/(899+1)=80Khz
//	
//	while(1)
//	{
// 		delay_ms(5);	 
//		if(dir)ccvalue++;
//		else ccvalue--;	 
// 		if(ccvalue>=899)dir=0;
//		//if(ccvalue>=899)dir=0;
//		if(ccvalue==0)dir=1;	   					 
//		//if(ccvalue==450)dir=1;	  
//		TIM3->CCR2=ccvalue;	   
//	} 
//}

//test program 3: usart1 test
//int main(void)
//{			
//	u8 t;
//	//u8 len;	
//	//u16 times=0;  
//	Stm32_Clock_Init(9); //系统时钟设置
//	delay_init(72);	     //延时初始化
//	uart1_init(72,9600);	 //串口初始化为9600
//	LED_Init();		  	 //初始化与LED连接的硬件接口
//	while(1)
//	{
//		/*if(USART_RX_STA&0x8000)
//		{					   
//			len=USART_RX_STA&0x3fff;//得到此次接收到的数据长度
//			printf("\r\n您发送的消息为:\r\n");
//			for(t=0;t<len;t++)
//			{
//				USART1->DR=USART_RX_BUF[t];
//				while((USART1->SR&0X40)==0);//等待发送结束
//			}
//			printf("\r\n\r\n");//插入换行
//			USART_RX_STA=0;
//		}else
//		{
//			times++;
//			if(times%5000==0)
//			{
//				printf("\r\nALIENTEK MiniSTM32开发板 串口实验\r\n");
//				printf("正点原子@ALIENTEK\r\n\r\n\r\n");
//			}
//			if(times%200==0)printf("请输入数据,以回车键结束\r\n");
//			if(times%30==0)LED0=!LED0;//闪烁LED,提示系统正在运行.
//			delay_ms(10);   
//		}*/
//		LED0^=0x01;
//		xprintf("t:%d\n",t);
//		delay_ms(500);
//		t++;
//	}
//}

//test program 4: sram test
//int main(void)
//{				 
// 	u8 i=0;
//	u8 d;

// 	Stm32_Clock_Init(9);		//系统时钟设置
//	uart_init(72,115200);		//串口初始化为115200
//	delay_init(72);	   	 		//延时初始化
//	
// 	LED_Init();		  			//初始化与LED连接的硬件接口
// 	FSMC_SRAM_Init();			//初始化外部SRAM

//	while(1)
//	{	
//		fsmc_sram_test_write(0xfffff, i);
//		d=fsmc_sram_test_read(0xfffff);
//		if(d==i)//DS0闪烁.
//		{
//			//i=0;
//			LED0^=0x01;
// 		}
//		delay_ms(500);
//		i++;
//	}	      
//}

//test program 5: usart2 test
//int main(void)
//{			
//	u8 t;
//  u8 rx_buf[256];
//	
//	Stm32_Clock_Init(9); //系统时钟设置
//	delay_init(72);	     //延时初始化
//	uart2_init(36,9600);	 //串口初始化为9600
//	LED_Init();		  	 //初始化与LED连接的硬件接口
//	while(1)
//	{
//		LED0^=0x01;
//		
//		//echo recieved character.
//		//usart2_tx_byte(usart2_rx_byte());
//		
//		//test xgets from elm-chan.
//		xprintf("no.%d:\n",t);
//		xgets(rx_buf, 10);
//		xprintf("\nno.%d is %s\n",t,rx_buf);
////		delay_ms(500);
//		t++;
//	}
//}

//test program 6: ntshell test
//#include "ntshell.h"
////#include <string.h>
//#include "ntlibc.h"
//#include "ntshell_usrcmd.h"

//typedef struct {
//    ntshell_t ntshell;
//    int my_example_number;
//    char my_example_text[16];
//} example_work_t;

//static int func_read(char *buf, int cnt, void *extobj)
//{
//#if (YOUR_IMPLEMENTATION)
//    example_work_t *w = (example_work_t *)extobj;
//    UNUSED_VARIABLE(w);
//    UNUSED_VARIABLE(buf);
//    UNUSED_VARIABLE(cnt);
//#else
//    /*
//     * @todo Read characters to the buf.
//     * @note This is a blocking interface.
//     */
//    int i;
//    for(i=0;i<cnt;i++)
//    	buf[i]=usart2_rx_byte();
//#endif
//    return cnt;
//}

//static int func_write(const char *buf, int cnt, void *extobj)
//{
//#if (YOUR_IMPLEMENTATION)
//    example_work_t *w = (example_work_t *)extobj;
//    UNUSED_VARIABLE(w);
//    UNUSED_VARIABLE(buf);
//    UNUSED_VARIABLE(cnt);
//#else
//    /*
//     * @todo Write characters from the buf.
//     * @note This is a blocking interface.
//     */
//    int i;
//    for(i=0;i<cnt;i++)
//    	usart2_tx_byte(buf[i]);
//#endif
//    return cnt;
//}

//static int func_callback(const char *text, void *extobj)
//{
//#if (YOUR_IMPLEMENTATION)
//    example_work_t *w = (example_work_t *)extobj;
//    UNUSED_VARIABLE(w);
//    UNUSED_VARIABLE(text);
//    UNUSED_VARIABLE(extobj);
//#else
//#ifdef _WRITE_ALL_CMD_HERE_
//    /*
//     * @todo Handle the text.
//     */
//    //the dumy function is just echo the command.
//    //there are two ways.

//    //method 1:
//    //use functions of uart.c or utility.c, for example:
//    //UART_printf("%s\r\n",text);

//    //method 2:
//    //use elm-chan's xprintf.c, but the basic I/O should be defined.
//    //for example:
//    xdev_out(UARTSend_Byte);
//    xdev_in(UARTReceive_Byte);
//    //xprintf("%s\r\n",text);

//    //use libc string.h
//    if(strcmp(text, "led1on")==0)
//    	xprintf("led1 is on.\r\n");
//    //else
//    //	xprintf("unknown command\r\n");

//    //use ntlibc.h
//    else if(ntlibc_strcmp(text, "led1off")==0)
//    	xprintf("led1 is off\r\n");
//    else
//    	xprintf("unknown command\r\n");

//#endif
//    //let ntshell_usrcmd.c take the power.
//    ntshell_usrcmd_execute(text, extobj);
//#endif
//    return 0;
//}


//int main(void)
//{				
//	Stm32_Clock_Init(9); //系统时钟设置
//	delay_init(72);	     //延时初始化
//	//uart2_init(36,115200);	 //串口初始化为115200
//	uart2_init(36,9600);	 //串口初始化为9600
//	LED_Init();		  	 //初始化与LED连接的硬件接口

//	example_work_t w;
//  ntshell_init(&(w.ntshell), func_read, func_write, func_callback, (void *)&w);
//	ntshell_set_prompt(&(w.ntshell), "MCS-1-32-SHELL>");
//	ntshell_execute(&(w.ntshell));
//}

//test program 7: capture test: TIM5_CH1
//extern u8 pluse_flag;

//int main(void)
//{	
//	u16 pluse_count=0;
//	
//	Stm32_Clock_Init(9);
//	uart2_init(36,9600);
//	delay_init(72);
//	LED_Init();

// 	TIM5_Cap_Init(0xFFFF,72-1); //1MHz

//	xprintf("pluse count for JGB37-3530B-24V-333RPM motor.\n");
//	while(1)
//	{
//		if(pluse_flag==1)
//		{
//			pluse_count++;
//			if((pluse_count%16)==0)
//				LED0 ^= 0x01;
//			//WARNNING: xprintf will delay and lose data.
//			//xprintf("pluse: %d\n",pluse_count);
//			pluse_flag=0;
//		}
//	}
//}

//test program 7.1: capture test: TIM5_CH1
//putting pluse_count into ISR is better.
//extern u8 pluse_flag;
//extern u16 pluse_count;

//int main(void)
//{		
//	Stm32_Clock_Init(9);
//	uart2_init(36,9600);
//	delay_init(72);
//	LED_Init();

// 	TIM5_Cap_Init(0xFFFF,72-1); //1MHz

//	xprintf("pluse count for JGB37-3530B-24V-333RPM motor.\r\n");
//	while(1)
//	{
//		if(pluse_flag==1)
//		{
//			if((pluse_count%16)==0)
//				LED0 ^= 0x01;
//			xprintf("pluse: %d\r\n",pluse_count);
//			pluse_flag=0;
//		}
//	}
//}

//test program 8: CAN test: loop back mode test

//int main(void)
//{	
//	u8 i=0;
//	u8 cnt=0;
//	u8 can_tx_buf[8];
//	u8 can_rx_buf[8];
//	u8 nbytes=0;
//	u8 mode=1;				//loop back mode.
//	
//	Stm32_Clock_Init(9);
//	uart2_init(36,9600);
//	delay_init(72);
//	LED_Init();

// 	//CAN_Mode_Init(1,8,9,4,mode); //500kbps, test OK
//	CAN_Mode_Init(1,8,9,2,mode); //1Mbps, test OK

//	xprintf("CAN TEST.\r\n");
//	
//	while(1)
//	{
//		for(i=0;i<8;i++)
//			can_tx_buf[i]=cnt+i;
//		
//		xprintf("tx: %d %d %d %d %d %d %d %d ...", 
//		can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
//		can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);
//		
//		if(CAN_Send_Msg(can_tx_buf,8))
//			xprintf("failed.\r\n");
//		else
//			xprintf("OK.\r\n");
//		
//		nbytes=CAN_Receive_Msg(can_rx_buf);
//		
//		if(nbytes)
//		{	
//			xprintf("rx: ");
// 			for(i=0;i<nbytes;i++)
//			{									    
//				xprintf("%d ", can_rx_buf[i]);
// 			}
//			xprintf(".\r\n");
//		}
//		
//		cnt++; 
//		LED0^=0x01;
//		xprintf("run %d times\r\n\r\n", cnt);
//		delay_ms(1000);
//	}
//}

//test program 9: CAN test: control the RoboModule Motor Driver.
//int main(void)
//{	
//	u8 i=0;
//	u16 cnt=0;
//	u8 can_tx_buf[8];
//	u8 can_rx_buf[8];
//	u8 nbytes=0;
//	u8 mode=0;				//normal mode.
//	u32 id;
//	u16 temp_pwm=5000;
//	int16_t temp_speed=-5;
//	
//	Stm32_Clock_Init(9);
//	uart2_init(36,9600);
//	delay_init(72);
//	LED_Init();

//	CAN_Mode_Init(1,8,9,2,mode); //1Mbps

//	xprintf("CAN to RoboModule test.\r\n");
//	
//	//step 1: reset robomodule
//	id=0x10;
//	for(i=0;i<8;i++)
//			can_tx_buf[i]=0x55;
//	xprintf("tx: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...", 
//		can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
//		can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);
//	
//	if(CAN_tx_RoboModule(id, can_tx_buf))
//		xprintf("failed.\r\n");
//	else
//		xprintf("OK.\r\n");
//	
//		delay_ms(1000);
//	
//		nbytes=CAN_rx_RoboModule(can_rx_buf);
//		
//		if(nbytes==8)
//		{	
//			xprintf("rx: ");
// 			for(i=0;i<nbytes;i++)
//				xprintf("0x%02x ", can_rx_buf[i]);
//			xprintf(".\r\n");
//		}else{
//			xprintf("Received Wrong Data, exit.\r\n");
//			return -1;
//		}
//	
//	//delay_ms(1000);
//	
//	//step 2: select mode of robomodule
//	id=0x11;
//	can_tx_buf[0]=0x33; //speed loop mode.
//	for(i=1;i<8;i++)
//		can_tx_buf[i]=0x55;
//	xprintf("tx: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...", 
//		can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
//		can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);
//	
//	if(CAN_tx_RoboModule(id, can_tx_buf))
//		xprintf("failed.\r\n");
//	else
//		xprintf("OK.\r\n");
//	delay_ms(500);
//	
//	//step 3: send a command to robomodule every 20ms
//	id=0x14;
//	while(1)
//	{
//		can_tx_buf[0]=(temp_pwm>>8)&0xff;
//		can_tx_buf[1]=temp_pwm&0xff;
//		can_tx_buf[2]=(u8)((temp_speed>>8)&0xff);
//		can_tx_buf[3]=(u8)(temp_speed&0xff);
//		
//		for(i=4;i<8;i++)
//			can_tx_buf[i]=0x55;
//		
////		xprintf("tx: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...", 
////		can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
////		can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);
//		
//		if(CAN_tx_RoboModule(id, can_tx_buf))
//			xprintf("failed.\r\n");
////		else
////			xprintf("OK.\r\n");
//		
//		if(cnt++%50==0) 
//			LED0^=0x01;

//		delay_ms(20);
//	}
//}

//test program 9.1: CAN test: control 2 RoboModule Motor Driver.
//int main(void)
//{	
//	u8 i=0, n=0;
//	u16 cnt=0;
//	u8 can_tx_buf[8];
//	u8 can_rx_buf[8];
//	u8 nbytes=0;
//	u8 ndrivers=0;
//	u8 driver[15]={0}; //to store driver's no. 
//	u8 mode=0;				//normal mode.
//	u32 id;
//	u16 temp_pwm[15]={5000,5000}; //pwm setting: for driver 1 to 15
//	int16_t temp_speed[15]={-5,5}; //speed setting for driver 1 to 15
//	
//	Stm32_Clock_Init(9);
//	uart2_init(36,9600);
//	delay_init(72);
//	LED_Init();

//	CAN_Mode_Init(1,8,9,2,mode); //1Mbps

//	xprintf("CAN to RoboModule test.\r\n");
//	
//	//step 1: reset all robomodule and get drivers number
//	id=0x00;
//	for(i=0;i<8;i++)
//			can_tx_buf[i]=0x55;
//	xprintf("tx: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...", 
//		can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
//		can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);
//	
//	if(CAN_tx_RoboModule(id, can_tx_buf))
//		xprintf("failed.\r\n");
//	else
//		xprintf("OK.\r\n");
//	
//		delay_ms(1000);
//	
//		while((nbytes=CAN_rx_RoboModule(can_rx_buf))!=0)
//		{
//			xprintf("rx: ");
//			for(i=0;i<nbytes;i++)
//				xprintf("0x%02x ", can_rx_buf[i]);
//			xprintf(".\r\n");
//			xprintf("driver 0x%02x online.\r\n", can_rx_buf[0]);
//			driver[ndrivers]=can_rx_buf[0];
//			ndrivers++;
//		}
//		
//	//delay_ms(1000);
//	
//	//step 2: select mode of robomodule
//	for(n=0;n<ndrivers;n++)
//	{
//		id=(driver[n]<<4)|0x01;
//		can_tx_buf[0]=0x33; //speed loop mode.
//		for(i=1;i<8;i++)
//			can_tx_buf[i]=0x55;
//		
//		xprintf("tx driver 0x%02x: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...", 
//		driver[n], can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
//		can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);
//	
//		if(CAN_tx_RoboModule(id, can_tx_buf))
//			xprintf("failed.\r\n");
//		else
//			xprintf("OK.\r\n");
//		delay_ms(500);
//	}
//	//step 3: send a command to robomodule every 20ms
//	
//	while(1)
//	{
//		for(i=4;i<8;i++)
//			can_tx_buf[i]=0x55;
//		
//		for(n=0;n<ndrivers;n++)
//		{
//			id=(driver[n]<<4)|0x04;
//			
//			can_tx_buf[0]=(temp_pwm[driver[n]-1]>>8)&0xff;
//			can_tx_buf[1]=temp_pwm[driver[n]-1]&0xff;
//			can_tx_buf[2]=(u8)((temp_speed[driver[n]-1]>>8)&0xff);
//			can_tx_buf[3]=(u8)(temp_speed[driver[n]-1]&0xff);
//			
//			if(CAN_tx_RoboModule(id, can_tx_buf))
//				xprintf("failed.\r\n");
//	//	else
//	//		xprintf("OK.\r\n");
//		}
//		
//		if(cnt++%50==0) 
//			LED0^=0x01;

//		delay_ms(20);
//	}
//}

//test program 10: ntshell command with motor control
#include "ntshell.h"
//#include <string.h>
#include "ntlibc.h"
#include "ntshell_usrcmd.h"

typedef struct {
    ntshell_t ntshell;
    int my_example_number;
    char my_example_text[16];
} example_work_t;

static int func_read(char *buf, int cnt, void *extobj)
{
#if (YOUR_IMPLEMENTATION)
    example_work_t *w = (example_work_t *)extobj;
    UNUSED_VARIABLE(w);
    UNUSED_VARIABLE(buf);
    UNUSED_VARIABLE(cnt);
#else
    /*
     * @todo Read characters to the buf.
     * @note This is a blocking interface.
     */
    int i;
    for(i=0;i<cnt;i++)
    		buf[i]=usart1_rx_byte();
			//buf[i]=usart2_rx_byte();
#endif
    return cnt;
}

static int func_write(const char *buf, int cnt, void *extobj)
{
#if (YOUR_IMPLEMENTATION)
    example_work_t *w = (example_work_t *)extobj;
    UNUSED_VARIABLE(w);
    UNUSED_VARIABLE(buf);
    UNUSED_VARIABLE(cnt);
#else
    /*
     * @todo Write characters from the buf.
     * @note This is a blocking interface.
     */
    int i;
    for(i=0;i<cnt;i++)
			usart1_tx_byte(buf[i]);
			//usart2_tx_byte(buf[i]);
#endif
    return cnt;
}

static int func_callback(const char *text, void *extobj)
{
#if (YOUR_IMPLEMENTATION)
    example_work_t *w = (example_work_t *)extobj;
    UNUSED_VARIABLE(w);
    UNUSED_VARIABLE(text);
    UNUSED_VARIABLE(extobj);
#else
#ifdef _WRITE_ALL_CMD_HERE_
    /*
     * @todo Handle the text.
     */
#endif
    //let ntshell_usrcmd.c take the power.
    ntshell_usrcmd_execute(text, extobj);
#endif
    return 0;
}

//the following functions defined in ntshell_usrcmd.c
extern u16 pluse_width_tim5_ch1;
extern u16 pluse_width_tim5_ch2;
extern u8 pluse_obtained_tim5;
extern u16 pluse_width_tim3_ch3;
extern u16 pluse_width_tim3_ch4;
extern u8 pluse_obtained_tim3;
extern u16 pluse_width_tim8_ch1;
extern u16 pluse_width_tim8_ch2;
extern u8 pluse_obtained_tim8;

extern u8 ndrivers;

int mcs_res(void)
{
	u8 i=0;
	u8 can_tx_buf[8];
	u8 can_rx_buf[8];
	u32 id;
	u32 flag;
	u8 mode=0;				//normal mode.
	
	CAN_Mode_Init(1,8,9,2,mode); //1Mbps
	
	//reset all robomodule and get drivers number
	id=0x00;
	for(i=0;i<8;i++)
		can_tx_buf[i]=0x55;
	
	if(CAN_tx_RoboModule(id, can_tx_buf))
	{
		xprintf("failed.\r\n");
		return 1;
	}else{
		//xprintf("OK.\r\n");
	}
		
  delay_ms(1000);
	ndrivers=0;
	
	while(CAN_rx_RoboModule(can_rx_buf, &flag)!=0)
	{
		if(flag == 0x7ff)
		{
			xprintf("driver 0x%02x online.\r\n", can_rx_buf[0]);
			ndrivers++;
		}
	}
	
	xprintf("Total %02d driver(s) online.\r\n", ndrivers);
	
	return 0;
}

int mcs_mo(void)
{
	u8 i=0,n=0;
	u8 can_tx_buf[8];
	u8 can_rx_buf[8];
	u8 count[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	long driver=0, mode=0x33, period=10; //all drivers, speed mode, feedback every 100ms.
	u32 id;
	u8 nbytes;
	
	int16_t real_current; //
  int16_t real_speed; //
	long real_location; //
	
	id=(driver<<4)|0x01;
	can_tx_buf[0]=mode; //speed loop mode.
	for(i=1;i<8;i++)
		can_tx_buf[i]=0x55;
	
	xprintf("tx driver 0x%02x: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...", 
	driver, can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
	can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);

	if(CAN_tx_RoboModule(id, can_tx_buf))
		xprintf("failed.\r\n");
	else
	{
		xprintf("OK.\r\n");
		
		if(period>0)
		{
			id=(driver<<4)|0x06;
			can_tx_buf[0]=period;
		
			for(i=1;i<8;i++)
				can_tx_buf[i]=0x55;
		
			if(CAN_tx_RoboModule(id, can_tx_buf))
			{
				xprintf("Set feedback failed.\r\n");
				return 1;
			}else{
				xprintf("Set feedback OK.\r\n");
			}
			
			delay_ms(500);
			
			if(driver==0){
				do
				{
					nbytes=CAN_rx_RoboModule(can_rx_buf, &id);
		
					if(nbytes != 0 && ((id&0x0f)==7))
					{
						driver=((id>>4)&0x0f);
						count[driver-1]++;
						if(count[driver-1]==1)
						{				
							real_current=(can_rx_buf[0]<<8)|can_rx_buf[1];
							real_speed=(can_rx_buf[2]<<8)|can_rx_buf[3];
							real_location=(can_rx_buf[4]<<24)|(can_rx_buf[5]<<16)|(can_rx_buf[6]<<8)|can_rx_buf[7];
							xprintf("driver %02d: current=%d speed=%d location=%ld\r\n", (id>>4)&0xff, real_current, real_speed, real_location);
				
							n++;  
						}
					}
				}while(n<ndrivers);
			}else{
				do
				{
					nbytes=CAN_rx_RoboModule(can_rx_buf, &id);
				}while((nbytes == 0) || (id!=((driver<<4)|0x07)));
				
				real_current=(can_rx_buf[0]<<8)|can_rx_buf[1];
				real_speed=(can_rx_buf[2]<<8)|can_rx_buf[3];
				real_location=(can_rx_buf[4]<<24)|(can_rx_buf[5]<<16)|(can_rx_buf[6]<<8)|can_rx_buf[7];
				xprintf("driver %02d: current=%d speed=%d location=%ld\r\n", (id>>4)&0x0f, real_current, real_speed, real_location);
			}
		}
	}
	return 0;
}

int mcs_th9x(void)
{
	u8 i=0;
	u8 drive_mode=0x33;
	u8 can_tx_buf[8];
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
	int16_t motor_speed[3];
	
	float rotate_speed;
	float vx, vy;
	
	int16_t rotate_pwm;
	int16_t vx_pwm, vy_pwm;
	int16_t brightness;
	
	TIM5_Cap_Init(0xFFFF,72-1); //1MHz
	TIM3_Cap_Init(0xFFFF,72-1); //1MHz
	TIM8_Cap_Init(0xFFFF,72-1);
	
	//use TIM1_CH1/PA8 to drive LED
	//the following two lines corrected a hardware bug of mcs-1-32
	GPIOB->CRL&=0xFF0FFFFF;	//set PB5 as float input
	GPIOB->CRL|=0x00400000;	
	
	TIM1_PWM_Init(899,0); //80KHz pwm to drive LED
	TIM1->CCR1=0;

	//if PG9 is connected to GND, exit from the loop
	GPIOG->CRH&=0xffffff0f;
	GPIOG->CRH|=0x00000080;	//PG9 input with pull-up/pull-down
	//GPIOG->ODR&=~(1<<9);		//PG9 pull-down
	GPIOG->ODR|=1<<9;		//PG9 pull-up
	RCC->APB2ENR|=1<<8;    	//enable PORTG clock
	
	while(PGin(9)){
		if(((pluse_obtained_tim5&0x03)==3)&&((pluse_obtained_tim3&0x02)==2)){
//			xprintf("TA5_CH1 pluse width is %ld\r\n", pluse_width_ch1);
//			xprintf("TA5_CH2 pluse width is %ld\r\n", pluse_width_ch2);
			
			if(drive_mode==0x33){
				vx=(pluse_width_tim5_ch1-1484)/70.0;
				vy=(pluse_width_tim5_ch2-1512)/70.0;
				
				rotate_speed=(pluse_width_tim3_ch4-1526)/100.0;
				
				motor_speed[0]=lroundf(-vx-rotate_speed);
				motor_speed[1]=lroundf((vx/2.0)+(0.866*vy)-rotate_speed);
				motor_speed[2]=lroundf((vx/2.0)-(0.866*vy)-rotate_speed);
				
				for(i=4;i<8;i++)
					can_tx_buf[i]=0x55;
				
				can_tx_buf[0]=(temp_pwm>>8)&0xff;
				can_tx_buf[1]=temp_pwm&0xff;
				
				for(i=0;i<3;i++){
					can_tx_buf[2]=(u8)((motor_speed[i]>>8)&0xff);
					can_tx_buf[3]=(u8)(motor_speed[i]&0xff);
					
					id=((i+1)<<4)|0x04;
					
					if(CAN_tx_RoboModule(id, can_tx_buf)){
						xprintf("Motor %02d failed with speed mode.\r\n", i+1);
						//return 1;
					}
				}
		  }else if(drive_mode==0x22){
				vx_pwm=(pluse_width_tim5_ch1-1484)*5;
				vy_pwm=(pluse_width_tim5_ch2-1512)*5;
				
				rotate_pwm=(pluse_width_tim3_ch4-1526)*5;
				
				motor_speed[0]=-vx_pwm-rotate_pwm;
				motor_speed[1]=(vx_pwm/2)+(0.866*vy_pwm)-rotate_pwm;
				motor_speed[2]=(vx_pwm/2)-(0.866*vy_pwm)-rotate_pwm;
				//motor_speed[2]=((vx_pwm/2)-(0.866*vy_pwm)-rotate_pwm)*1.2; //test only
				
				for(i=2;i<8;i++)
					can_tx_buf[i]=0x55;
				
				for(i=0;i<3;i++){
					//xprintf("motor %02d pwm is %d\r\n", i+1, motor_speed[i]); 
					can_tx_buf[0]=(u8)((motor_speed[i]>>8)&0xff);
					can_tx_buf[1]=(u8)(motor_speed[i]&0xff);
					
					id=((i+1)<<4)|0x03;
					
					if(CAN_tx_RoboModule(id, can_tx_buf)){
						xprintf("Motor %02d failed with pwm mode.\r\n", i+1);
						//return 1;
					}
				}
			}
			pluse_obtained_tim5=0;
			pluse_obtained_tim3=0;
			delay_ms(10);
		}
		
		//adjust brightness of LED by tim8_ch1(capture) and tim1_ch1(compare pwm)
		brightness=1903-pluse_width_tim8_ch1;
		if(brightness>=0 && brightness <=899){
			TIM1->CCR1=brightness;
		}else{
			TIM1->CCR1=0;
		}
		
		//switch on/off led0 by tim8_ch2
		if(pluse_width_tim8_ch2>1500)
			LED0=1;
		else
			LED0=0;
		
	}
	
	return 0;
}


int main(void)
{				
	Stm32_Clock_Init(9); //系统时钟设置
	delay_init(72);	     //延时初始化
	uart1_init(72,115200);	 //串口1初始化为115200
	//uart2_init(36,9600);	 //串口2初始化为9600, to wireless module
	LED_Init();		  	 //初始化与LED连接的硬件接口

	TIM8_Cap_Init(0xFFFF,72-1); //initialize TIM8 to capture the radio signal
	delay_ms(500); //capturing the signal from th9x
	
	if(pluse_width_tim8_ch2>1500){ //
		LED0=1;
		if(mcs_res()){
			; //error 
		}
		if(mcs_mo()){
			; //error 
		}
		if(mcs_th9x()){
			; //error 
		}
			
	}else{
		LED0=0;
		example_work_t w;
		ntshell_init(&(w.ntshell), func_read, func_write, func_callback, (void *)&w);
		ntshell_set_prompt(&(w.ntshell), "zkhf>");
		ntshell_execute(&(w.ntshell));
	}

}

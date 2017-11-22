/**
 * @file ntshell_usrcmd.c
 * @author Shinichiro Nakamura
 * @brief ナチュラルタイニーシェルタスクコマンドの実装。
 * @details
 */

/*
 * ===============================================================
 *  Natural Tiny Shell - User Command Module
 * ===============================================================
 * Copyright (c) 2010-2011 Shinichiro Nakamura
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 * ===============================================================
 */
 
 /*
  * Implement commands here.
	* SUN GE <sun.ge@126.com>
  */
 

#include "ntopt.h"
#include "ntlibc.h"
#include "ntint.h"
#include "xprintf.h"

#include "led.h"
#include "timer.h"
#include "can.h"
#include "delay.h"
#include "math.h"

#ifndef NULL
#define NULL ((void *) 0)
#endif

static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj);
static int usrcmd_help(int argc, char **argv);
static int usrcmd_info(int argc, char **argv);

//led control
static int usrcmd_setled0(int argc, char **argv);
static int usrcmd_br(int argc, char **argv);

//motor control
static int usrcmd_res(int argc, char **argv);
static int usrcmd_mo(int argc, char **argv);
static int usrcmd_sp(int argc, char **argv);
static int usrcmd_pwm(int argc, char **argv);
static int usrcmd_lo(int argc, char **argv);
static int usrcmd_fb(int argc, char **argv);

//motion control simple
static int usrcmd_ss(int argc, char **argv);
//static int usrcmd_fs(int argc, char **argv);
//static int usrcmd_bs(int argc, char **argv);
static int usrcmd_tls(int argc, char **argv);
static int usrcmd_trs(int argc, char **argv);

static int usrcmd_sl(int argc, char **argv);

//motion control advanced
static int usrcmd_gs(int argc, char **argv);
static int usrcmd_ga(int argc, char **argv);

//test radio receiver
static int usrcmd_r9b(int argc, char **argv);

//motion control by FS-TH9X
static int usrcmd_th9x(int argc, char **argv);

/**
 * @brief コマンドテーブル。
 */
typedef struct {
    char *cmd;  /**< コマンド名。 */
    char *desc; /**< コマンド説明。 */
    int (*func)(int argc, char **argv); /**< コールバック関数。 */
} cmd_table_t;

/**
 * @brief コマンドの定義。
 * @details システムで必要なコマンドの実装を追加すると良い。
 */
static const cmd_table_t cmdlist[] = {
    { "help", "Help to learn", usrcmd_help },
    { "info", "About the System", usrcmd_info },
    { "setled0", "Switch On or Off LED0", usrcmd_setled0},
		{ "br", "Set the Brightness of LED Light", usrcmd_br},
		{ "res", "Reset All Motor Connected to the CAN Bus", usrcmd_res},
		{ "mo", "Set Motor Mode", usrcmd_mo},
		{ "sp", "Set Motor Speed", usrcmd_sp},
		{ "pwm", "Set PWM for Driving Motor", usrcmd_pwm},
		{ "lo", "Set Motor Location", usrcmd_lo},
		{ "fb", "Motor Feedback", usrcmd_fb},
		{ "ss", "Stop All Motors in Speed Mode", usrcmd_ss},
//		{ "fs", "Go Forward in Speed Mode", usrcmd_fs},
//		{ "bs", "Go Backward in Speed Mode", usrcmd_bs},
		{ "tls", "Turn Anti-clockwise in Speed Mode", usrcmd_tls},
		{ "trs", "Turn Clockwise in Speed Mode", usrcmd_trs},
		{ "sl", "Stop All Motors in Location Mode", usrcmd_sl},
		{ "gs", "Go with Specified Speed by Speed Mode", usrcmd_gs},
		{ "ga", "Go to Specified Angle with Specified Speed by Speed Mode", usrcmd_ga},
		{ "r9b", "Test the FS-TH9X/FS-R9B Transmiter/Reciever", usrcmd_r9b},
		{ "th9x", "Let FS-TH9X Take Control", usrcmd_th9x},
    { NULL, NULL, NULL }
};

/**
 * @brief NT-Shellコマンドを実行する。
 * @details
 *
 * @param text テキスト。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */
int ntshell_usrcmd_execute(const char *text, void *extobj)
{
    /*
     * ntoptライブラリを使って引数を解析し実行する。
     */
    return ntopt_parse(text, usrcmd_ntopt_callback, NULL);
}

/**
 * @brief ユーザコマンドを実行する。
 * @details
 *
 * @param argc 引数の数。
 * @param argv 引数。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */
static int usrcmd_ntopt_callback(int argc, char **argv, void *extobj)
{
    /*
     * 引数の数が0ならば何もしない。
     * 単にenterが押されただけと解釈する。
     */
    if (argc == 0) {
        return 0;
    }

    /*
     * コマンドテーブルを探索して、
     * コマンド名が一致したらコールバック関数を呼び出す。
     */
    const cmd_table_t *p = &cmdlist[0];
    while (p->cmd != NULL) {
        if (ntlibc_strcmp((const char *)argv[0], p->cmd) == 0) {
            return p->func(argc, argv);
        }
        p++;
    }

    /*
     * ここに到達するということは、未知のコマンドである。
     */
    {
        static const char *msg = "Unknown command. Type help to get command list.";
        xprintf("%s\r\n",msg);
    }
    return 0;
}

/**
 * @brief helpコマンド。
 * @details
 *
 * @param argc 引数の数。
 * @param argv 引数。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */
static int usrcmd_help(int argc, char **argv)
{
    const cmd_table_t *p = &cmdlist[0];
    char buf[128];

    /*
     * コマンド名とコマンド説明を列挙する。
     */
    while (p->cmd != NULL) {
        ntlibc_strcpy(buf, p->cmd);
        ntlibc_strcat(buf, "\t:");
        ntlibc_strcat(buf, p->desc);
        ntlibc_strcat(buf, "\r\n");
        xprintf("%s",(const char *)buf);
        p++;
    }
    return 0;
}

static int usrcmd_info(int argc, char **argv)
{
    if (argc != 2) {
        static const char *msg = "info sys\r\ninfo ver\r\n";
        xprintf("%s",msg);
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "sys") == 0) {
        static const char *msg = "NT-Shell for zkhf\r\n";
        xprintf("%s",msg);
        return 0;
    }
    if (ntlibc_strcmp(argv[1], "ver") == 0) {
        static const char *msg = "0\tJuly 30,2015\r\n";
        xprintf("%s",msg);
        return 0;
    }

    static const char *msg = "Unknown parameter.\n";
    xprintf("%s",msg);
    return -1;
}


static int usrcmd_setled0(int argc, char **argv)
{
	
	if (argc != 2) {
		xprintf("%s on/off\r\n", argv[0]);
		return 1;
	}

	if (ntlibc_strcmp(argv[1],"on")==0) {
		LED0=1;
		return 0;
	}
	
	if (ntlibc_strcmp(argv[1],"off")==0) {
		LED0=0;
		return 0;
	}
	
	xprintf("%s is illegal. Please input on or off\r\n",argv[1]);
	return 1;

}

//drive PB5 by TIM3(CH2, remap) with pwm.
//PB5 is connected to an LED.
//the following command will adjust the brightness of LED.
//see TIM3_PWM_Init for details.
static int usrcmd_br(int argc, char **argv)
{
	long brightness;
	
	//TIM3_PWM_Init(899,0);//不分频。PWM频率=72000/(899+1)=80Khz
	
	//use TIM1_CH1/PA8 to drive LED
	GPIOB->CRL&=0xFF0FFFFF;	//set PB5 as float input
	GPIOB->CRL|=0x00400000;	
	
	TIM1_PWM_Init(899,0);
	//TIM3->CCR2=0;	
	TIM1->CCR1=0;

	if (argc != 2) {
		xprintf("%s [0-899]. 0~899 is the brightness value of led.\r\n", argv[0]);
		return 1;
	}
	
	if(xatoi(&argv[1], &brightness)==0){
		xprintf("the argument must be a number.\r\n");
		return 1;
	}
	
	if(brightness>=0 && brightness <=899){
		//TIM3->CCR2=(u16)brightness;
		TIM1->CCR1=(u16)brightness;
		return 0;
	}
	
	xprintf("the brightness value must between 0 and 899.\r\n");

	return 1;
}


u16 mode_flag=0;
//u8 fb_flag=0;

//how many drivers online
u8 ndrivers=0;

static int usrcmd_res(int argc, char **argv)
//int usrcmd_res(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	u8 can_rx_buf[8];
  u8 nbytes=0;
	u32 id;
	u32 flag;
	u8 mode=0;				//normal mode.
	
	CAN_Mode_Init(1,8,9,2,mode); //1Mbps

	//xprintf("CAN to RoboModule test.\r\n");
	
	//reset all robomodule and get drivers number
	id=0x00;
	for(i=0;i<8;i++)
		can_tx_buf[i]=0x55;
	
//	xprintf("tx: 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x ...", 
//		can_tx_buf[0],can_tx_buf[1],can_tx_buf[2],can_tx_buf[3],
//		can_tx_buf[4],can_tx_buf[5],can_tx_buf[6],can_tx_buf[7]);
	
	if(CAN_tx_RoboModule(id, can_tx_buf))
	{
		xprintf("failed.\r\n");
		return 1;
	}else{
		//xprintf("OK.\r\n");
	}
		
  delay_ms(1000);
	ndrivers=0;
	
	while((nbytes=CAN_rx_RoboModule(can_rx_buf, &flag))!=0)
	{
		if(flag == 0x7ff)
		{
//			xprintf("rx: ");
//			for(i=0;i<nbytes;i++)
//				xprintf("0x%02x ", can_rx_buf[i]);
//			xprintf(".\r\n");
			xprintf("driver 0x%02x online.\r\n", can_rx_buf[0]);

			ndrivers++;
			
//			fb_flag=0;
		}
	}
	
	//clear mode_flag
	mode_flag=0;
	xprintf("Total %02d driver(s) online.\r\n", ndrivers);
	
	return 0;
}

//set motor and feedback
static int usrcmd_mo(int argc, char **argv)
{
	u8 i=0,n=0;
	u8 can_tx_buf[8];
	u8 can_rx_buf[8];
	u8 count[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	long driver, mode, period;
	u32 id;
	u8 nbytes;
	
	int16_t real_current; //
  int16_t real_speed; //
	long real_location; //
	
	if(argc != 4){	
		xprintf("%s motor_number(0-15)  motor_mode(pwm1/pwm2/speed/location) feedback_interval(0-255, 0 for no feedback)\r\n", argv[0]);
		return 1;
	}
	
	//select mode of robomodule
	xatoi(&argv[1], &driver);
	if(driver<0 || driver>15){
		xprintf("motor_number is between 0 and 15.\r\n");
		return 1;
	}
	
	if(ntlibc_strcmp(argv[2],"pwm1")==0){
		mode=0x11;
	}else if(ntlibc_strcmp(argv[2],"pwm2")==0){
		mode=0x22;
	}else if(ntlibc_strcmp(argv[2],"speed")==0){
		mode=0x33;
	}else if(ntlibc_strcmp(argv[2],"location")==0){
		mode=0x44;
	}else{
		xprintf("available motor_mode is pwm1/pwm2/speed/location.\r\n");
		return 1;
	}
	
//	xatoi(&argv[2], &mode);
//	if(mode != 0x11 && mode != 0x22 && mode != 0x33 && mode != 0x44){
//		xprintf("available motor_mode is 0x11/0x22/0x33/0x44.\r\n");
//		return 1;
//	}
	
	xatoi(&argv[3], &period);
	if(period<0 || period>0xff){
		xprintf("available period is 0~255. 0 for no feedback.\r\n");
		return 1;
	}
	
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
		mode_flag|=(1<<driver);
		
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
//				fb_flag=1;
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

//set pwm of motor
static int usrcmd_pwm(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	long driver;
	u32 id;
	
  long pwm; //pwm setting: -5000~5000
	
	if(argc != 3){	
		xprintf("%s motor_number(0-15) pwm(-5000~5000)\r\n", argv[0]);
		return 1;
	}
	
	xatoi(&argv[1], &driver);
	if(driver<0 || driver>15){
		xprintf("motor_number is between 0 and 15.\r\n");
		return 1;
	}
	
	xatoi(&argv[2], &pwm);
	if(pwm<-5000 || pwm>5000){
		xprintf("available pwm is -5000~5000.\r\n");
		return 1;
	}
	
	// if mode_flag is not set.
	if(((mode_flag&(1<<driver))==0) && ((mode_flag&0x01)==0)){
		xprintf("set motor mode first.\r\n");
		return 1;
	}
	
	for(i=2;i<8;i++)
		can_tx_buf[i]=0x55;
		
	id=(driver<<4)|0x03;
	
	can_tx_buf[0]=(pwm>>8)&0xff;
	can_tx_buf[1]=pwm&0xff;
	
	if(CAN_tx_RoboModule(id, can_tx_buf)){
		xprintf("failed.\r\n");
		return 1;
	}
	
	return 0;
}

//set speed of motor
static int usrcmd_sp(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	long driver;
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long temp_speed; //speed setting: -1000~1000
	
	if(argc != 3){	
		xprintf("%s motor_number(0-15) motor_speed(-1000~1000)\r\n", argv[0]);
		return 1;
	}
	
	xatoi(&argv[1], &driver);
	if(driver<0 || driver>15){
		xprintf("motor_number is between 0 and 15.\r\n");
		return 1;
	}
	
	xatoi(&argv[2], &temp_speed);
	if(temp_speed<-1000 || temp_speed>1000){
		xprintf("available speed is -1000~1000.\r\n");
		return 1;
	}
	
	// if mode_flag is not set.
	if(((mode_flag&(1<<driver))==0) && ((mode_flag&0x01)==0)){
		xprintf("set motor mode first.\r\n");
		return 1;
	}
	
	for(i=4;i<8;i++)
		can_tx_buf[i]=0x55;
		
	id=(driver<<4)|0x04;
	
	can_tx_buf[0]=(temp_pwm>>8)&0xff;
	can_tx_buf[1]=temp_pwm&0xff;
	can_tx_buf[2]=(u8)((temp_speed>>8)&0xff);
	can_tx_buf[3]=(u8)(temp_speed&0xff);
	
	if(CAN_tx_RoboModule(id, can_tx_buf)){
		xprintf("failed.\r\n");
		return 1;
	}
	
	return 0;
}

//set location of motor
static int usrcmd_lo(int argc, char **argv)
{
	u8 can_tx_buf[8];
	long driver;
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long temp_speed; //speed setting: 0~1000
	long temp_location; //location setting: -5000000~5000000
	
	if(argc != 4){	
		xprintf("lo motor_number(0-15) motor_speed(0~1000) motor_location(-5000000~5000000)\r\n");
		return 1;
	}
	
	xatoi(&argv[1], &driver);
	if(driver<0 || driver>15){
		xprintf("motor_number is between 0 and 15.\r\n");
		return 1;
	}
	
	xatoi(&argv[2], &temp_speed);
	if(temp_speed<0 || temp_speed>1000){
		xprintf("available speed is 0~1000.\r\n");
		return 1;
	}
	
	xatoi(&argv[3], &temp_location);
	if(temp_speed<-5000000 || temp_speed>5000000){
		xprintf("available location is -5000000~5000000.\r\n");
		return 1;
	}	
	
	// if mode_flag is not set.
	if(((mode_flag&(1<<driver))==0) && ((mode_flag&0x01)==0)){
		xprintf("set motor mode first.\r\n");
		return 1;
	}
		
	id=(driver<<4)|0x05;
	
	can_tx_buf[0]=(temp_pwm>>8)&0xff;
	can_tx_buf[1]=temp_pwm&0xff;
	can_tx_buf[2]=(u8)((temp_speed>>8)&0xff);
	can_tx_buf[3]=(u8)(temp_speed&0xff);
	can_tx_buf[4]=(temp_location>>24)&0xff;
	can_tx_buf[5]=(temp_location>>16)&0xff;
	can_tx_buf[6]=(temp_location>>8)&0xff;
	can_tx_buf[7]=temp_location&0xff;
	
	if(CAN_tx_RoboModule(id, can_tx_buf)){
		xprintf("failed.\r\n");
		return 1;
	}
	
	return 0;
}

//obtain feedback of drivers
static int usrcmd_fb(int argc, char **argv)
{
	u8 i=0,n=0;
	u8 can_tx_buf[8];
	u8 can_rx_buf[8];
	u8 nbytes;
	u32 id;
	long driver;
	long period;				//0-255
	u8 count[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	int16_t real_current; //
  int16_t real_speed; //
	long real_location; //
	
	if(argc != 3){	
		xprintf("fb motor_number(0-15) feedback_interval(0-255, 0 for rx only)\r\n");
		return 1;
	}
	
	xatoi(&argv[1], &driver);
	if(driver<0 || driver>15){
		xprintf("motor_number is between 0 and 15.\r\n");
		return 1;
	}
	
	xatoi(&argv[2], &period);
	if(period<0 || period>0xff){
		xprintf("available period is 0~255.\r\n");
		return 1;
	}
	
// if mode_flag is not set.
	if(((mode_flag&(1<<driver))==0) && ((mode_flag&0x01)==0)){
		xprintf("set motor mode first.\r\n");
		return 1;
	}

	if(period!=0)
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
 }
	
//	i=0;
//	do
//	{
//		nbytes=CAN_rx_RoboModule(can_rx_buf, &id);
//		
//		if(nbytes != 0 && ((id&0x0f)==7))
//		{
//			i++;
//			real_current=(can_rx_buf[0]<<8)|can_rx_buf[1];
//			real_speed=(can_rx_buf[2]<<8)|can_rx_buf[3];
//			real_location=(can_rx_buf[4]<<24)|(can_rx_buf[5]<<16)|(can_rx_buf[6]<<8)|can_rx_buf[7];
//			xprintf("driver %02d: current=%d speed=%d location=%ld\r\n", (id>>4)&0xff, real_current, real_speed, real_location);
//		}
//	}
//	while((i<30));
 
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
 
	return 0;
}

//stop all motors in speed mode
static int usrcmd_ss(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	long driver=0; // 0 for all motors
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long temp_speed=0; //speed setting: 0 for stop
	
	for(i=4;i<8;i++)
		can_tx_buf[i]=0x55;
		
	id=(driver<<4)|0x04;
	
	can_tx_buf[0]=(temp_pwm>>8)&0xff;
	can_tx_buf[1]=temp_pwm&0xff;
	can_tx_buf[2]=(u8)((temp_speed>>8)&0xff);
	can_tx_buf[3]=(u8)(temp_speed&0xff);
	
	if(CAN_tx_RoboModule(id, can_tx_buf)){
		xprintf("failed.\r\n");
		return 1;
	}
	
	return 0;
}

//static int usrcmd_fs(int argc, char **argv)
//{
//	u8 i=0,n=0;
//	u8 can_tx_buf[2][8];
//	u32 id[2];
//	
//	u16 temp_pwm=5000; //set pwm limit: 0~5000
//  long temp_speed; //speed setting: 0~1000
//	
//	if(argc != 2){	
//		xprintf("%s motor_speed(0~1000)\r\n", argv[0]);
//		return 1;
//	}
//	
//	xatoi(&argv[1], &temp_speed);
//	if(temp_speed<0 || temp_speed>1000){
//		xprintf("available speed is 0~1000.\r\n");
//		return 1;
//	}
//	
//	can_tx_buf[0][2]=(u8)((temp_speed>>8)&0xff);
//	can_tx_buf[0][3]=(u8)(temp_speed&0xff);
//	
//	temp_speed=(~temp_speed)+1;
//	can_tx_buf[1][2]=(u8)((temp_speed>>8)&0xff);
//	can_tx_buf[1][3]=(u8)(temp_speed&0xff);	
//	
//	for(n=0;n<2;n++){
//		can_tx_buf[n][0]=(temp_pwm>>8)&0xff;
//		can_tx_buf[n][1]=temp_pwm&0xff;
//		
//		for(i=4;i<8;i++)
//			can_tx_buf[n][i]=0x55;
//	}
//	
//	id[0]=(2<<4)|0x04;
//	id[1]=(3<<4)|0x04;
//	
//	if(CAN_tx_RoboModule(id[0], can_tx_buf[0]) || CAN_tx_RoboModule(id[1], can_tx_buf[1])){
//		xprintf("failed.\r\n");
//		return 1;
//	}
//	
//	return 0;
//}

//static int usrcmd_bs(int argc, char **argv)
//{
//	u8 i=0,n=0;
//	u8 can_tx_buf[2][8];
//	u32 id[2];
//	
//	u16 temp_pwm=5000; //set pwm limit: 0~5000
//  long temp_speed; //speed setting: 0~1000
//	
//	if(argc != 2){	
//		xprintf("%s motor_speed(0~1000)\r\n", argv[0]);
//		return 1;
//	}
//	
//	xatoi(&argv[1], &temp_speed);
//	if(temp_speed<0 || temp_speed>1000){
//		xprintf("available speed is 0~1000.\r\n");
//		return 1;
//	}
//	
//	can_tx_buf[0][2]=(u8)((temp_speed>>8)&0xff);
//	can_tx_buf[0][3]=(u8)(temp_speed&0xff);
//	
//	temp_speed=(~temp_speed)+1;
//	can_tx_buf[1][2]=(u8)((temp_speed>>8)&0xff);
//	can_tx_buf[1][3]=(u8)(temp_speed&0xff);	
//	
//	for(n=0;n<2;n++){
//		can_tx_buf[n][0]=(temp_pwm>>8)&0xff;
//		can_tx_buf[n][1]=temp_pwm&0xff;
//		
//		for(i=4;i<8;i++)
//			can_tx_buf[n][i]=0x55;
//	}
//	
//	id[0]=(2<<4)|0x04;
//	id[1]=(3<<4)|0x04;
//	
//	if(CAN_tx_RoboModule(id[0], can_tx_buf[1]) || CAN_tx_RoboModule(id[1], can_tx_buf[0])){
//		xprintf("failed.\r\n");
//		return 1;
//	}
//	
//	return 0;
//}

//turn anti-clockwise in speed mode
static int usrcmd_tls(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long temp_speed; //speed setting: 0~1000
	
	if(argc != 2){	
		xprintf("%s speed(0~1000)\r\n", argv[0]);
		return 1;
	}
	
	xatoi(&argv[1], &temp_speed);
	if(temp_speed<0 || temp_speed>1000){
		xprintf("available speed is 0~1000.\r\n");
		return 1;
	}
	
	for(i=4;i<8;i++)
		can_tx_buf[i]=0x55;
		
	id=0x04;
	
	can_tx_buf[0]=(temp_pwm>>8)&0xff;
	can_tx_buf[1]=temp_pwm&0xff;
	can_tx_buf[2]=(u8)((temp_speed>>8)&0xff);
	can_tx_buf[3]=(u8)(temp_speed&0xff);
	
	if(CAN_tx_RoboModule(id, can_tx_buf)){
		xprintf("failed.\r\n");
		return 1;
	}
	
	return 0;
}

//turn clockwise in speed mode
static int usrcmd_trs(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long temp_speed; //speed setting: 0~1000
	
	if(argc != 2){	
		xprintf("%s speed(0~1000)\r\n", argv[0]);
		return 1;
	}
	
	xatoi(&argv[1], &temp_speed);
	if(temp_speed<0 || temp_speed>1000){
		xprintf("available speed is 0~1000.\r\n");
		return 1;
	}
	
	for(i=4;i<8;i++)
		can_tx_buf[i]=0x55;
		
	id=0x04;
	temp_speed=(~temp_speed)+1;
	
	can_tx_buf[0]=(temp_pwm>>8)&0xff;
	can_tx_buf[1]=temp_pwm&0xff;
	can_tx_buf[2]=(u8)((temp_speed>>8)&0xff);
	can_tx_buf[3]=(u8)(temp_speed&0xff);
	
	if(CAN_tx_RoboModule(id, can_tx_buf)){
		xprintf("failed.\r\n");
		return 1;
	}
	
	return 0;
}

//stop all motors at current location
static int usrcmd_sl(int argc, char **argv)
{
	u8 can_tx_buf[8];
	u8 can_rx_buf[8];
	
	u8 driver;
	u32 id;
	u16 count[15]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	u8 nbytes=0;
	u8 n=0;
	
	int16_t real_current; //
  int16_t real_speed; //
	long real_location; //
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long temp_speed=1; //
	
	do
	{
		nbytes=CAN_rx_RoboModule(can_rx_buf, &id);
		
		if(nbytes != 0 && ((id&0x0f)==7))
		{
			driver=(id>>4)&0x0f;
			count[driver-1]++;
			if(count[driver-1]==1)
			{	
					
				real_current=(can_rx_buf[0]<<8)|can_rx_buf[1];
				real_speed=(can_rx_buf[2]<<8)|can_rx_buf[3];
				real_location=(can_rx_buf[4]<<24)|(can_rx_buf[5]<<16)|(can_rx_buf[6]<<8)|can_rx_buf[7];
				xprintf("driver %02d: current=%d speed=%d location=%ld\r\n", (id>>4)&0x0f, real_current, real_speed, real_location);
				
				id=(driver<<4)|0x05;
				can_tx_buf[0]=(temp_pwm>>8)&0xff;
				can_tx_buf[1]=temp_pwm&0xff;
				can_tx_buf[2]=(u8)((temp_speed>>8)&0xff);
				can_tx_buf[3]=(u8)(temp_speed&0xff);
				can_tx_buf[4]=can_rx_buf[4];
				can_tx_buf[5]=can_rx_buf[5];
				can_tx_buf[6]=can_rx_buf[6];
				can_tx_buf[7]=can_rx_buf[7];
				
				if(CAN_tx_RoboModule(id, can_tx_buf)){
					xprintf("failed.\r\n");
					return 1;
				}
				
				n++;  
			}
		}
	}while(n<ndrivers);
	
	return 0;
}

//drive three motors with specified speed
static int usrcmd_gs(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long temp_speed[3]; //speed setting: -1000~1000
	
	if(argc != 4){	
		xprintf("%s motor1_speed motor2_speed motor3_speed (speed value is -1000~1000)\r\n", argv[0]);
		return 1;
	}
	
	for(i=0;i<3;i++){
		xatoi(&argv[i+1], &temp_speed[i]);
		if(temp_speed[i]<-1000 || temp_speed[i]>1000){
			xprintf("available speed is -1000~1000.\r\n");
			return 1;
		}
		
		// if mode_flag is not set.
		if(((mode_flag&(1<<(i+1)))==0) && ((mode_flag&0x01)==0)){
			xprintf("set motor mode first.\r\n");
			return 1;
		}
	}
	
	for(i=4;i<8;i++)
		can_tx_buf[i]=0x55;
	
	can_tx_buf[0]=(temp_pwm>>8)&0xff;
	can_tx_buf[1]=temp_pwm&0xff;
	
	for(i=0;i<3;i++){
		can_tx_buf[2]=(u8)((temp_speed[i]>>8)&0xff);
		can_tx_buf[3]=(u8)(temp_speed[i]&0xff);
		
		id=((i+1)<<4)|0x04;
		
		if(CAN_tx_RoboModule(id, can_tx_buf)){
			xprintf("Motor %02d failed with speed mode.\r\n", i+1);
			return 1;
		}
	}
	return 0;
}

//goto specified angle by speed mode
#define PI 3.14
static int usrcmd_ga(int argc, char **argv)
{
	u8 i=0;
	u8 can_tx_buf[8];
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
  long speed; //speed setting: 0~1000
	long angle;
	int16_t motor_speed[3];
	
	float vx, vy;
	
	if(argc != 3){	
		xprintf("%s speed(0~1000) angle(0-360)\r\n", argv[0]);
		return 1;
	}
	
	xatoi(&argv[1], &speed);
	if(speed<0 || speed>1000){
		xprintf("available speed is 0~1000.\r\n");
		return 1;
	}
	
	xatoi(&argv[2], &angle);
	if(angle<0 || angle>360){
		xprintf("available angle is between 0 and 360.\r\n");
		return 1;
	}
		
	// if mode_flag is not set.
	if(((mode_flag&(1<<(i+1)))==0) && ((mode_flag&0x01)==0)){
		xprintf("set motor mode first.\r\n");
		return 1;
	}
	
	vx=((float)speed)*cosf(((float)angle)*PI/180.0);
	vy=((float)speed)*sinf(((float)angle)*PI/180.0);
	motor_speed[0]=lroundf(-vx);
	motor_speed[1]=lroundf((vx/2.0)+(0.866*vy));
	motor_speed[2]=lroundf((vx/2.0)-(0.866*vy));
	
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
			return 1;
		}
	}
	return 0;
}

extern u16 pluse_width_tim5_ch1;
extern u16 pluse_width_tim5_ch2;
extern u8 pluse_obtained_tim5;
extern u16 pluse_width_tim3_ch3;
extern u16 pluse_width_tim3_ch4;
extern u8 pluse_obtained_tim3;
extern u16 pluse_width_tim8_ch1;
extern u16 pluse_width_tim8_ch2;
extern u8 pluse_obtained_tim8;
static int usrcmd_r9b(int argc, char **argv)
{
	TIM5_Cap_Init(0xFFFF,72-1); //1MHz
	TIM3_Cap_Init(0xFFFF,72-1); //1MHz
	TIM8_Cap_Init(0xFFFF,72-1);

	//if PG9 is connected to GND, exit from the loop
	GPIOG->CRH&=0xffffff0f;
	GPIOG->CRH|=0x00000080;	//PG9 input with pull-up/pull-down
	//GPIOG->ODR&=~(1<<9);		//PG9 pull-down
	GPIOG->ODR|=1<<9;		//PG9 pull-up
	RCC->APB2ENR|=1<<8;    	//enable PORTG clock
	
	while(PGin(9)){
		//if(((pluse_obtained_tim5&0x03)==3)&&((pluse_obtained_tim3&0x03)==3)){
			xprintf("TA5_CH1 pluse width is %d\r\n", pluse_width_tim5_ch1);
			xprintf("TA5_CH2 pluse width is %d\r\n", pluse_width_tim5_ch2);
			xprintf("TA3_CH3 pluse width is %d\r\n", pluse_width_tim3_ch3);
			xprintf("TA3_CH4 pluse width is %d\r\n\r\n", pluse_width_tim3_ch4);
			xprintf("TA8_CH1 pluse width is %d\r\n", pluse_width_tim8_ch1);
			xprintf("TA8_CH2 pluse width is %d\r\n\r\n", pluse_width_tim8_ch2);
		
			pluse_obtained_tim5=0;
			pluse_obtained_tim3=0;
		  pluse_obtained_tim8=0;
			delay_ms(500);
		//}
	}
	
	return 0;
}

static int usrcmd_th9x(int argc, char **argv)
{
	u8 i=0;
	u8 drive_mode;
	u8 can_tx_buf[8];
	u32 id;
	
	u16 temp_pwm=5000; //set pwm limit: 0~5000
	int16_t motor_speed[3];
	
	float rotate_speed;
	float vx, vy;
	
	int16_t rotate_pwm;
	int16_t vx_pwm, vy_pwm;
	int16_t brightness;
	
	if (argc != 2) {
		xprintf("%s speed/pwm\r\n", argv[0]);
		return 1;
	}
	
	if(ntlibc_strcmp(argv[1],"speed")==0){
		drive_mode=0x33;
	}else if(ntlibc_strcmp(argv[1],"pwm")==0){
		drive_mode=0x22;
	}else{
		xprintf("%s is illegal. Please input speed or pwm\r\n",argv[1]);
		return 1;
	}
	
	// if mode_flag is not set.
	if(((mode_flag&(1<<(i+1)))==0) && ((mode_flag&0x01)==0)){
		xprintf("set motor mode first.\r\n");
		return 1;
	}
	
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

/**
 * @brief infoコマンド。
 * @details
 *
 * @param argc 引数の数。
 * @param argv 引数。
 *
 * @retval 0 成功。
 * @retval !0 失敗。
 */

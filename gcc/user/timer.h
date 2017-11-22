#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"

#define LED2_PWM_VAL TIM8->CCR1

void TIM1_PWM_Init(u16 arr,u16 psc); 

void TIM8_PWM_Init(u16 arr,u16 psc); 

void TIM3_Int_Init(u16 arr,u16 psc);

void TIM3_PWM_Init(u16 arr,u16 psc);

void TIM3_Cap_Init(u16 arr,u16 psc);

void TIM5_Cap_Init(u16 arr,u16 psc);

void TIM8_Cap_Init(u16 arr,u16 psc);
#endif
























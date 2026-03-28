#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init(void)
{
	PWM_Init();
}


//0		500
//180		2500
void Servo_SetAngle(uint16_t angle)
{
	if (angle > 180)
		angle=180;
	PWM_SetCompare2(500+angle*2000/180);
}

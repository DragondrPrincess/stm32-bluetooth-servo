#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Key.h"
#include "USART.h"

/* ================= 全局变量 ================= */

uint8_t KeyNum;

volatile uint16_t rx_angle = 0;        // 串口正在接收的角度（临时储存角度）
volatile uint16_t rx_angle_ok = 0;     // 已确认完成的一帧角度
volatile uint16_t servo_angle = 90;    // 当前舵机角度

volatile uint8_t USART_Flag = 0;       // 串口接收完成标志
volatile uint8_t rx_has_digit = 0;     // 是否接收到过数字（没有数字就结束）
volatile uint8_t rx_negative = 0;      // 是否检测到负号

/* ================= 主函数 ================= */

int main(void)
{
    OLED_Init();
    Servo_Init();
    Key_Init();
    USART1_Init();

    /* 初始角度 */
    Servo_SetAngle(servo_angle);

    OLED_ShowString(1,1,"Angle:");
    OLED_ShowNum(1,7,servo_angle,3);
    OLED_ShowString(2,1,"RX:");

    while (1)
    {
        /* ---------- 按键控制 ---------- */
        KeyNum = Key_GetNum();
        if (KeyNum == 1)
        {
            servo_angle += 30;
            if (servo_angle > 180)
                servo_angle = 0;

            Servo_SetAngle(servo_angle);    // 更新舵机角度
            OLED_ShowNum(1,7,servo_angle,3);
        }

        /* ---------- 串口控制 ---------- */
        if (USART_Flag)
        {
            USART_Flag = 0;

            servo_angle = rx_angle_ok;
            Servo_SetAngle(servo_angle);

            OLED_ShowNum(1,7,servo_angle,3);
            OLED_ShowNum(2,4,servo_angle,3);
        }
    }
}

/* ================= 串口中断 ================= */
void USART1_IRQHandler(void)   // 当USART收到数据后执行
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        uint8_t ch = USART_ReceiveData(USART1);

        /* ---------- 负号检测 ---------- */
        if (ch == '-')
        {
            rx_negative = 1;   // 标记非法（负角度）
        }
        /* ---------- 数字累积 ---------- */
        else if (ch >= '0' && ch <= '9')
        {
            rx_has_digit = 1;
            rx_angle = rx_angle * 10 + (ch - '0');
        }
        /* ---------- 帧结束 ----------- */
        else if (ch == '\r' || ch == '\n')
        {
            /* 合法条件：有数字 + 非负 + 范围合法 */
            if (rx_has_digit && !rx_negative && rx_angle <= 180)
            {
                rx_angle_ok = rx_angle;
                USART_Flag = 1;
            }

            /* 清空状态，准备下一帧 */
            rx_angle = 0;
            rx_has_digit = 0;
            rx_negative = 0;
        }
    }
}

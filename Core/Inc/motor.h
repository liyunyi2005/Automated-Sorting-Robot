
#ifndef INC_MOTOR_H_
#define INC_MOTOR_H_

#include "stm32f4xx_hal.h"

extern float current_angle;  // 当前角度

void Encoder_UpdateAngle(void);
float Encoder_GetCurrentAngle(void);
void Encoder_ResetAngle(void);
// 函数声明
void Motor_SetTargetSpeed(uint8_t motor_id, float target);  // 设置目标转速
void Motor_PIDCalc(void);                                   // PID计算（每10ms调用）
void Motor_UpdatePWM(void);                                 // 更新PWM输出
void Car_Control(int16_t speed, float turn);                // 小车运动控制（速度+转向）
void Test_PWM_Output(void);
#endif /* INC_MOTOR_H_ */



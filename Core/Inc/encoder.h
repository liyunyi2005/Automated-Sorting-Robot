#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "stm32f4xx_hal.h"

// 外部变量声明
extern float motor_speed[2];

// 电机编号定义
#define LEFT_MOTOR  0
#define RIGHT_MOTOR 1

// 函数声明
void Encoder_Init(void);               // 初始化编码器
void Encoder_CalcSpeed(void);          // 计算电机转速（每10ms调用）

#endif

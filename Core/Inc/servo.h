#ifndef INC_SERVO_H_
#define INC_SERVO_H_

#include "stm32f4xx_hal.h"

// 舵机枚举（对应TIM1的3个通道）
typedef enum {
  SERVO_GRIPPER = 0,   // 控制架子开闭（TIM1_CH1，PE9）
  SERVO_STRUCTURE = 1, // 控制结构开闭（TIM1_CH2，PE11）
  SERVO_ARM = 2        // 原机械臂舵机（TIM1_CH3，PE13）
} Servo_TypeDef;

// 函数声明
void Servo_Init(void);                  // 舵机初始化
void Servo_SetAngle(Servo_TypeDef id, uint8_t angle);  // 设置舵机角度（0-180°）


#endif /* INC_SERVO_H_ */

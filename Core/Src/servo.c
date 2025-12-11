
#include "servo.h"
#include "tim.h"

// 角度转定时器计数单位（0°→50，180°→250，对应500us~2500us）
#define ANGLE_TO_TIM_CNT(angle) (50 + (angle) * 200 / 180)
// 限制角度范围（防止超出舵机物理极限）
#define LIMIT_ANGLE(angle) (angle < 0 ? 0 : (angle > 180 ? 180 : angle))

// 初始化舵机PWM（TIM1_CH1~CH3）
void Servo_Init(void) {
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 架子舵机（PE9）
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2); // 结构舵机（PE11）
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3); // 机械臂舵机（PE13）

  // 初始角度设为中位（90°）
  Servo_SetAngle(SERVO_GRIPPER, 90);
  Servo_SetAngle(SERVO_STRUCTURE, 90);
  Servo_SetAngle(SERVO_ARM, 90);
}

// 设置舵机角度
void Servo_SetAngle(Servo_TypeDef id, uint8_t angle) {
	// 1. 限制角度范围
	uint8_t valid_angle = LIMIT_ANGLE(angle);
	// 2. 转换为定时器计数单位
	uint16_t tim_cnt = ANGLE_TO_TIM_CNT(valid_angle);

  switch (id) {
    case SERVO_GRIPPER:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, tim_cnt);
      break;
    case SERVO_STRUCTURE:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_2, tim_cnt);
      break;
    case SERVO_ARM:
      __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_3, tim_cnt);
      break;
  }
}




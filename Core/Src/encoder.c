#include "encoder.h"
#include "tim.h"

// 编码器全局变量
float motor_speed[2] = {0};           // 转速（rpm）

// 静态变量（只在当前文件使用）
static int32_t encoder_last[2] = {0};      // 上一次计数

void Encoder_Init(void) {
  // 启动TIM2编码器接口（左电机）
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);

  // 启动TIM4编码器接口（右电机）
  HAL_TIM_Encoder_Start(&htim4, TIM_CHANNEL_ALL);

  // 重置计数器到中间值，避免立即溢出
  __HAL_TIM_SET_COUNTER(&htim2, 32768);
  __HAL_TIM_SET_COUNTER(&htim4, 32768);

  // 记录初始计数
  encoder_last[LEFT_MOTOR] = __HAL_TIM_GET_COUNTER(&htim2);
  encoder_last[RIGHT_MOTOR] = __HAL_TIM_GET_COUNTER(&htim4);

  // 初始化速度数组
  motor_speed[LEFT_MOTOR] = 0;
  motor_speed[RIGHT_MOTOR] = 0;
}

void Encoder_CalcSpeed(void) {
	  float pulse_per_round = 11.0f;  // 每圈脉冲数

  // 左电机速度计算 (TIM2)
  int32_t current_left = __HAL_TIM_GET_COUNTER(&htim2);
  int32_t diff_left = current_left - encoder_last[LEFT_MOTOR];

  if(diff_left<-32768)diff_left+=65536;
  else if(diff_left > 32768) diff_left -= 65536;

  // 计算转速：RPM = (脉冲差 / 时间) × (60秒/分钟) / (每圈脉冲数)
  motor_speed[LEFT_MOTOR] = (float)diff_left /pulse_per_round;
  encoder_last[LEFT_MOTOR] = current_left;

  // 右电机速度计算 (TIM4)
  int32_t current_right = __HAL_TIM_GET_COUNTER(&htim4);
  int32_t diff_right = current_right - encoder_last[RIGHT_MOTOR];

  if(diff_right<-32768)diff_right+=65536;
  else if(diff_right > 32768) diff_right -= 65536;
  // 计算转速：RPM = (脉冲差 / 时间) × (60秒/分钟) / (每圈脉冲数)
  motor_speed[RIGHT_MOTOR] = (float)diff_right /pulse_per_round;
  encoder_last[RIGHT_MOTOR] = current_right;

}


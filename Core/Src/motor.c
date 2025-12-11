
#include "motor.h"
#include "tim.h"
#include "encoder.h"
#include "gpio.h"
#include <math.h>
// PID结构体（左电机=0，右电机=1）
typedef struct {
  float target;       // 目标转速（rpm）
  float kp;           // 比例系数
  float ki;           // 积分系数
  float kd;           // 微分系数
  float error;        // 当前误差
  float last_error;   // 上一次误差
  float integral;     // 积分项
  float output;       // PID输出（PWM）
} PID_HandleTypeDef;

// 初始化PID参数（520电机适配）
PID_HandleTypeDef pid[2] = {
  {0, 2.f, 0.f, 0.01f, 0, 0, 0, 0},  // 左电机
  {0, 2.f, 0.f, 0.01f, 0, 0, 0, 0}   // 右电机
};

void Motor_SetTargetSpeed(uint8_t motor_id, float target) {
    if (motor_id < 2) {
        pid[motor_id].target = target;
        // 重置积分和上次误差（防止目标突变导致积分饱和）
        pid[motor_id].integral = 0.0f;
        pid[motor_id].last_error = 0.0f;
    }
}

void Motor_PIDCalc(void) {
  for(int i=0; i<2; i++) {
    pid[i].error = pid[i].target - motor_speed[i]*0.1;  // 计算误差

    // 积分项（限幅）
    pid[i].integral += pid[i].error * 0.01f;  // 控制周期0.01s
    if(pid[i].integral > 300) pid[i].integral = 300;
    else if(pid[i].integral < -300) pid[i].integral = -300;

    // 微分项
    float derivative = (pid[i].error - pid[i].last_error) / 0.01f;

    // PID输出
    pid[i].output = pid[i].kp * pid[i].error + pid[i].ki * pid[i].integral + pid[i].kd * derivative;

    // 输出限幅（0~2000）
    if(pid[i].output > 2000) pid[i].output = 2000;
    else if(pid[i].output < -2000) pid[i].output = -2000;

    pid[i].last_error = pid[i].error;  // 保存误差
  }
}
void Motor_UpdatePWM(void) {
    // 左电机（PWM=PC6/TIM3_CH1，方向=PA0）
    if (pid[0].output > 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_SET);  // 正转
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint16_t)pid[0].output);
    } else if (pid[0].output < 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0, GPIO_PIN_RESET); // 反转
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint16_t)-pid[0].output);
    } else {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);      // 停止
    }

    // 右电机（PWM=PC7/TIM3_CH2，方向=PA1）
    if (pid[1].output > 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_SET);  // 正转
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (uint16_t)pid[1].output);
    } else if (pid[1].output < 0) {
        HAL_GPIO_WritePin(GPIOA, GPIO_PIN_1, GPIO_PIN_RESET); // 反转
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, (uint16_t)-pid[1].output);
    } else {
        __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, 0);      // 停止
    }
}

// 小车运动控制（差速转向）
// speed: 前进/后退速度（-500~500rpm）
// turn: 转向偏移（-1~1，负左偏，正右偏）
void Car_Control(int16_t speed, float turn) {
  int16_t left_target = speed * (1 + turn);
  int16_t right_target = speed * (1 - turn);

  // 限制最大转速
  left_target = (left_target > 1000) ? 1000 : (left_target < -1000 ? -1000 : left_target);
  right_target = (right_target > 1000) ? 1000 : (right_target < -1000 ? -1000 : right_target);

  Motor_SetTargetSpeed(0, left_target);  // 左电机
  Motor_SetTargetSpeed(1, right_target); // 右电机
}

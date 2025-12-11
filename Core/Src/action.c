#include "action.h"
#include "motor.h"
#include "main.h"
#include "comm.h"  // 引入通信模块
#include "servo.h"
#include <math.h>

static int16_t straight_speed = 300;
static int16_t turn_speed = 200;

void Action_SetSpeed(int16_t s_speed, int16_t t_speed) {
  straight_speed = s_speed;
  turn_speed = t_speed;
}

// 动作执行延时（带PID维持）
static void Action_Delay(uint32_t ms) {
  uint32_t start = HAL_GetTick();
  while (HAL_GetTick() - start < ms) {
    Motor_PIDCalc();
    Motor_UpdatePWM();
    HAL_Delay(1);
  }
}

// 执行动作后，自动调用Comm_SendDone()回复
void Action_Execute(Action_TypeDef action) {
  Car_Control(0, 0);
  HAL_Delay(500);  // 先停止

  switch(action) {
    case GO_STRAIGHT:
      Car_Control(straight_speed, 0);
      Action_Delay(1000);
      break;
    case TURN_LEFT_45:
      Car_Control(turn_speed, -0.5f);
      Action_Delay(300);
      break;
    case TURN_LEFT_90:
      Car_Control(turn_speed, -0.5f);
      Action_Delay(600);
      break;
    case TURN_RIGHT_45:
      Car_Control(turn_speed, 0.5f);
      Action_Delay(300);
      break;
    case TURN_RIGHT_90:
      Car_Control(turn_speed, 0.5f);
      Action_Delay(600);
      break;
    case TURN_LEFT_135:
      Car_Control(turn_speed, -0.5f);
      Action_Delay(900);  // 135度 = 90度 × 1.5倍时间
      break;

    case TURN_LEFT_180:
      Car_Control(turn_speed, -0.5f);
      Action_Delay(1200); // 180度 = 90度 × 2倍时间
      break;

    case TURN_RIGHT_135:
      Car_Control(turn_speed, 0.5f);
      Action_Delay(900);  // 135度 = 90度 × 1.5倍时间
      break;

    case TURN_RIGHT_180:
      Car_Control(turn_speed, 0.5f);
      Action_Delay(1200); // 180度 = 90度 × 2倍时间
      break;
    case SPIN_IN_PLACE:
      Car_Control(0, 1.0f);
      Action_Delay(1200);
      break;

      // 新增架子开闭动作
          case GRIPPER_OPEN:
            Servo_SetAngle(SERVO_GRIPPER, 180); // 架子完全打开
            Action_Delay(500);
            break;
          case GRIPPER_CLOSE:
            Servo_SetAngle(SERVO_GRIPPER, 0);   // 架子完全关闭
            Action_Delay(500);
            break;

          // 新增结构开闭动作
          case STRUCTURE_OPEN:
            Servo_SetAngle(SERVO_STRUCTURE, 180); // 结构完全打开
            Action_Delay(500);
            break;
          case STRUCTURE_CLOSE:
            Servo_SetAngle(SERVO_STRUCTURE, 0);   // 结构完全关闭
            Action_Delay(500);
            break;
  }

  Car_Control(0, 0);  // 动作结束后停止
  Comm_SendDone();    // 发送"完成"回复
}




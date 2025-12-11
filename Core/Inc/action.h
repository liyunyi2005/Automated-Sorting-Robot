

#ifndef INC_ACTION_H_
#define INC_ACTION_H_

#include "stm32f4xx_hal.h"
#include "main.h"

// 动作枚举（方便调用）
typedef enum {
	GO_STRAIGHT,    // 0：直走
	TURN_LEFT_45,   // 1：左转45°
	TURN_LEFT_90,   // 2：左转90°
	TURN_RIGHT_45,  // 3：右转45°
	TURN_RIGHT_90,  // 4：右转90°
	SPIN_IN_PLACE,  // 5：原地转向（360°）
	GRIPPER_OPEN,   // 6：架子打开
	GRIPPER_CLOSE,  // 7：架子关闭
	STRUCTURE_OPEN, // 8：结构打开
	STRUCTURE_CLOSE, // 9：结构关闭
    TURN_LEFT_135 , //10
    TURN_LEFT_180 , //11
    TURN_RIGHT_135 ,//12
    TURN_RIGHT_180 //13
} Action_TypeDef;

// 函数声明
void Action_Execute(Action_TypeDef action);  // 执行指定动作
// 设置动作速度参数（straight_speed：直走转速；turn_speed：转向基础转速）
void Action_SetSpeed(int16_t straight_speed, int16_t turn_speed);
void Action_Update(void);

// 添加精确角度转向函数的声明
uint8_t Action_TurnToAngle(float target_angle, float tolerance, uint32_t timeout);
#endif /* INC_ACTION_H_ */

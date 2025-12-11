

#ifndef INC_TRACK_H_
#define INC_TRACK_H_

#include "stm32f4xx_hal.h"

// 外部声明全局变量
extern uint32_t track_data[8];  // 8路循迹传感器数据
extern float track_error;       // 循迹偏差（-1~1）

// 函数声明
void Track_UpdateData(uint32_t *adc_buf);  // 更新循迹数据
void Track_CalcError(void);                // 计算循迹偏差（每50ms调用）
void Track_Control(int16_t base_speed);    // 根据偏差控制小车转向
uint8_t Track_IsCross(void);
#endif /* INC_TRACK_H_ */

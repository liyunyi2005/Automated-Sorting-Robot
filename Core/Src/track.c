
#include "track.h"
#include "adc.h"
#include "motor.h"
#include <math.h>

uint32_t track_data[8] = {0};  // 8路循迹传感器数据
float track_error = 0;         // 循迹偏差（-1~1）
float track_error_update = 0;
// 更新循迹数据
void Track_UpdateData(uint32_t *adc_buf) {
  for(int i=0; i<8; i++) {
    track_data[i] = adc_buf[i];  // 从ADC缓冲区读取数据
  }
}

void Track_CalcError(void) {
  int8_t weight[8] = {-7, -5, -3, -1, 1, 3, 5, 7};  // 增强权重差异
  int32_t sum = 0, count = 0;
  int8_t active_sensors = 0;
  uint32_t threshold = 1800;  // 可能需要根据实际情况调整

  for(int i=0; i<8; i++) {
    if(track_data[i] < threshold) {
      int32_t intensity = threshold - track_data[i];
      sum += weight[i] * intensity * intensity;  // 平方增强差异
      count += intensity;
      active_sensors++;
    }
  }

  if(count > 0 && active_sensors > 0) {
    // 使用更敏感的计算方式
    track_error = (float)sum / (count * 7);

    // 缩小死区，提高灵敏度
    if(fabs(track_error) < 0.03f) track_error = 0;

    // 限幅
    if(track_error > 1.0f) track_error = 1.0f;
    else if(track_error < -1.0f) track_error = -1.0f;
  }
  else {
    // 丢失路线处理策略
    static uint8_t lost_count = 0;
    lost_count++;
    if(lost_count > 5) {
      // 按上次偏差方向进行搜索转向
      track_error = (track_error > 0) ? 0.9f : -0.9f;

    }
  }
  track_error_update = track_error;
}
// 根据循迹偏差自动控制小车转向
void Track_Control(int16_t base_speed) {
  float abs_error = fabs(track_error_update);
  float k_turn;

  // 自适应转向系数 - 根据偏差大小动态调整
  if(abs_error > 0.8f) {
    k_turn = 2.5f;  // 大偏差时强力转向
    base_speed *= 0.6f;  // 大转弯时减速
  }
  else if(abs_error > 0.5f) {
    k_turn = 1.8f;  // 中等偏差
    base_speed *= 0.8f;
  }
  else if(abs_error > 0.3f) {
    k_turn = 1.2f;  // 小偏差
  }
  else {
    k_turn = 0.8f;  // 微小偏差，柔和转向
  }

  float turn_amount = track_error_update * k_turn;

  // 转向限幅
  if(turn_amount > 1.0f) turn_amount = 1.0f;
  else if(turn_amount < -1.0f) turn_amount = -1.0f;

  Car_Control(base_speed, turn_amount);
}
uint8_t Track_IsCross(void) {
  uint32_t threshold = 1800; // 根据实际情况调整
  uint8_t black_count = 0;

  for (int i = 0; i < 8; i++) {
    // 如果你的黑线电压更高，请改为 >
    if (track_data[i] < threshold) black_count++;
  }

  // 超过 6 个认为是十字路口（容忍波动）
  return (black_count >= 6);
}



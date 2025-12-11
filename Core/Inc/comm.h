#ifndef INC_COMM_H_
#define INC_COMM_H_

#include "stm32f4xx_hal.h"

extern uint8_t rx_buf[50];
extern uint8_t rx_len;

void Comm_ParseCmd(void);
void Comm_SendDone(void);  // 新增：发送动作完成回复
void Comm_RxCallback(uint8_t data);



#endif /* INC_COMM_H_ */

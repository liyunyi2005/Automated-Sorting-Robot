
#include "comm.h"
#include "usart.h"
#include "action.h"
#include <string.h>
#include <stdio.h>

uint8_t rx_buf[50] = {0};
uint8_t rx_len = 0;
uint8_t current_action = 255;

void Comm_ParseCmd(void) {
  if (strstr((char*)rx_buf, "ACTION:")) {
    int action_id;
    if (sscanf((char*)rx_buf, "ACTION:%d", &action_id) == 1
        && action_id >= 0 && action_id <= 13) { // 动作ID扩展到10
      current_action = action_id;
      Action_Execute((Action_TypeDef)action_id);
    }
  }

  rx_len = 0;
  memset(rx_buf, 0, 50);
}

void Comm_SendDone(void) {
  if (current_action != 255) {
    char done_buf[20];
    sprintf(done_buf, "DONE:%d\n", current_action);
    HAL_UART_Transmit(&huart2, (uint8_t*)done_buf, strlen(done_buf), 100);
    current_action = 255;
  }
}

void Comm_RxCallback(uint8_t data) {
  if (rx_len < 49) {
    rx_buf[rx_len++] = data;
    if (data == '\n') Comm_ParseCmd();
  } else {
    rx_len = 0;
    memset(rx_buf, 0, 50);
  }
}



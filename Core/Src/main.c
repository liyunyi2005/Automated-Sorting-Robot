/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "servo.h"
#include "encoder.h"
#include "motor.h"
#include "action.h"
#include "track.h"
#include "comm.h"
#include "string.h"
#include "stdio.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint32_t sys_tick = 0;
uint32_t Track_count =0;
uint32_t adc_date[8]={0};
extern uint8_t rx_buf[50];
extern uint8_t rx_len;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC2_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  Encoder_Init();
  Servo_Init();
  HAL_ADC_Start_DMA(&hadc2, adc_date, 8);
  HAL_UART_Receive_IT(&huart2, &rx_buf[rx_len], 1);
  HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_ALL);
  static uint8_t track_tick = 0;

  Track_Control(300);


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // 6.1 10ms定时任务（核心控制周期）
    if (sys_tick == 40) {
      Encoder_CalcSpeed(); // 计算电机实时转速

      // 循迹处理（每40ms执行一次）
      if (track_tick >= 4) { // 40ms = 4 * 10ms
        Track_UpdateData(adc_date);
        Track_CalcError();

        // ===== 检测是否为十字路口 =====
        if (Track_IsCross()) {
          Car_Control(0, 0);   // 停止小车
          HAL_Delay(500);      // 停止0.5秒（可调）
        }
        // =============================

        Track_Control(200);    // 设置基础速度，比如200rpm
        track_tick = 0;
      }

      track_tick++;
      Motor_PIDCalc();         // 执行PID算法，计算PWM输出
      Motor_UpdatePWM();       // 更新电机PWM，控制转速
      sys_tick = 0;


    }
    /* USER CODE END 3 */
    // main函数尾部循环结束
        HAL_Delay(1);
        sys_tick++;
        Track_count++;
    } // main函数结束
}
    /*/* USER CODE BEGIN 4 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {  // 使用USART2与树莓派通信
    Comm_RxCallback(rx_buf[rx_len]);
    // 重新开启接收中断
    HAL_UART_Receive_IT(&huart2, &rx_buf[rx_len], 1);
  }
}

// 也可以添加错误处理
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
  if (huart->Instance == USART2) {
    // 重新启动接收
    HAL_UART_Receive_IT(&huart2, &rx_buf[rx_len], 1);
  }
}
    /**
      * @brief System Clock Configuration
      * @retval None
      */
    void SystemClock_Config(void)
    {
        RCC_OscInitTypeDef RCC_OscInitStruct = {0};
        RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

        /* 配置主内部稳压器输出电压 */
        __HAL_RCC_PWR_CLK_ENABLE();
        __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

        /* 初始化 RCC 振荡器 */
        RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
        RCC_OscInitStruct.HSEState = RCC_HSE_ON;
        RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
        RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
        RCC_OscInitStruct.PLL.PLLM = 4;
        RCC_OscInitStruct.PLL.PLLN = 168;
        RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
        RCC_OscInitStruct.PLL.PLLQ = 4;
        if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        /* 初始化 CPU、AHB 和 APB 总线时钟 */
        RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                                    | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
        RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
        RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
        RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
        RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
        if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
        {
            Error_Handler();
        }
    }

    /**
      * @brief  错误处理函数
      * @retval None
      */
  void Error_Handler(void)
    {
        /* 禁用中断，进入死循环 */
        __disable_irq();
        while (1)
        {
        }
   }

    #ifdef USE_FULL_ASSERT
    /**
      * @brief  报告断言失败信息
      * @param  file: 发生断言的源文件
      * @param  line: 断言所在行号
      * @retval None
      */
    void assert_failed(uint8_t *file, uint32_t line)
    {
        /* 用户可以添加打印调试信息，例如：
           printf("Wrong parameters value: file %s on line %d\r\n", file, line); */
    }
    #endif /* USE_FULL_ASSERT */

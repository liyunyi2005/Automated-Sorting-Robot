# Automated-Sorting-Robot

基于树莓派视觉识别与 STM32F407 控制的分拣小车。树莓派负责颜色识别并通过串口下发动作，STM32 负责底盘循迹、双电机 PID 驱动及三路舵机（夹爪/结构/机械臂）控制，实现不同颜色物块的抓取和分拣。

## 硬件概要
- 主控：STM32F407ZET6（CubeMX/HAL 工程）。
- 底盘：双直流电机 + 编码器（TIM2/TIM4 采集），PWM 驱动在 TIM3_CH1/CH2，方向由 PA0/PA1 控制。
- 舵机：3 路 PWM（TIM1_CH1/2/3 -> PE9/PE11/PE13），分别控制夹爪、结构、机械臂。
- 循迹：8 路反射式循迹传感器，经 ADC2 + DMA 采样。
- 通信：UART2（PA2/PA3，115200 8N1）与树莓派交互；UART1 预留调试。

## 目录结构
- `Core/Inc`, `Core/Src`：用户代码与外设配置（CubeMX 生成 + 手写逻辑）。
- `Core/Startup`：启动文件 `startup_stm32f407zetx.s`。
- `Drivers/`：CMSIS 与 STM32F4 HAL/LL 库。
- `Debug/`：编译产物与生成的 makefile/对象列表（CubeIDE 默认输出）。
- 工程文件：`car.ioc`（CubeMX 配置）、`.cproject/.project/.mxproject`（CubeIDE/Eclipse 工程）、链接脚本 `STM32F407ZETX_FLASH.ld`/`STM32F407ZETX_RAM.ld`。

## 关键功能
- 底盘控制：`motor.c` 实现双电机 PID（目标速度设定、PWM 更新），`encoder.c` 读取编码器并计算转速，`Car_Control` 支持差速转向。
- 循迹算法：`track.c` 读取 8 路 ADC，计算偏差（-1 ~ 1），自适应转向增益，检测十字路口时主动停车（默认停 0.5 s）。
- 动作编排：`action.c` 定义基础动作（直行、45°/90°/135°/180° 左右转、原地旋转、夹爪/结构开合），动作结束后自动回传完成状态。
- 舵机控制：`servo.c` 提供 0~180° 角度设定，启动时居中。
- 通信协议：`comm.c` 解析串口指令并触发动作，完成后回传。

## 串口协议（树莓派 ⇄ STM32，UART2 115200）
- 下发：`ACTION:<id>\n`  
  - 0 GO_STRAIGHT，1 TURN_LEFT_45，2 TURN_LEFT_90，3 TURN_RIGHT_45，4 TURN_RIGHT_90，  
    5 SPIN_IN_PLACE，6 GRIPPER_OPEN，7 GRIPPER_CLOSE，8 STRUCTURE_OPEN，9 STRUCTURE_CLOSE，  
    10 TURN_LEFT_135，11 TURN_LEFT_180，12 TURN_RIGHT_135，13 TURN_RIGHT_180
- 回传：`DONE:<id>\n`（动作完成后由 STM32 主动发送）

## 运行流程（固件侧）
1. 上电后初始化 GPIO/ADC/DMA/TIM/UART、启动编码器与舵机、开启 ADC DMA 采样与串口中断。
2. 主循环以 10 ms 节拍运行 PID，40 ms 执行一次循迹偏差计算与调节（默认基准速度 200）。
3. 接收到 `ACTION` 指令后按预设速度/时间执行动作，动作结束即发送 `DONE`。

## 构建与烧录
- 推荐：STM32CubeIDE 打开工程（或用 `car.ioc` 重新生成后构建），选择 `Debug`/`Release` 配置编译并下载。
- 命令行：使用生成的 makefile  
  ```sh
  make -C Debug
  ```

## 调试与参数
- 循迹阈值 `threshold`、基准速度等可在 `track.c` 调节（默认阈值 1800）。
- 电机 PID 系数与输出限幅在 `motor.c`，需按实际电机/驱动调试。
- 舵机开/合角度在 `action.c` 中预设，可根据机构行程修改。

## 后续改进
- 实现 `Action_TurnToAngle` 等闭环角度动作（结合编码器或陀螺仪）。
- 上电自校准循迹阈值与增益，提高不同地面/光照下的鲁棒性。

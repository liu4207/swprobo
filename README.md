# swprobo 🚗
STM32 + FreeRTOS 差速小车底盘驱动系统，支持 ROS2 上位机 `cmd_vel` 控制，IMU + 编码器数据融合。

## 🧩 项目特点
- ✅ 支持 ROS2 接收速度指令（线速度+角速度）
- ✅ 支持 IMU 姿态解算（ATK-MS601）
- ✅ 支持编码器 PID 控速 + 速度估算
- ✅ FreeRTOS 多任务调度架构
- ✅ UART 协议通信解耦设计
- ✅ 支持 Keil 工程一键编译

## 📂 工程结构
| 文件夹       | 说明 |
|--------------|------|
| Src/         | 主业务逻辑代码，如 UART、任务调度 |
| Inc/         | 头文件定义 |
| Drivers/     | STM32 HAL 驱动 |
| MDK-ARM/     | Keil 工程文件和外设驱动文件 |
| Middlewares/ | FreeRTOS 核心 |
| test_lsk.ioc | CubeMX 工程文件 |

## 🛠️ 开发环境
- 开发板：STM32F407VGT6
- IDE：Keil MDK-ARM 5.38a
- 工程文件：位于 `MDK-ARM/` 文件夹，双击 `.uvprojx` 即可打开
- 编译工具链：ARMCC
- 系统框架：FreeRTOS + HAL
- 项目生成工具：STM32CubeMX（对应配置文件为 `test_lsk.ioc`）

## 🚗 项目亮点总结

- ✅ 手写 UART 解码函数，支持 ROS2 风格的 `cmd_vel` 指令解析
- ✅ 实现 IMU 姿态解算 + UART 串口稳定发送
- ✅ 支持双轮编码器速度反馈，构建完整闭环控制系统
- ✅ FreeRTOS 多任务结构：IMU任务、电机任务、命令解析任务
- ✅ PID 调速逻辑结合死区控制，保证低速精度
- ✅ 可用于小车导航定位、轨迹跟踪等任务，未来支持扩展3D雷达、深度相机

## 🚀 快速开始
1. 打开 `test_lsk.ioc` 使用 STM32CubeMX，点击 `Generate Code`
2. 打开 `MDK-ARM/swprobo.uvprojx` 使用 Keil 编译下载（记得注释掉freertos自动生成的函数 否则会重复定义）
3. 上位机通过串口发送形如 `#V+0.22,A-0.13\n` 指令进行控制（底盘代码协议是这个 可以自己手动改）

## 🧑‍💻 树莓派代码部分
请见 [raspi 分支](https://github.com/liu4207/swprobo/tree/raspi)
- **接收** ROS2 上位机的 `/cmd_vel` 控制指令。
- **发送** 到 STM32 底盘控制系统。
- **发布** IMU 和里程计数据，供上位机进行导航和定位。

## 🖥️ 串口协议（树莓派发下来的）
格式：#V+线速度,A+角速度\n
例如：#V+0.20,A-0.10\n

## 📸 效果展示
> （可放图 / 视频 / GIF / ROS 上位机截图）

## 上位机部分
上位机部分代码位于 [ros2_host 分支](https://github.com/liu4207/swprobo/tree/ros2_host)

### 🧩 查看硬件设计文件

本项目硬件部分请见 [hardware 分支](https://github.com/liu4207/swprobo/tree/hardware)，包含：

- 原理图（PDF）
- PCB（PDF）
- Gerber 文件（可生产）
- BOM / 坐标文件
  
## 👨‍💻 作者
刘珅凯 @ 重庆大学明月班  
联系：2323453274@qq.com

## 📄 License
MIT License

Copyright (c) 2025 Liu Shenkai

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

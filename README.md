## 🧭 项目结构说明

本项目包含树莓派上运行的 ROS 2 节点，用于：

- ✅ 订阅 ROS 2 `/cmd_vel` 控制指令，并通过串口发送给 STM32 底盘
- ✅ 发布 `/imu` 和 `/odom` 数据供导航/可视化使用
- ✅ 驱动 YDLidar X3 激光雷达并发布 `/scan` 数据用于 SLAM

---

## 📦 节点功能说明

| 脚本文件            | 功能简介 |
|---------------------|----------|
| `server.py`         | 串口读取 IMU/编码器，发布 `/imu`, `/odom` |
| `manual_control.py` | 用键盘控制发送 `cmd_vel` |
| `server2.py`        | 串口协议优化版本，适配新版底盘控制指令 |
| `serverlsk3.py`     | 综合发布+接收版本：订阅 `/cmd_vel`，串口通信控制底盘，并发布传感器话题 |

---
## 📦 依赖组件

- [YDLidar ROS2 驱动](https://github.com/YDLIDAR/ydlidar_ros2_driver)

安装方式：
```bash
cd ~/ros2_ws/src
git clone https://github.com/YDLIDAR/ydlidar_ros2_driver.git

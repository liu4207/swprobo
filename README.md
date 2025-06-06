# ROS2 Serial Bridge

本项目提供了在 Ubuntu 22.04 上使用 ROS 2 Humble 进行 SLAM 和导航的完整配置 

## 🧰 前置准备

### ✅ 系统要求：
- 操作系统：Ubuntu 22.04
- ROS 2 版本：Humble Hawksbill

### ✅ 依赖安装：
安装 ROS 2 Humble，推荐使用小鱼安装器：
```bash
wget http://fishros.com/install -O fishros && . fishros
```
安装额外依赖:
```bash
sudo apt update && sudo apt install -y \
    python3-colcon-common-extensions \
    ros-humble-navigation2 \
    ros-humble-nav2-bringup \
    ros-humble-slam-toolbox \
    ros-humble-turtlebot3*  # 可选，适用于仿真
```
🧱 步骤：安装和使用
1️⃣ 克隆仓库并编译：
```bash
git clone https://github.com/liu4207/swprobo.git
cd swprobo
colcon build --symlink-install
```
2️⃣ 设置环境变量：
```bash
source install/setup.bash
```
建议将其添加到 ~/.bashrc

🗺️ 第三步：启动 SLAM 建图
运行以下命令开始 SLAM 建图，并在 RViz 中可视化：
```bash
source install/setup.bash
ros2 launch serial_bridge slam_launch.py
```

启动键盘遥控控制机器人
```bash
ros2 run serial_bridge wasdx_teleop_launch.py
```

保存建图的地图文件：
```bash
ros2 run nav2_map_server map_saver_cli -f ~/swprobo/src/serial_bridge/maps/<map_name>
```

第四步：启动导航模块
运行以下命令启动导航模块，并在 RViz 中查看路径规划：
```bash
ros2 launch serial_bridge nav_with_map_launch.py
```
如果你有自定义的地图文件，可以这样启动：
```bash
ros2 launch serial_bridge nav_with_map_launch.py map:=/path/to/save/map.yaml
```

🚀 第五步：边建图边导航（SLAM + Navigation）
若希望在建图的同时执行导航，请运行：
```bash
ros2 launch serial_bridge nav_with_slam.launch.py
```
在导航过程中，你可以随时保存当前地图：
```bash
ros2 run nav2_map_server map_saver_cli -f ~/swprobo/src/serial_bridge/maps/<map_name>
```
📝 附加说明
请确保你的机器人运行了指定的 STM32 固件代码，并且连接了合适的底层硬件。
机器人需搭载兼容的 LiDAR 雷达（如 YDLidar），并正确连接至树莓派。
请确保树莓派和 PC 处于同一局域网内，以便进行数据传输和控制。
在 ~/.bashrc 文件中，添加以下行以确保正确的 ROS 域 ID 设置：
```bash
export ROS_DOMAIN_ID=0
```
或者通过命令设置：
```bash
echo "export ROS_DOMAIN_ID=0" >> ~/.bashrc
source ~/.bashrc
```

版权声明： 参考了我同学的课程设计，部分代码由对方完成！！！



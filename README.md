🚀 快速开始指南
🧰 前置准备
✅ 系统要求：
操作系统：Ubuntu 22.04

ROS 2 版本：Humble

✅ 依赖安装：
安装 ROS 2 Humble：

推荐使用小鱼安装器，详细教程请参考 中文安装教程。

执行以下命令来安装：

bash
复制
wget http://fishros.com/install -O fishros && . fishros
安装额外依赖：

bash
复制
sudo apt update && sudo apt install -y \
    python3-colcon-common-extensions \
    ros-humble-navigation2 \
    ros-humble-nav2-bringup \
    ros-humble-slam-toolbox \
    ros-humble-turtlebot3*  # 可选，适用于仿真
🧱 第一步：克隆与编译代码
克隆仓库：

bash
复制
git clone https://github.com/liu4207/swprobo.git
cd swprobo
使用 colcon 构建 ROS2 工作空间：

bash
复制
colcon build --symlink-install
🛠️ 第二步：设置环境变量
设置 ROS 2 环境变量以便每次启动时自动加载：

bash
复制
source install/setup.bash
为了每次都能自动生效，建议将其加入到 ~/.bashrc：

bash
复制
echo "source ~/swprobo/install/setup.bash" >> ~/.bashrc
source ~/.bashrc
🗺️ 第三步：启动 SLAM 建图
运行以下命令开始 SLAM 建图，并在 RViz 中可视化：

bash
复制
source install/setup.bash
ros2 launch serial_bridge slam_launch.py
启动键盘遥控控制机器人：

bash
复制
ros2 run serial_bridge wasdx_teleop_launch.py
保存建图的地图文件：

bash
复制
ros2 run nav2_map_server map_saver_cli -f ~/swprobo/src/serial_bridge/maps/<map_name>
🧭 第四步：启动导航模块
运行以下命令启动导航模块，并在 RViz 中查看路径规划：

bash
复制
ros2 launch serial_bridge nav_with_map_launch.py
如果你有自定义的地图文件，可以这样启动：

bash
复制
ros2 launch serial_bridge nav_with_map_launch.py map:=/path/to/save/map.yaml
🚀 第五步：边建图边导航（SLAM + Navigation）
若希望在建图的同时执行导航，请运行：

bash
复制
ros2 launch serial_bridge nav_with_slam.launch.py
在导航过程中，你可以随时保存当前地图：

bash
复制
ros2 run nav2_map_server map_saver_cli -f ~/swprobo/src/serial_bridge/maps/<map_name>
📝 附加说明
请确保你的机器人运行了指定的 STM32 固件代码，并且连接了合适的底层硬件。

机器人需搭载兼容的 LiDAR 雷达（如 YDLidar），并正确连接至树莓派。

请确保树莓派和 PC 处于同一局域网内，以便进行数据传输和控制。

在 ~/.bashrc 文件中，添加以下行以确保正确的 ROS 域 ID 设置：

bash
复制
export ROS_DOMAIN_ID=0
或者通过命令设置：

bash
复制
echo "export ROS_DOMAIN_ID=0" >> ~/.bashrc
source ~/.bashrc

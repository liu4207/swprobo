# # 刘珅凯写的！！！！！！！！！！！！！！
# # -*- coding: utf-8 -*-
# from flask import Flask, request, jsonify
# from flask_cors import CORS
# import serial
# import threading
# import time
# import rclpy
# from rclpy.node import Node
# from sensor_msgs.msg import Imu
# from nav_msgs.msg import Odometry
# from geometry_msgs.msg import Quaternion, TransformStamped
# from tf2_ros import TransformBroadcaster
# import re
# import os

# ###############################
# # 串口初始化（全局串口对象）
# ###############################
# def find_serial_port():
#     for path in ['/dev/serial/by-id/', '/dev']:
#         if os.path.exists(path):
#             for dev in os.listdir(path):
#                 if 'USB' in dev or 'imu' in dev:
#                     full_path = os.path.realpath(os.path.join(path, dev))
#                     print(f"[INFO] 尝试使用串口: {full_path}")
#                     return full_path
#     return '/dev/ttyUSB0'

# try:
#     ser = serial.Serial(
#         port=find_serial_port(),
#         baudrate=115200,
#         timeout=1,
#         bytesize=serial.EIGHTBITS,
#         parity=serial.PARITY_NONE,
#         stopbits=serial.STOPBITS_ONE
#     )
#     print("[INFO] 串口已打开")
# except serial.SerialException as e:
#     print(f"[ERROR] 串口打开失败: {e}")
#     ser = None

# ####################################
# # Flask 线程：接收前端指令并发串口
# ####################################
# flask_app = Flask(__name__)
# CORS(flask_app)

# @flask_app.route('/command', methods=['POST'])
# def handle_command():
#     data = request.json
#     command = data.get('command')
#     valid_commands = ['0','1','2','3','4','5','6','7','8','9','a','b','c','x','y','z']
#     if not command or str(command) not in valid_commands:
#         return jsonify({"status": "error", "message": "Invalid command"}), 400

#     print(f"[RECEIVED] 指令: {command}")
#     if ser and ser.is_open:
#         try:
#             ser.write((command + '\n').encode('utf-8'))
#             ser.flush()
#             print(f"[SENT TO STM32] 发送数据: {command}")
#         except Exception as e:
#             print(f"[ERROR] 发送失败: {e}")
#             return jsonify({"status": "error", "message": "串口发送失败"}), 500
#     else:
#         print("[ERROR] 串口未打开")
#         return jsonify({"status": "error", "message": "串口未打开"}), 500
#     return jsonify({"status": "success", "command": command})

# def run_flask():
#     flask_app.run(host='0.0.0.0', port=5001)

# ####################################
# # ROS 线程：读取串口并发布 IMU/Odom
# ####################################
# class SerialPublisher(Node):
#     def __init__(self):
#         super().__init__('fast_serial_publisher')
#         self.imu_pub = self.create_publisher(Imu, '/imu/data_raw', 10)
#         self.odom_pub = self.create_publisher(Odometry, '/odom', 10)
#         self.tf_broadcaster = TransformBroadcaster(self)
#         self.data_buffer = {'quaternion': None, 'gyro': None, 'accel': None, 'odom': None}
#         self.timer = self.create_timer(0.001, self.read_serial)

#     def read_serial(self):
#         if not ser or not ser.is_open:
#             return
#         try:
#             line = ser.readline().decode('utf-8').strip()
#             if not line:
#                 return
#             if line.startswith('Quaternion:'):
#                 match = re.match(r"Quaternion: q0=([\d\-.]+) q1=([\d\-.]+) q2=([\d\-.]+) q3=([\d\-.]+)", line)
#                 if match:
#                     self.data_buffer['quaternion'] = tuple(map(float, match.groups()))
#             elif line.startswith('Gx:'):
#                 match = re.match(r"Gx: ([\d\-.]+) Gy: ([\d\-.]+) Gz: ([\d\-.]+)", line)
#                 if match:
#                     self.data_buffer['gyro'] = tuple(map(float, match.groups()))
#             elif line.startswith('Ax:'):
#                 match = re.match(r"Ax: ([\d\-.]+)G Ay: ([\d\-.]+)G Az: ([\d\-.]+)G", line)
#                 if match:
#                     self.data_buffer['accel'] = tuple(map(lambda x: float(x) * 9.80665, match.groups()))
#             elif line.startswith('LeftDist:'):
#                 match = re.match(r"LeftDist: ([\d\-.]+) RightDist: ([\d\-.]+) LinVel: ([\d\-.]+) AngVel: ([\d\-.]+)", line)
#                 if match:
#                     self.data_buffer['odom'] = tuple(map(float, match.groups()))

#             if all(self.data_buffer.values()):
#                 self.publish_all()
#                 for key in self.data_buffer:
#                     self.data_buffer[key] = None
#         except Exception as e:
#             self.get_logger().warn(f"[串口读取错误]: {e}")

#     def publish_all(self):
#         try:
#             q = self.data_buffer['quaternion']
#             g = self.data_buffer['gyro']
#             a = self.data_buffer['accel']
#             o = self.data_buffer['odom']
#             now = self.get_clock().now().to_msg()

#             imu_msg = Imu()
#             imu_msg.header.stamp = now
#             imu_msg.header.frame_id = 'base_link'
#             imu_msg.orientation = Quaternion(x=q[1], y=q[2], z=q[3], w=q[0])
#             imu_msg.angular_velocity.x = g[0]
#             imu_msg.angular_velocity.y = g[1]
#             imu_msg.angular_velocity.z = g[2]
#             imu_msg.linear_acceleration.x = a[0]
#             imu_msg.linear_acceleration.y = a[1]
#             imu_msg.linear_acceleration.z = a[2]
#             self.imu_pub.publish(imu_msg)

#             odom_msg = Odometry()
#             odom_msg.header.stamp = now
#             odom_msg.header.frame_id = 'odom'
#             odom_msg.child_frame_id = 'base_link'
#             x = (o[0] + o[1]) / 2.0 / 100.0
#             odom_msg.pose.pose.position.x = x
#             odom_msg.pose.pose.orientation = imu_msg.orientation
#             odom_msg.twist.twist.linear.x = o[2]
#             odom_msg.twist.twist.angular.z = o[3]
#             self.odom_pub.publish(odom_msg)

#             t = TransformStamped()
#             t.header.stamp = now
#             t.header.frame_id = 'odom'
#             t.child_frame_id = 'base_link'
#             t.transform.translation.x = x
#             t.transform.translation.y = 0.0
#             t.transform.translation.z = 0.0
#             t.transform.rotation = imu_msg.orientation
#             self.tf_broadcaster.sendTransform(t)

#         except Exception as e:
#             self.get_logger().warn(f"[发布错误]: {e}")

# ####################################
# # 主函数：两个线程分别启动
# ####################################
# def main():
#     # 启动 Flask 服务线程
#     flask_thread = threading.Thread(target=run_flask, daemon=True)
#     flask_thread.start()
#     print("[INFO] Flask 已启动")

#     # 启动 ROS2 节点
#     rclpy.init()
#     try:
#         node = SerialPublisher()
#         print("[INFO] ROS2 节点启动成功，进入循环")
#         rclpy.spin(node)
#     except KeyboardInterrupt:
#         print("用户中断")
#     finally:
#         rclpy.shutdown()
#         print("ROS2 已关闭")

# if __name__ == '__main__':
#     main()
# -*- coding: utf-8 -*-
# from flask import Flask, request, jsonify
# from flask_cors import CORS
# import serial
# import threading
# import time
# import re
# import os

# import rclpy
# from rclpy.node import Node
# from sensor_msgs.msg import Imu
# from nav_msgs.msg import Odometry
# from geometry_msgs.msg import Quaternion, TransformStamped
# from tf2_ros import TransformBroadcaster

# ########################################
# # 自动查找串口
# ########################################
# def find_serial_port():
#     for path in ['/dev/serial/by-id/', '/dev']:
#         if os.path.exists(path):
#             for dev in os.listdir(path):
#                 if 'USB' in dev or 'imu' in dev or 'ttyUSB' in dev:
#                     full_path = os.path.realpath(os.path.join(path, dev))
#                     print(f"[INFO] 发现串口设备: {full_path}")
#                     return full_path
#     print("[WARN] 未找到匹配串口，默认使用 /dev/ttyUSB0")
#     return '/dev/ttyUSB0'

# try:
#     serial_path = find_serial_port()
#     ser = serial.Serial(
#         port=serial_path,
#         baudrate=115200,
#         timeout=1,
#         bytesize=serial.EIGHTBITS,
#         parity=serial.PARITY_NONE,
#         stopbits=serial.STOPBITS_ONE
#     )
#     print(f"[INFO] 串口 {serial_path} 已打开")
# except Exception as e:
#     print(f"[FATAL] 无法打开串口: {e}")
#     ser = None

########################################
# Flask Server：接收控制指令
########################################
# flask_app = Flask(__name__)
# CORS(flask_app)

# @flask_app.route('/command', methods=['POST'])
# def handle_command():
#     data = request.json
#     command = data.get('command')
#     valid_commands = list('0123456789abcxyz')

#     if not command or str(command) not in valid_commands:
#         return jsonify({"status": "error", "message": "Invalid command"}), 400

#     print(f"[RECEIVED] 指令: {command}")
#     if ser and ser.is_open:
#         try:
#             ser.write((command + '\n').encode('utf-8'))
#             ser.flush()
#             print(f"[SENT TO STM32] 发送数据: {command}")
#         except Exception as e:
#             print(f"[ERROR] 串口发送失败: {e}")
#             return jsonify({"status": "error", "message": "串口发送失败"}), 500
#     else:
#         print("[ERROR] 串口未打开")
#         return jsonify({"status": "error", "message": "串口未打开"}), 500

#     return jsonify({"status": "success", "command": command})

# def run_flask():
#     flask_app.run(host='0.0.0.0', port=5001)

# ########################################
# # ROS2 Node：读取串口数据并发布
# ########################################
# class SerialPublisher(Node):
#     def __init__(self):
#         super().__init__('fast_serial_publisher')
#         self.imu_pub = self.create_publisher(Imu, '/imu/data_raw', 10)
#         self.odom_pub = self.create_publisher(Odometry, '/odom', 10)
#         self.tf_broadcaster = TransformBroadcaster(self)
#         self.data_buffer = {'quaternion': None, 'gyro': None, 'accel': None, 'odom': None}
#         self.timer = self.create_timer(0.005, self.read_serial)  # 200Hz

#     def read_serial(self):
#         if not ser or not ser.is_open:
#             return
#         try:
#             line = ser.readline().decode('utf-8').strip()
#             if not line:
#                 return

#             # ✅ 打印串口原始数据
#             print(f"[SERIAL] {line}")

#             if line.startswith('Quaternion:'):
#                 match = re.match(r"Quaternion: q0=([\d\-.]+) q1=([\d\-.]+) q2=([\d\-.]+) q3=([\d\-.]+)", line)
#                 if match:
#                     self.data_buffer['quaternion'] = tuple(map(float, match.groups()))
#             elif line.startswith('Gx:'):
#                 match = re.match(r"Gx: ([\d\-.]+) Gy: ([\d\-.]+) Gz: ([\d\-.]+)", line)
#                 if match:
#                     self.data_buffer['gyro'] = tuple(map(float, match.groups()))
#             elif line.startswith('Ax:'):
#                 match = re.match(r"Ax: ([\d\-.]+)G Ay: ([\d\-.]+)G Az: ([\d\-.]+)G", line)
#                 if match:
#                     self.data_buffer['accel'] = tuple(map(lambda x: float(x) * 9.80665, match.groups()))
#             elif line.startswith('LeftDist:'):
#                 match = re.match(r"LeftDist: ([\d\-.]+) RightDist: ([\d\-.]+) LinVel: ([\d\-.]+) AngVel: ([\d\-.]+)", line)
#                 if match:
#                     self.data_buffer['odom'] = tuple(map(float, match.groups()))

#             # 满足全部数据后发布
#             if all(self.data_buffer.values()):
#                 self.publish_all()
#                 self.data_buffer = {k: None for k in self.data_buffer}

#         except Exception as e:
#             self.get_logger().warn(f"[串口读取错误]: {e}")

#     def publish_all(self):
#         try:
#             q = self.data_buffer['quaternion']
#             g = self.data_buffer['gyro']
#             a = self.data_buffer['accel']
#             o = self.data_buffer['odom']
#             now = self.get_clock().now().to_msg()

#             imu_msg = Imu()
#             imu_msg.header.stamp = now
#             imu_msg.header.frame_id = 'base_link'
#             imu_msg.orientation = Quaternion(x=q[1], y=q[2], z=q[3], w=q[0])
#             imu_msg.angular_velocity.x = g[0]
#             imu_msg.angular_velocity.y = g[1]
#             imu_msg.angular_velocity.z = g[2]
#             imu_msg.linear_acceleration.x = a[0]
#             imu_msg.linear_acceleration.y = a[1]
#             imu_msg.linear_acceleration.z = a[2]
#             self.imu_pub.publish(imu_msg)

#             odom_msg = Odometry()
#             odom_msg.header.stamp = now
#             odom_msg.header.frame_id = 'odom'
#             odom_msg.child_frame_id = 'base_link'
#             x = (o[0] + o[1]) / 2.0 / 100.0
#             odom_msg.pose.pose.position.x = x
#             odom_msg.pose.pose.orientation = imu_msg.orientation
#             odom_msg.twist.twist.linear.x = o[2]
#             odom_msg.twist.twist.angular.z = o[3]
#             self.odom_pub.publish(odom_msg)

#             t = TransformStamped()
#             t.header.stamp = now
#             t.header.frame_id = 'odom'
#             t.child_frame_id = 'base_link'
#             t.transform.translation.x = x
#             t.transform.translation.y = 0.0
#             t.transform.translation.z = 0.0
#             t.transform.rotation = imu_msg.orientation
#             self.tf_broadcaster.sendTransform(t)

#             print("[ROS2] 发布 IMU/ODOM/TF 成功")
#         except Exception as e:
#             self.get_logger().warn(f"[发布错误]: {e}")

# ########################################
# # 主入口：并行启动 Flask + ROS2
# ########################################
# def main():
#     flask_thread = threading.Thread(target=run_flask, daemon=True)
#     flask_thread.start()
#     print("[INFO] Flask 已启动")

#     rclpy.init()
#     try:
#         node = SerialPublisher()
#         print("[INFO] ROS2 节点已启动，进入循环")
#         rclpy.spin(node)
#     except KeyboardInterrupt:
#         print("[INFO] 用户中断")
#     finally:
#         rclpy.shutdown()
#         print("[INFO] ROS2 已关闭")

# if __name__ == '__main__':
#     main()


#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Quaternion, TransformStamped
from flask import Flask, request, jsonify
from flask_cors import CORS
import serial
import re
import math
import threading
import time
import sys
from tf2_ros import TransformBroadcaster

class FastSerialPublisher(Node):
    def __init__(self):
        super().__init__('fast_serial_publisher')
        
        # 初始化串口
        self.serial_lock = threading.Lock()
        try:
            self.ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=0.1)
            self.get_logger().info('串口打开成功')
        except Exception as e:
            self.get_logger().error(f'串口打开失败: {e}')
            # 尝试备用设备
            try:
                self.ser = serial.Serial('/dev/ttyAMA0', 115200, timeout=0.1)
                self.get_logger().info('串口在备用设备打开成功')
            except Exception as e2:
                self.get_logger().error(f'备用设备也失败: {e2}')
                self.ser = None

        # 数据缓冲区
        self.data_buffer = {
            'quaternion': None,
            'gyro': None,
            'accel': None,
            'odom': None
        }

        # ROS2发布者
        self.imu_pub = self.create_publisher(Imu, '/imu/data_raw', 10)
        self.odom_pub = self.create_publisher(Odometry, '/odom', 10)
        self.tf_broadcaster = TransformBroadcaster(self)
        
        # 设置日志级别
        self.get_logger().set_level(rclpy.logging.LoggingSeverity.INFO)
        
        # 创建串口读取定时器
        if self.ser:
            self.create_timer(0.001, self.read_serial)
        
        # 初始化Flask应用
        self.flask_app = Flask(__name__)
        CORS(self.flask_app)
        
        # 添加路由
        @self.flask_app.route('/command', methods=['POST'])
        def handle_command():
            return self._handle_command()
        
        # 启动Flask线程
        self.flask_thread = threading.Thread(
            target=self.flask_app.run, 
            kwargs={'host': '0.0.0.0', 'port': 5001},
            daemon=True
        )
        self.flask_thread.start()
        self.get_logger().info("Flask服务器启动于 0.0.0.0:5001")

    def _handle_command(self):
        """处理来自Flask的命令请求"""
        data = request.json
        command = data.get('command')  # 例如 "1", "a", "x"
        
        # 检查命令是否有效
        valid_commands = ['0','1','2','3','4','5','6','7','8','9','a','b','c','x','y','z']
        if not command or str(command) not in valid_commands:
            return jsonify({"status": "error", "message": "Invalid command"}), 400

        self.get_logger().info(f"收到指令: {command}")
        
        # 发送到STM32
        if self.ser and self.ser.is_open:
            with self.serial_lock:
                try:
                    byte_cmd = command.encode('utf-8')
                    self.ser.write(byte_cmd)
                    self.ser.flush()
                    self.get_logger().info(f"发送到STM32: {command}")
                    return jsonify({"status": "success", "command": command})
                except Exception as e:
                    self.get_logger().error(f"发送失败: {e}")
                    return jsonify({"status": "error", "message": "串口发送失败"}), 500
        else:
            self.get_logger().error("串口未打开")
            return jsonify({"status": "error", "message": "串口未打开"}), 500

    def read_serial(self):
        """从串口读取并处理数据"""
        if not self.ser or not self.ser.is_open:
            return
            
        try:
            with self.serial_lock:
                # 读取所有可用数据
                while self.ser.in_waiting > 0:
                    try:
                        line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    except UnicodeDecodeError:
                        continue
                        
                    if not line:
                        continue
                    
                    # 解析不同数据行
                    if line.startswith('Quaternion:'):
                        match = re.match(r"Quaternion: q0=([\d\-.]+) q1=([\d\-.]+) q2=([\d\-.]+) q3=([\d\-.]+)", line)
                        if match:
                            self.data_buffer['quaternion'] = tuple(map(float, match.groups()))
                    elif line.startswith('Gx:'):
                        match = re.match(r"Gx: ([\d\-.]+) Gy: ([\d\-.]+) Gz: ([\d\-.]+)", line)
                        if match:
                            self.data_buffer['gyro'] = tuple(map(float, match.groups()))
                    elif line.startswith('Ax:'):
                        match = re.match(r"Ax: ([\d\-.]+)G Ay: ([\d\-.]+)G Az: ([\d\-.]+)G", line)
                        if match:
                            self.data_buffer['accel'] = tuple(map(lambda x: float(x) * 9.80665, match.groups()))
                    elif line.startswith('LeftDist:'):
                        match = re.match(r"LeftDist: ([\d\-.]+) RightDist: ([\d\-.]+) LinVel: ([\d\-.]+) AngVel: ([\d\-.]+)", line)
                        if match:
                            self.data_buffer['odom'] = tuple(map(float, match.groups()))
                    
                    # 检查是否收集到完整数据
                    if all(self.data_buffer.values()):
                        self.publish_all()
                        for key in self.data_buffer:
                            self.data_buffer[key] = None
                        break  # 处理完一组数据后退出循环
        except Exception as e:
            self.get_logger().warn(f"[串口读取错误]: {e}")

    def publish_all(self):
        """发布所有收集到的传感器数据"""
        try:
            q = self.data_buffer['quaternion']
            g = self.data_buffer['gyro']
            a = self.data_buffer['accel']
            o = self.data_buffer['odom']

            now = self.get_clock().now().to_msg()

            # 发布IMU数据
            imu_msg = Imu()
            imu_msg.header.stamp = now
            imu_msg.header.frame_id = 'base_link'
            imu_msg.orientation = Quaternion(x=q[1], y=q[2], z=q[3], w=q[0])
            imu_msg.angular_velocity.x = g[0]
            imu_msg.angular_velocity.y = g[1]
            imu_msg.angular_velocity.z = g[2]
            imu_msg.linear_acceleration.x = a[0]
            imu_msg.linear_acceleration.y = a[1]
            imu_msg.linear_acceleration.z = a[2]
            self.imu_pub.publish(imu_msg)

            # 发布里程计数据
            odom_msg = Odometry()
            odom_msg.header.stamp = now
            odom_msg.header.frame_id = 'odom'
            odom_msg.child_frame_id = 'base_link'
            x = (o[0] + o[1]) / 2.0 / 100.0  # 转换为米
            odom_msg.pose.pose.position.x = x
            odom_msg.pose.pose.orientation = imu_msg.orientation
            odom_msg.twist.twist.linear.x = o[2]
            odom_msg.twist.twist.angular.z = o[3]
            self.odom_pub.publish(odom_msg)

            # 发布TF变换
            t = TransformStamped()
            t.header.stamp = now
            t.header.frame_id = 'odom'
            t.child_frame_id = 'base_link'
            t.transform.translation.x = x
            t.transform.translation.y = 0.0
            t.transform.translation.z = 0.0
            t.transform.rotation = imu_msg.orientation
            self.tf_broadcaster.sendTransform(t)

            self.get_logger().debug("[已发布IMU和里程计数据]")

        except Exception as e:
            self.get_logger().warn(f"[发布错误]: {e}")

    def destroy_node(self):
        # 关闭串口
        if self.ser and self.ser.is_open:
            self.ser.close()
            self.get_logger().info("串口已关闭")
        super().destroy_node()

def main():
    rclpy.init()
    try:
        node = FastSerialPublisher()
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    except Exception as e:
        print(f"[FATAL ERROR]: {e}")
    finally:
        node.destroy_node()
        rclpy.shutdown()
        print("ROS 2 已关闭")
        
if __name__ == '__main__':
    main()
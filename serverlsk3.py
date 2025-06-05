import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Imu
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Quaternion, TransformStamped
from tf2_ros import TransformBroadcaster
import serial
import threading
import re
import math

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
            try:
                self.ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=0.1)
                self.get_logger().info('备用串口打开成功')
            except Exception as e2:
                self.get_logger().error(f'备用串口打开失败: {e2}')
                self.ser = None

        # ROS发布者
        self.imu_pub = self.create_publisher(Imu, '/imu/data_raw', 10)
        self.odom_pub = self.create_publisher(Odometry, '/odom', 10)
        self.tf_broadcaster = TransformBroadcaster(self)

        # 数据缓冲区
        self.data_buffer = {
            'quaternion': None,
            'gyro': None,
            'accel': None,
            'odom': None
        }

        # 订阅cmd_vel话题
        self.create_subscription(Twist, '/cmd_vel', self.cmdvel_callback, 10)

        # 定时器读取串口
        if self.ser:
            self.create_timer(0.001, self.read_serial)

    def cmdvel_callback(self, msg):
        """将cmd_vel的速度指令发送到STM32"""
        v = msg.linear.x
        w = msg.angular.z

        # 构造格式化字符串：#V+0.220,A-0.130\n
        try:
            v = max(min(v, 1.5), -1.5)
            w = max(min(w, 2.0), -2.0)
            data_str = f"#V{v:+.3f},A{w:+.3f}\n"
            if self.ser and self.ser.is_open:
                with self.serial_lock:
                    self.ser.write(data_str.encode('utf-8'))
                    self.ser.flush()
                    self.get_logger().info(f"发送指令到STM32: {data_str.strip()}")
        except Exception as e:
            self.get_logger().error(f"发送失败: {e}")

    def read_serial(self):
        if not self.ser or not self.ser.is_open:
            return
        try:
            with self.serial_lock:
                while self.ser.in_waiting > 0:
                    try:
                        line = self.ser.readline().decode('utf-8', errors='ignore').strip()
                    except UnicodeDecodeError:
                        continue
                    if not line:
                        continue
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
                    if all(self.data_buffer.values()):
                        self.publish_all()
                        for key in self.data_buffer:
                            self.data_buffer[key] = None
                        break
        except Exception as e:
            self.get_logger().warn(f"[串口读取错误]: {e}")

    def publish_all(self):
        try:
            q = self.data_buffer['quaternion']
            g = self.data_buffer['gyro']
            a = self.data_buffer['accel']
            o = self.data_buffer['odom']

            now = self.get_clock().now().to_msg()

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

            odom_msg = Odometry()
            odom_msg.header.stamp = now
            odom_msg.header.frame_id = 'odom'
            odom_msg.child_frame_id = 'base_link'
            x = (o[0] + o[1]) / 2.0 / 100.0
            odom_msg.pose.pose.position.x = x
            odom_msg.pose.pose.orientation = imu_msg.orientation
            odom_msg.twist.twist.linear.x = o[2]
            odom_msg.twist.twist.angular.z = o[3]
            self.odom_pub.publish(odom_msg)

            t = TransformStamped()
            t.header.stamp = now
            t.header.frame_id = 'odom'
            t.child_frame_id = 'base_link'
            t.transform.translation.x = x
            t.transform.translation.y = 0.0
            t.transform.translation.z = 0.0
            t.transform.rotation = imu_msg.orientation
            self.tf_broadcaster.sendTransform(t)

        except Exception as e:
            self.get_logger().warn(f"[发布错误]: {e}")

    def destroy_node(self):
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

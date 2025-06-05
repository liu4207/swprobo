'''
from flask import Flask, request, jsonify
from flask_cors import CORS

app = Flask(__name__)
CORS(app)  # 解决跨域问题

@app.route('/command', methods=['POST'])
def handle_command():
    data = request.json
    command = data.get('command')
    print(f"[RECEIVED] 指令: {command}")
    # 在此添加控制GPIO或硬件的代码（例如控制电机）
    return jsonify({"status": "success"})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # 允许局域网访问
'''


'''
from flask import Flask, request, jsonify
from flask_cors import CORS
import serial
import threading
import time

# 初始化串口（根据你树莓派连接STM32的设备号修改，如 /dev/ttyUSB0 或 /dev/ttyAMA0）
try:
    ser = serial.Serial('/dev/serial0', 115200, timeout=1)
    print("[INFO] 串口已打开")
except serial.SerialException as e:
    print(f"[ERROR] 无法打开串口: {e}")
    ser = None

app = Flask(__name__)
CORS(app)  # 允许跨域访问

@app.route('/command', methods=['POST'])
def handle_command():
    data = request.json
    command = data.get('command')  # 例如 "1", "2", "3", "4"
    
    if not command or str(command)not in ['1','2','3','4']:
        return jsonify({"status": "error", "message": "缺少 'command' 参数"}), 400

    print(f"[RECEIVED] 指令: {command}")
    
    # 发送到STM32
    if ser and ser.is_open:
        try:
            ser.write(command.encode())
            print(f"[SENT TO STM32] 指令已发送: {command}")
        except Exception as e:
            print(f"[ERROR] 发送失败: {e}")
            return jsonify({"status": "error", "message": "串口发送失败"}), 500
    else:
        print("[ERROR] 串口未打开")
        return jsonify({"status": "error", "message": "串口未打开"}), 500

    return jsonify({"status": "success", "command": command})
''
def local_command_line():
    print(" 本地串口测试: 输入 1(前) 2(左) 3(后) 4(右), q退出")
    try:
        while True:
            cmd = input(">> 输入方向 (1/2/3/4/q): ").strip()
            if cmd == 'q':
                break
            elif cmd in ['1', '2', '3', '4']:
                if ser and ser.is_open:
                    ser.write(bytes([int(cmd)]))
                    print(f"[SENT MANUALLY] 指令已发送: {cmd}")
                else:
                    print("[ERROR] 串口未打开")
            else:
                print(" 无效指令，请输入 1/2/3/4/q")
    except KeyboardInterrupt:
        pass
''

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000)  # 所有IP都能访问（局域网）
'''


# from flask import Flask, request, jsonify
# from flask_cors import CORS
# import serial
# import time

# # 初始化串口（确保树莓派已禁用串口控制台）
# try:
#     ser = serial.Serial(
#         port='/dev/ttyUSB0',  # 或 /dev/ttyAMA0
#         baudrate=115200,
#         timeout=1,
#         bytesize=serial.EIGHTBITS,
#         parity=serial.PARITY_NONE,
#         stopbits=serial.STOPBITS_ONE
#     )
#     print("[INFO] 串口已打开")
# except serial.SerialException as e:
#     print(f"[ERROR] 无法打开串口: {e}")
#     ser = None

# app = Flask(__name__)
# CORS(app)  # 允许跨域访问

# @app.route('/command', methods=['POST'])
# def handle_command():
#     data = request.json
#     command = data.get('command')  # 例如 "1", "2", "3", "4"
    
#     if not command or str(command) not in ['0','1','2','3','4','5','6','7','8','9','a','b','c','x','y','z']:
#         return jsonify({"status": "error", "message": "Invalid command"}), 400

#     print(f"[RECEIVED] 指令: {command}")
    
#     # 发送到STM32（二进制数值 + 换行符）
#     if ser and ser.is_open:
#         try:
#             # 发送二进制数值（例如1 -> 0x01）并追加换行符
#             # byte_cmd = bytes([int(command)]) + b'\n'  # 根据STM32协议调整
#             # 改为发送字符 + 换行，例如 '1\n'
#             byte_cmd = (command + '\n').encode('utf-8')

#             ser.write(byte_cmd)
#             ser.flush()  # 确保数据立即发送
#             print(f"[SENT TO STM32] 发送数据: {byte_cmd.hex()}")
#         except Exception as e:
#             print(f"[ERROR] 发送失败: {e}")
#             return jsonify({"status": "error", "message": "串口发送失败"}), 500
#     else:
#         print("[ERROR] 串口未打开")
#         return jsonify({"status": "error", "message": "串口未打开"}), 500

#     return jsonify({"status": "success", "command": command})

# if __name__ == '__main__':
#     app.run(host='0.0.0.0', port=5001) 



from flask import Flask, request, jsonify
from flask_cors import CORS
import serial
import time

# 初始化串口（确保树莓派已禁用串口控制台）
try:
    ser = serial.Serial(
        port='/dev/ttyUSB0',  # 或 /dev/ttyAMA0
        baudrate=115200,
        timeout=1,
        bytesize=serial.EIGHTBITS,
        parity=serial.PARITY_NONE,
        stopbits=serial.STOPBITS_ONE
    )
    print("[INFO] 串口已打开")
except serial.SerialException as e:
    print(f"[ERROR] 无法打开串口: {e}")
    ser = None

app = Flask(__name__)
CORS(app)  # 允许跨域访问

@app.route('/command', methods=['POST'])
def handle_command():
    data = request.json
    command = data.get('command')  # 例如 "1", "a", "x"
    
    # 检查命令是否有效
    valid_commands = ['0','1','2','3','4','5','6','7','8','9','a','b','c','x','y','z']
    if not command or str(command) not in valid_commands:
        return jsonify({"status": "error", "message": "Invalid command"}), 400

    print(f"[RECEIVED] 指令: {command}")
    
    # 发送到STM32（直接发送字符）
    if ser and ser.is_open:
        try:
            # 直接发送原始字符（不带换行符）
            byte_cmd = command.encode('utf-8')
            ser.write(byte_cmd)
            ser.flush()  # 确保数据立即发送
            print(f"[SENT TO STM32] 发送数据: {byte_cmd.hex()}")
        except Exception as e:
            print(f"[ERROR] 发送失败: {e}")
            return jsonify({"status": "error", "message": "串口发送失败"}), 500
    else:
        print("[ERROR] 串口未打开")
        return jsonify({"status": "error", "message": "串口未打开"}), 500

    return jsonify({"status": "success", "command": command})

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5001)


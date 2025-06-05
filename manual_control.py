import serial
import time

# 初始化串口
try:
    ser = serial.Serial(
        port='/dev/ttyUSB0',
        baudrate=115200,
        timeout=1
    )
    print("[INFO] 串口已打开 /dev/ttyUSB0")
except serial.SerialException as e:
    print(f"[ERROR] 无法打开串口: {e}")
    ser = None

# 指令说明
command_map = {
    '1': '前进',
    '2': '左转',
    '3': '后退',
    '4': '右转',
    '5': 'gu',
    '6': 'hgi',
    '7': 'guf',
    '8': 'dwji',
    '9': 'jh',
    'a': 'hj',
    'b': 'dyt',
    'c': 'hg',
    'x': 'ghjg',
    'y': 'gjfy',
    'z': 'fuy'
}

def main():
    print("======== 地盘串口控制调试工具 ========")
    print("输入 1(前进), 2(左转), 3(后退), 4(右转)，q 退出")

    if not ser or not ser.is_open:
        print("[ERROR] 串口未打开，程序退出")
        return

    try:
        while True:
            cmd = input(">> 请输入指令 (1/2/3/4/q): ").strip()
            if cmd == 'q':
                print("退出调试")
                break
            elif cmd in command_map:
                # 发送 ASCII 字符 + 换行符
                packet = (cmd + '\n').encode()
                ser.write(packet)
                ser.flush()
                print(f"[SENT] 已发送指令: {cmd} ({command_map[cmd]})")
            else:
                print("[WARN] 无效输入，请输入 1 / 2 / 3 / 4 / q")
    except KeyboardInterrupt:
        print("\n用户中断，退出程序")
    finally:
        if ser and ser.is_open:
            ser.close()
            print("[INFO] 串口已关闭")

if __name__ == '__main__':
    main()

import serial
import time

# 串口参数配置
PORT = 'COM6'
BAUDRATE = 921600
LINES_PER_FILE = 2500

def main():
    ser = serial.Serial(PORT, BAUDRATE, timeout=1)

    file_index = 0
    line_buffer = []
    ser.dtr = False
    ser.rts = False

    print("开始接收数据...")

    try:
        while True:
            line = ser.readline().decode('ASCII').strip()
            print(line)
            if line:
                line_buffer.append(line)

                if len(line_buffer) >= LINES_PER_FILE:
                    filename = f"data_{file_index}.txt"
                    with open(filename, 'w') as f:
                        f.write('\n'.join(line_buffer))
                    print(f"已写入文件: {filename}")
                    line_buffer = []
                    file_index += 1
    except KeyboardInterrupt:
        print("停止采集")
    finally:
        if line_buffer:
            filename = f"data_{file_index}.txt"
            with open(filename, 'w') as f:
                f.write('\n'.join(line_buffer))
            print(f"保存剩余数据至: {filename}")
        ser.close()

if __name__ == "__main__":
    main()

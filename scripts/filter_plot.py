import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import firwin, filtfilt, freqz, lfilter

# 读取 ECG 数据
filename = "data_1.txt"
ecg_data = np.loadtxt(filename)

# 参数设置
fs = 250
lowcut = 0.5
highcut = 40
numtaps = 51

# 设计 Kaiser 窗 FIR 滤波器
b = firwin(numtaps, [lowcut, highcut], pass_zero=False, fs=fs)

# 去除直流分量
ecg_data_centered = ecg_data - np.mean(ecg_data)

# 双向滤波，零相位延迟
filtered_ecg = lfilter(b, 1, ecg_data_centered)

# 画滤波器频率响应
w, h = freqz(b, worN=8000, fs=fs)
plt.figure(figsize=(12, 6))
plt.plot(w, 20 * np.log10(abs(h)), 'b')
plt.title('Frequency Response of FIR Bandpass Filter')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Gain (dB)')
plt.grid(True)
plt.tight_layout()
plt.show()

# 画原始和滤波后信号对比
plt.figure(figsize=(12, 6))
plt.plot(ecg_data, label='Original ECG', alpha=0.6)
plt.plot(filtered_ecg, label='Filtered ECG (0.5-40 Hz)', linewidth=1.5)
plt.xlabel("Sample Index")
plt.ylabel("Amplitude")
plt.title("ECG Signal Before and After Filtering")
plt.legend()
plt.grid(True)
plt.tight_layout()
plt.show()

print("const float32_t ecg_filter_coeffs[ECG_FILTER_TAP_NUM] = {")
for i, c in enumerate(b):
    print(f"{c:.8f},", end=" ")
    if (i+1) % 5 == 0:
        print()
print("}；")
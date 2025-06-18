import numpy as np
import matplotlib.pyplot as plt

filename = "data_0.txt"
ecg_data = np.loadtxt(filename)

# 转换为 numpy 数组
h = np.array(ecg_data)

# 2. 进行傅里叶变换
H = np.fft.fft(h, n=1024)  # 用1024点FFT提升频谱分辨率
H_shifted = np.fft.fftshift(H)  # 把频谱中心移到0频点

# 3. 构造频率轴
fs = 250  # 归一化频率（或用实际采样率）
f = np.linspace(-fs / 2, fs / 2, len(H_shifted))

# 4. 画出幅度谱
plt.figure(figsize=(10, 5))
plt.plot(f, 20 * np.log10(np.abs(H_shifted) + 1e-12))  # dB幅度，加1e-12防止 log(0)
plt.title("Frequency Response of FIR Filter")
plt.xlabel("Normalized Frequency")
plt.ylabel("Magnitude (dB)")
plt.grid(True)
plt.tight_layout()
plt.show()

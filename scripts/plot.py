import numpy as np
import matplotlib.pyplot as plt

filename = "data_0.txt"
ecg_data = np.loadtxt(filename)

# 转换为 numpy 数组
h = np.array(ecg_data)

# 4. 画出幅度谱
plt.figure(figsize=(10, 5))
plt.plot(h, label='Original ECG')
plt.title("Frequency Response of FIR Filter")
plt.xlabel("Normalized Frequency")
plt.ylabel("Magnitude (dB)")
plt.grid(True)
plt.tight_layout()
plt.show()
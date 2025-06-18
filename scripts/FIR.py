from scipy.signal import firwin,butter, sosfreqz
import numpy as np
from scipy.signal import freqz
import matplotlib.pyplot as plt
import re

filter_path  = "../User/filter.c"

fs = 250
lowcut = 5
highcut = 15
numtaps = 101

coeffs = firwin(numtaps, [lowcut, highcut], pass_zero=False, fs=fs)
w, h = freqz(coeffs, worN=8000, fs=fs)

plt.plot(w, 20 * np.log10(np.abs(h)))
plt.title('Improved FIR Filter Frequency Response')
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude (dB)')
plt.grid()
plt.show()

with open(filter_path, "r", encoding="utf-8") as f:
    content = f.read()
    
print(content)
    
coeffs_str = "const float32_t fir_filter_coeffs[ECG_FILTER_TAP_NUM] = {\n"

for i, c in enumerate(coeffs):
    coeffs_str += f"{c:.8f}, "
    if (i + 1) % 5 == 0:
        coeffs_str += "\n"

coeffs_str += "};\n"

   
new_fir_code = "//Begin FIR filter\n" + f"#define ECG_FILTER_TAP_NUM {numtaps}\n" + "arm_fir_instance_f32 fir_filter;\n" +\
            "float32_t fir_filter_state[ECG_FILTER_TAP_NUM + 1];\n" + coeffs_str + "//End FIR filter"


pattern = r"//Begin FIR filter.*?//End FIR filter"
new_content = re.sub(pattern, new_fir_code, content, flags=re.DOTALL)

with open(filter_path, "w", encoding="utf-8") as f:
    f.write(new_content)
            
print("\nInitalize FIR Filter******************************************************************\n")
print("arm_fir_init_f32(&fir_filter, ECG_FILTER_TAP_NUM, fir_filter_coeffs, fir_filter_state, 1);")
print("\nFiltering*****************************************************************************\n")
print("arm_fir_f32(&fir_filter, &input, &output, 1);")
print("\n")

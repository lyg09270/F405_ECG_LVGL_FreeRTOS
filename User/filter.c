#include "filter.h"
#include <string.h>
#include <stdlib.h>

//DC removal param
#define DC_REMOVAL_WINDOW 20
float32_t dc_buffer[DC_REMOVAL_WINDOW] = {0};
uint32_t dc_index = 0;

//Moving average param
#define MA_WINDOW_SIZE 5  // 滑动平均窗口大小，建议 4~10
float ma_buffer[MA_WINDOW_SIZE] = {0};
uint8_t ma_index = 0;
uint8_t ma_filled = 0;


//Begin 50Hz notch filter
float32_t notch_coeffs[5] = {0.96953125, -0.59920327, 0.96953125, 0.59920327, -0.93906251};
arm_biquad_cascade_df2T_instance_f32 notch_filter;
float32_t notch_state[4];
//End 50Hz notch filter

//Begin FIR filter
#define ECG_FILTER_TAP_NUM 101
arm_fir_instance_f32 fir_filter;
float32_t fir_filter_state[ECG_FILTER_TAP_NUM + 1];
const float32_t fir_filter_coeffs[ECG_FILTER_TAP_NUM] = {
-0.00000000, -0.00012706, -0.00024068, -0.00031903, -0.00033598, 
-0.00026229, -0.00006954, 0.00026345, 0.00074278, 0.00135192, 
0.00204667, 0.00275462, 0.00338065, 0.00381865, 0.00396871, 
0.00375718, 0.00315623, 0.00219868, 0.00098426, -0.00032578, 
-0.00152693, -0.00240144, -0.00275696, -0.00246789, -0.00151275, 
0.00000000, 0.00182403, 0.00358965, 0.00484180, 0.00509904, 
0.00392705, 0.00101722, -0.00374149, -0.01020925, -0.01797125, 
-0.02634987, -0.03445581, -0.04127510, -0.04578348, -0.04707497, 
-0.04448827, -0.03771366, -0.02686459, -0.01250209, 0.00439442, 
0.02250909, 0.04031658, 0.05624004, 0.06882123, 0.07688208, 
0.07965742, 0.07688208, 0.06882123, 0.05624004, 0.04031658, 
0.02250909, 0.00439442, -0.01250209, -0.02686459, -0.03771366, 
-0.04448827, -0.04707497, -0.04578348, -0.04127510, -0.03445581, 
-0.02634987, -0.01797125, -0.01020925, -0.00374149, 0.00101722, 
0.00392705, 0.00509904, 0.00484180, 0.00358965, 0.00182403, 
0.00000000, -0.00151275, -0.00246789, -0.00275696, -0.00240144, 
-0.00152693, -0.00032578, 0.00098426, 0.00219868, 0.00315623, 
0.00375718, 0.00396871, 0.00381865, 0.00338065, 0.00275462, 
0.00204667, 0.00135192, 0.00074278, 0.00026345, -0.00006954, 
-0.00026229, -0.00033598, -0.00031903, -0.00024068, -0.00012706, 
-0.00000000, };
//End FIR filter

#define DIFF_TAP_NUM 5
float diff_coeffs[DIFF_TAP_NUM] = {
    0.25f, 0.125f, 0.0f, -0.125f, -0.25f
};
float diff_state[DIFF_TAP_NUM + 1];  // 状态 buffer
arm_fir_instance_f32 diff_filter;

#define INTEGRATOR_TAP_NUM 30
float integrator_coeffs[INTEGRATOR_TAP_NUM] = {
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,1,
	1,1,1,1,1,
};

float integrator_state[INTEGRATOR_TAP_NUM + 1];
arm_fir_instance_f32 integrator_filter;

//Begin Respiration filter
#define RESP_FILTER_TAP_NUM 101
arm_fir_instance_f32 resp_filter;
float32_t resp_filter_state[RESP_FILTER_TAP_NUM + 1];
const float32_t resp_filter_coeffs[ECG_FILTER_TAP_NUM] = {
0.00139471, 0.00141529, 0.00146770, 0.00155203, 0.00166827, 
0.00181626, 0.00199567, 0.00220608, 0.00244690, 0.00271739, 
0.00301672, 0.00334388, 0.00369778, 0.00407716, 0.00448067, 
0.00490685, 0.00535412, 0.00582080, 0.00630512, 0.00680524, 
0.00731921, 0.00784505, 0.00838068, 0.00892400, 0.00947283, 
0.01002501, 0.01057830, 0.01113049, 0.01167933, 0.01222261, 
0.01275811, 0.01328363, 0.01379703, 0.01429621, 0.01477909, 
0.01524370, 0.01568811, 0.01611048, 0.01650906, 0.01688219, 
0.01722832, 0.01754600, 0.01783392, 0.01809087, 0.01831577, 
0.01850769, 0.01866583, 0.01878951, 0.01887823, 0.01893161, 
0.01894943, 0.01893161, 0.01887823, 0.01878951, 0.01866583, 
0.01850769, 0.01831577, 0.01809087, 0.01783392, 0.01754600, 
0.01722832, 0.01688219, 0.01650906, 0.01611048, 0.01568811, 
0.01524370, 0.01477909, 0.01429621, 0.01379703, 0.01328363, 
0.01275811, 0.01222261, 0.01167933, 0.01113049, 0.01057830, 
0.01002501, 0.00947283, 0.00892400, 0.00838068, 0.00784505, 
0.00731921, 0.00680524, 0.00630512, 0.00582080, 0.00535412, 
0.00490685, 0.00448067, 0.00407716, 0.00369778, 0.00334388, 
0.00301672, 0.00271739, 0.00244690, 0.00220608, 0.00199567, 
0.00181626, 0.00166827, 0.00155203, 0.00146770, 0.00141529, 
0.00139471, };
//End Respiration filter


void filter_init(void)
{
	arm_fir_init_f32(&fir_filter, ECG_FILTER_TAP_NUM, fir_filter_coeffs, fir_filter_state, 1);
	arm_fir_init_f32(&resp_filter, RESP_FILTER_TAP_NUM, resp_filter_coeffs, resp_filter_state, 1);
	arm_fir_init_f32(&diff_filter, DIFF_TAP_NUM, diff_coeffs, diff_state, 1);
	arm_fir_init_f32(&integrator_filter, INTEGRATOR_TAP_NUM, integrator_coeffs, integrator_state, 1);
}

float32_t remove_dc(float32_t input) 
{
    dc_buffer[dc_index++] = input;
    if (dc_index >= DC_REMOVAL_WINDOW) dc_index = 0;

    float32_t sum = 0;
    for (int i = 0; i < DC_REMOVAL_WINDOW; i++) sum += dc_buffer[i];
    return input - (sum / DC_REMOVAL_WINDOW);
}

float32_t moving_average(float new_sample)
{
    ma_buffer[ma_index] = new_sample;
    ma_index = (ma_index + 1) % MA_WINDOW_SIZE;

    if (ma_filled < MA_WINDOW_SIZE) ma_filled++;

    float sum = 0;
    for (int i = 0; i < ma_filled; i++)
        sum += ma_buffer[i];

    return sum / ma_filled;
}

int compare_float(const void *a, const void *b) 
{
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    if (fa < fb) return -1;
    if (fa > fb) return 1;
    return 0;
}
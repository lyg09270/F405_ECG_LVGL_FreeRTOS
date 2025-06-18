#ifndef FILTER_H
#define FILTER_H

#include "arm_math.h"

extern arm_biquad_cascade_df2T_instance_f32 notch_filter;
extern arm_biquad_cascade_df2T_instance_f32 butter_filter;
extern arm_fir_instance_f32 diff_filter;
extern arm_fir_instance_f32 integrator_filter;
extern arm_fir_instance_f32 fir_filter;
extern arm_fir_instance_f32 resp_filter;

void filter_init(void);
float32_t remove_dc(float32_t input);
float32_t moving_average(float new_sample);
int compare_float(const void *a, const void *b);

#endif //FILTER_H
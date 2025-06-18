#include "app.h"
#include "nnom.h"
#include "weights.h"
#include "ads1292.h"
#include "filter.h"

int32_t adcValue;

#ifdef NNOM_USING_STATIC_MEMORY
	uint8_t static_buf[1024 * 8];
#endif //NNOM_USING_STATIC_MEMORY
#define QUANTIFICATION_SCALE (pow(2,INPUT_1_OUTPUT_DEC))

nnom_model_t* model;

SemaphoreHandle_t lvgl_mutex;
lv_chart_series_t * ui_Chart1_series_1;
lv_coord_t ui_Chart1_series_1_array[SLIDE_WINDOW_MAX_SIZE] = {0};
lv_disp_t * dispp;
lv_theme_t * theme;
Slide_Window adc_window;

TaskHandle_t DisplayTaskHandle;
TaskHandle_t Chart1RefreshTaskHandle;
TaskHandle_t ecgTaskHandle = NULL;

QueueHandle_t ecgQueueHandle;

#define ALPHA 0.2f
#define SAMPLING_RATE 250.0f

typedef struct {
    uint32_t sample_counter;      // 总采样点计数
    uint32_t last_peak_sample;    // 上一次检测到峰值的采样点
    int above_threshold;          // 锁存标志
    float smoothed_bpm;           // 平滑后的BPM
    int initialized;
} BPMDetector;

BPMDetector bpm_detector = {0};

void bpm_detector_init(void) {
    bpm_detector.sample_counter = 0;
    bpm_detector.last_peak_sample = 0;
    bpm_detector.above_threshold = 0;
    bpm_detector.smoothed_bpm = 0;
    bpm_detector.initialized = 0;
}

void model_feed_data(void)
{
	srand(xTaskGetTickCount());
	const double scale = QUANTIFICATION_SCALE;
	uint16_t i = 0;
	for (int i = 0; i < 784; i++) {
        nnom_input_data[i] = (int8_t)((rand() % 256) - 128);
    }

}

int8_t model_get_output(void)
{
	volatile int8_t i = 0;
	volatile int8_t max_output = -128;
	int8_t ret = 0;
	model_feed_data();
	model_run(model);
	for(i = 0; i < 5;i++){
		printf("Output[%d] = %.2f %%\n",i,(nnom_output_data[i] / 127.0)*100);
		if(nnom_output_data[i] >= max_output){
			max_output = nnom_output_data[i] ;
			ret = i;
		}
	}
	
	return ret;
}

void LEDTask(void* param) 
{
	UNUSED(param);
	while(1) {
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_8);
		vTaskDelay(500);
		}
}


int calculate_bpm(float32_t integrated_value, float32_t threshold) {
    static int last_returned_bpm = 0;
    int bpm = 0;

    bpm_detector.sample_counter++;

    if (integrated_value > threshold) {
        if (!bpm_detector.above_threshold) {
            bpm_detector.above_threshold = 1;

            if (bpm_detector.last_peak_sample != 0) {
                uint32_t rr_interval_samples = bpm_detector.sample_counter - bpm_detector.last_peak_sample;

                if (rr_interval_samples > (int)(SAMPLING_RATE * 0.3f) && rr_interval_samples < (int)(SAMPLING_RATE * 2.0f)) {
                    float instant_bpm = 60.0f * SAMPLING_RATE / rr_interval_samples;

                    if (!bpm_detector.initialized) {
                        bpm_detector.smoothed_bpm = instant_bpm;
                        bpm_detector.initialized = 1;
                    } else {
                        bpm_detector.smoothed_bpm = ALPHA * instant_bpm + (1 - ALPHA) * bpm_detector.smoothed_bpm;
                    }

                    bpm = (int)(bpm_detector.smoothed_bpm + 0.5f);
                    last_returned_bpm = bpm;
                }
            }
            bpm_detector.last_peak_sample = bpm_detector.sample_counter;
        }
    } else {
        bpm_detector.above_threshold = 0;
    }

    if (bpm == 0) {
        return last_returned_bpm;
    } else {
        return bpm;
    }
}


void ECGTask(void* param)
{
	UNUSED(param);
	float value[2] = {0};
	
	ads1292_init();
	ads1292_sampling_start();
	filter_init();
	HAL_NVIC_EnableIRQ(EXTI2_IRQn);
	while(1)
	{
		if (xQueueReceive(ecgQueueHandle, &value, portMAX_DELAY) == pdTRUE)
		{
			float32_t input_resp = value[0] *ADS1292_CH1_FACTOR_MV;
			float32_t input_ecg = value[1] *ADS1292_CH2_FACTOR_MV;
			
			
			float32_t output_ecg;
			float32_t output_resp;
			float32_t output_tompkins;
			float32_t output_square;
			
			arm_fir_f32(&fir_filter, &input_ecg, &output_ecg, 1);
			arm_fir_f32(&diff_filter, &output_ecg, &output_square, 1);
			output_square = output_square * output_square;
			arm_fir_f32(&integrator_filter, &output_square, &output_tompkins, 1);
			int bpm = calculate_bpm(output_tompkins,1e+6);

			arm_fir_f32(&resp_filter, &input_resp, &output_resp, 1);
//			output_resp = remove_dc(output_resp);
//			output_resp = moving_average(output_resp);
			//printf("%f %f\n", output_resp,value[0]);
			//printf("%f %f %d\n", output_ecg,output_tompkins,bpm);
			//printf("%f\n",value[1]);
			slide_window_add(&adc_window, (int32_t)output_ecg);
			for (int i = 0; i < SLIDE_WINDOW_MAX_SIZE; i++)
			{
				slide_window_retrieve(&adc_window, ui_Chart1_series_1_array, i);
			}
		}
	}
}

void Chart1RefreshTask(void* param) {
    UNUSED(param);
 
    while (1) {
		if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
			lv_chart_refresh(ui_Chart1);
			xSemaphoreGive(lvgl_mutex);
		}
		vTaskDelay(1);
    }
}

void DisplayTask(void* param) {
		UNUSED(param);
		lv_init();
		lv_tick_set_cb(xTaskGetTickCount);
		lv_port_disp_init();
		lv_port_indev_init();
		dispp = lv_display_get_default();
		theme = lv_theme_default_init(dispp, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED),true, LV_FONT_DEFAULT);
		lv_disp_set_theme(dispp, theme);
		ui_Screen1_screen_init();
    ui_Screen2_screen_init();
    ui_Screen3_screen_init();
    ui_Screen4_screen_init();
    ui_Screen5_screen_init();
		ui_screen_menu(NULL);

    while (1) {
		if (xSemaphoreTake(lvgl_mutex, portMAX_DELAY) == pdTRUE) {
			lv_task_handler();
			xSemaphoreGive(lvgl_mutex);
		}
    }
}

void NNTask(void* param){
	UNUSED(param);
	#ifdef NNOM_USING_STATIC_MEMORY
		nnom_set_static_buf(static_buf, sizeof(static_buf)); 
	#endif //NNOM_USING_STATIC_MEMORY
	model = nnom_model_create();
	
	while(1)
	{
		uint32_t t_start = xTaskGetTickCount();
        model_get_output();
        uint32_t t_end = xTaskGetTickCount();

        uint32_t time_used_ms = (t_end - t_start) * portTICK_PERIOD_MS;
        printf("Inference time: %d ms\n", time_used_ms);
		vTaskDelay(1000);
	}
}

void AppInit(void)
{
	lvgl_mutex = xSemaphoreCreateMutex();
	ecgQueueHandle = xQueueCreate(15, sizeof(float)*2);
	xTaskCreate(LEDTask,"Task1",100,NULL,1,NULL);
	//xTaskCreate(DisplayTask,"Task3",2048,NULL,1,NULL);
	xTaskCreate(ECGTask, "ECG", 512, NULL, 1, &ecgTaskHandle);
	//xTaskCreate(NNTask,"NNTask",1024,NULL,0,NULL);
	vTaskStartScheduler();
}

static uint8_t quantize_fast(uint32_t input) {
    return (input * 255 + 2047) / 4095;
}

void EXTI2_IRQHandler(void)
{
	__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    float value[2] = {0};
		ads1292_read_frame(value);
    xQueueSendFromISR(ecgQueueHandle, &value, &xHigherPriorityTaskWoken);
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);	
}



void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    printf("Task %s stack overflow\n", pcTaskName);

    printf("Task handle address：%p\n", xTask);

    taskDISABLE_INTERRUPTS();
    for(;;);
}
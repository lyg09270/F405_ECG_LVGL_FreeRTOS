// ADS1292 Register Map Definitions

#ifndef ADS1292_H
#define ADS1292_H

#include "spi.h"
#include "gpio.h"
#include "FreeRTOS.h"
#include "task.h"

//Constant
#define ADS1292_V_REF 2.42f
#define ADS1292_CH1_GAIN 2
#define ADS1292_CH2_GAIN 6
#define ADS1292_CH1_FACTOR_MV ((ADS1292_V_REF / ADS1292_CH1_GAIN) / 8388608.0f * 10000000.0f);
#define ADS1292_CH2_FACTOR_MV ((ADS1292_V_REF / ADS1292_CH2_GAIN) / 8388608.0f * 10000000.0f);

//PIN Defines
#define ADS1292_CS_LOW	    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_RESET)
#define ADS1292_CS_HIGH     HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, GPIO_PIN_SET)
#define ADS1292_DRDY_LOW    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_RESET)
#define ADS1292_DRDY_HIGH   HAL_GPIO_WritePin(GPIOD, GPIO_PIN_2, GPIO_PIN_SET)
#define ADS1292_RESET_LOW   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET)
#define ADS1292_RESET_HIGH  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET)
#define ADS1292_START_LOW	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET)
#define ADS1292_START_HIGH  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET)

//Delay
#define ADS1292_DELAY_MS(ms) vTaskDelay(ms)

#define ADS1292_FRAME_SIZE 9  // 3 status + 2 channels * 3 bytes

// System Commands
#define ADS1292_CMD_WAKEUP     0x02  // Wake-up from standby mode
#define ADS1292_CMD_STANDBY    0x04  // Enter standby mode
#define ADS1292_CMD_RESET      0x06  // Reset the device
#define ADS1292_CMD_START      0x08  // Start/restart conversions
#define ADS1292_CMD_STOP       0x0A  // Stop conversion
#define ADS1292_CMD_OFFSETCAL  0x1A  //Channel offset calibration
// Data Read Commands
#define ADS1292_CMD_RDATAC     0x10  // Enable Read Data Continuous mode
#define ADS1292_CMD_SDATAC     0x11  // Stop Read Data Continuous mode
#define ADS1292_CMD_RDATA      0x12  // Read data by command (only after SDATAC)

// Register Access Commands (command ORed with address internally)
#define ADS1292_CMD_RREG       0x20  // Read register (0x20 | addr), followed by number of registers
#define ADS1292_CMD_WREG       0x40  // Write register (0x40 | addr), followed by number of registers

//Register
#define ADS1292_REG_ID          0x00  // ID Control Register (Read-only, contains revision info)
#define ADS1292_REG_CONFIG1     0x01  // Configuration Register 1 (sets data rate and single-shot mode)
#define ADS1292_REG_CONFIG2     0x02  // Configuration Register 2 (test signal, reference, clock options)
#define ADS1292_REG_LOFF        0x03  // Lead-Off Control Register (lead-off detection settings)
#define ADS1292_REG_CH1SET      0x04  // Channel 1 Settings (power-down, gain, input mux)
#define ADS1292_REG_CH2SET      0x05  // Channel 2 Settings (same as CH1SET but for channel 2)
#define ADS1292_REG_RLD_SENS    0x06  // Right Leg Drive Sense (RLD sense configuration)
#define ADS1292_REG_LOFF_SENS   0x07  // Lead-Off Sense Selection (which inputs to monitor)
#define ADS1292_REG_LOFF_STAT   0x08  // Lead-Off Status (lead-off detection results)
#define ADS1292_REG_RESP1       0x09  // Respiration Control Register 1 (for ADS1292R only)
#define ADS1292_REG_RESP2       0x0A  // Respiration Control Register 2 (for calibration and frequency)
#define ADS1292_REG_GPIO        0x0B  // General Purpose I/O Register (controls GPIOs)

// ID Register Bit Definitions (Read-only)
#define ADS1292_ID_EXPECTED_VALUE 0x73

//Configuration

// CONFIG1 Register Bit Masks
#define ADS1292_CONFIG1_SINGLE_SHOT        0x80  // Bit 7: Single-shot conversion mode enable
#define ADS1292_CONFIG1_RESERVED           0x00  // Bits 6:4: Reserved, always write 000
#define ADS1292_CONFIG1_DATA_RATE_125SPS   0x00  // Bits 2:0: Data Rate = 125SPS
#define ADS1292_CONFIG1_DATA_RATE_250SPS   0x01  // 250SPS
#define ADS1292_CONFIG1_DATA_RATE_500SPS   0x02  // 500SPS (default)
#define ADS1292_CONFIG1_DATA_RATE_1KSPS    0x03  // 1kSPS
#define ADS1292_CONFIG1_DATA_RATE_2KSPS    0x04  // 2kSPS
#define ADS1292_CONFIG1_DATA_RATE_4KSPS    0x05  // 4kSPS
#define ADS1292_CONFIG1_DATA_RATE_8KSPS    0x06  // 8kSPS
#define ADS1292_CONFIG1_DATA_RATE_NOT_USE  0x07  // Do not use

// CONFIG2 Register Bit Masks
#define ADS1292_CONFIG2_RESERVED           0x80  // Bit 7: Reserved, always write 1
#define ADS1292_CONFIG2_INT_TEST_OFF       0x00  // Bit 1: Test signal disabled
#define ADS1292_CONFIG2_INT_TEST_ON        0x02  //        Test signal enabled
#define ADS1292_CONFIG2_TEST_FREQ_DC       0x00  // Bit 0: Test signal DC
#define ADS1292_CONFIG2_TEST_FREQ_AC       0x01  //        Test signal 1Hz
#define ADS1292_CONFIG2_PDB_REFBUF_OFF     0x00  // Bit 5: Internal reference buffer off
#define ADS1292_CONFIG2_PDB_REFBUF_ON      0x20  //        Internal reference buffer on
#define ADS1292_CONFIG2_VREF_242V          0x00  // Bit 4: Internal reference = 2.42V
#define ADS1292_CONFIG2_VREF_4V            0x10  //        Internal reference = 4.033V
#define ADS1292_CONFIG2_CLK_EN             0x00  // Bit 3: Internal clock output enabled
#define ADS1292_CONFIG2_CLK_DIS            0x08  //        Internal clock output disabled
#define ADS1292_CONFIG2_LOFF_COMP_OFF      0x00  // Bit 6: Lead-off comparator off
#define ADS1292_CONFIG2_LOFF_COMP_ON       0x40  //        Lead-off comparator on

// LOFF Register Bit Masks
#define ADS1292_LOFF_COMP_TH_95_5          0x00  // Bits 7:5: 95%/5% threshold
#define ADS1292_LOFF_COMP_TH_92_5_7_5      0x20
#define ADS1292_LOFF_COMP_TH_90_10         0x40
#define ADS1292_LOFF_COMP_TH_87_5_12_5     0x60
#define ADS1292_LOFF_COMP_TH_85_15         0x80
#define ADS1292_LOFF_COMP_TH_80_20         0xA0
#define ADS1292_LOFF_COMP_TH_75_25         0xC0
#define ADS1292_LOFF_COMP_TH_70_30         0xE0

#define ADS1292_LOFF_ILEAD_OFF_6NA         0x00  // Bits 3:2: 6nA current
#define ADS1292_LOFF_ILEAD_OFF_22NA        0x04  // 22nA
#define ADS1292_LOFF_ILEAD_OFF_6UA         0x08  // 6uA
#define ADS1292_LOFF_ILEAD_OFF_22UA        0x0C  // 22uA

#define ADS1292_LOFF_FREQ_DC               0x00  // Bit 0: DC lead-off detection
#define ADS1292_LOFF_FREQ_AC               0x01  //        AC lead-off detection
#define ADS1292_LOFF_RESERVED              0x10  // Bit 4: Reserved


//CH1SET (0x04) / CH2SET (0x05) – Channel Settings
#define ADS1292_CHnSET_PD_OFF           0x80  // Bit 7: Power-down channel (1=power-down)
#define ADS1292_CHnSET_GAIN_6           0x00  // Bits[6:4]: PGA gain = 6 (default)
#define ADS1292_CHnSET_GAIN_1           0x10
#define ADS1292_CHnSET_GAIN_2           0x20
#define ADS1292_CHnSET_GAIN_3           0x30
#define ADS1292_CHnSET_GAIN_4           0x40
#define ADS1292_CHnSET_GAIN_8           0x50
#define ADS1292_CHnSET_GAIN_12          0x60
#define ADS1292_CHnSET_MUX_NORMAL       0x00  // Bits[3:0]: Normal electrode input
#define ADS1292_CHnSET_MUX_SHORTED      0x01  // Input shorted
#define ADS1292_CHnSET_MUX_RLD_MEASURE  0x02
#define ADS1292_CHnSET_MUX_MVDD         0x03
#define ADS1292_CHnSET_MUX_TEMP_SENSOR  0x04
#define ADS1292_CHnSET_MUX_TEST_SIGNAL  0x05
#define ADS1292_CHnSET_MUX_RLD_DRP      0x06
#define ADS1292_CHnSET_MUX_RLD_DRM      0x07
#define ADS1292_CHnSET_MUX_RLD_DRPM     0x08
#define ADS1292_CHnSET_MUX_IN3P_IN3N    0x09
#define ADS1292_CHnSET_MUX_RESERVED     0x0A

//RLD_SENS (0x06) – Right-Leg Drive Sense
#define ADS1292_RLD_CHOP_OFF            0x00  // Bits[7:6]: CHOP freq = fMOD / 16
#define ADS1292_RLD_CHOP_RESERVED       0x40
#define ADS1292_RLD_CHOP_HALF           0x80  // fMOD / 2
#define ADS1292_RLD_CHOP_QUARTER        0xC0  // fMOD / 4
#define ADS1292_RLD_PDB_OFF             0x00  // Bit 5: RLD buffer off
#define ADS1292_RLD_PDB_ON              0x20
#define ADS1292_RLD_LOFF_SENSE_OFF      0x00  // Bit 4: RLD lead-off sense disable
#define ADS1292_RLD_LOFF_SENSE_ON       0x10
#define ADS1292_RLD2N_ENABLE            0x08
#define ADS1292_RLD2P_ENABLE            0x04
#define ADS1292_RLD1N_ENABLE            0x02
#define ADS1292_RLD1P_ENABLE            0x01

//LOFF_SENS (0x07) – Lead-Off Sense Selection
#define ADS1292_LOFF_FLIP2_ENABLE       0x20
#define ADS1292_LOFF_FLIP1_ENABLE       0x10
#define ADS1292_LOFF_2N_ENABLE          0x08
#define ADS1292_LOFF_2P_ENABLE          0x04
#define ADS1292_LOFF_1N_ENABLE          0x02
#define ADS1292_LOFF_1P_ENABLE          0x01
#define ADS1292_LOFF_SENS_RESERVED      0xC0  // Bits[7:6] must be 0


//RESP1 (0x09) – Respiration Control 1
#define ADS1292_RESP1_DEMOD_EN             0x80  // Bit 7: Demodulator enable
#define ADS1292_RESP1_MOD_EN               0x40  // Bit 6: Modulator enable
#define ADS1292_RESP1_PHASE_0000           0x00  // Bits[5:2]: Phase setting
#define ADS1292_RESP1_PHASE_0001           0x04
#define ADS1292_RESP1_PHASE_0010           0x08
#define ADS1292_RESP1_PHASE_0100           0x10
#define ADS1292_RESP1_PHASE_0101           0x14
#define ADS1292_RESP1_PHASE_0110           0x18
#define ADS1292_RESP1_PHASE_0111           0x1C
#define ADS1292_RESP1_RESERVED             0x02  // Bit 1 must be 1
#define ADS1292_RESP1_CTRL_INTERNAL				 0x00
#define ADS1292_RESP1_CTRL_EXTERNAL        0x01  // Bit 0: RESP control bit

//RESP2 (0x0A) – Respiration Control 2
#define ADS1292_RESP2_CALIB_OFF            0x00  // Bit 7: Calibration off
#define ADS1292_RESP2_CALIB_ON             0x80
#define ADS1292_RESP2_FREQ_32KHZ           0x00  // Bit 1: RESP_FREQ low
#define ADS1292_RESP2_FREQ_64KHZ           0x04
#define ADS1292_RESP2_RLDREF_EXTERNAL      0x00  // Bit 1: External RLDREF
#define ADS1292_RESP2_RLDREF_INTERNAL      0x02  // Internal RLDREF connect to IN1N
#define ADS1292_RESP2_RESERVED						 0x01

//GPIO (0x0B)
#define ADS1292_GPIO_C2_HIGH               0x08
#define ADS1292_GPIO_C1_HIGH               0x04
#define ADS1292_GPIO_D2_HIGH               0x02
#define ADS1292_GPIO_D1_HIGH               0x01
#define ADS1292_GPIO_RESERVED              0xF0  // Bits 7:4 are reserved


extern uint8_t ads1292_rx_buf[ADS1292_FRAME_SIZE];

void ads1292_check_id();
void ads1292_init();
void ads1292_write_reg(uint8_t regAddress, uint8_t value);
void ads1292_read_reg(uint8_t regAddress,uint8_t *rx);
void ads1292_send_cmd(uint8_t cmd);
void ads1292_write_regs(uint8_t regAddress, uint8_t *data, uint8_t len);
void ads1292_read_regs(uint8_t regAddress, uint8_t *buffer, uint8_t len);
void ads1292_sampling_start();
void ads1292_read_frame(float* data);

#endif //ADS1292_H
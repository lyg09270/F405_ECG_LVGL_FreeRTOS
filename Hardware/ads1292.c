#include "ads1292.h"
#include "spi.h"
#include <stdio.h>

uint8_t ads1292_rx_buf[ADS1292_FRAME_SIZE];

void ads1292_check_id()
{
    uint8_t id = 0;
	
	while(id != ADS1292_ID_EXPECTED_VALUE)
	{
		ads1292_read_reg(ADS1292_REG_ID, &id);
		printf("ADS1292 ID Register: 0x%02X\r\n", id);
	}
    if(id == ADS1292_ID_EXPECTED_VALUE) {
        printf("ADS1292 communication OK!\r\n");
    } else {
        printf("ADS1292 communication FAILED!\r\n");
    }
}


void ads1292_init()
{
	ADS1292_START_LOW;
	ADS1292_RESET_LOW;
	ADS1292_DELAY_MS(1);
  ADS1292_RESET_HIGH;
	ADS1292_CS_HIGH;
	
    ads1292_send_cmd(ADS1292_CMD_SDATAC);
    ADS1292_DELAY_MS(1);

    ads1292_send_cmd(ADS1292_CMD_RESET);
    ADS1292_DELAY_MS(1);

    ads1292_send_cmd(ADS1292_CMD_SDATAC);
    ADS1292_DELAY_MS(1);
	
		ads1292_check_id();

		
		ADS1292_CS_LOW;
		ADS1292_DELAY_MS(1);
    // CONFIG1 (0x01): Set data rate to 125SPS, single-shot mode disabled
    ads1292_write_reg(ADS1292_REG_CONFIG1,
							ADS1292_CONFIG1_RESERVED |
                      ADS1292_CONFIG1_DATA_RATE_250SPS);

    // CONFIG2 (0x02): Internal reference buffer ON, 2.42V Vref,
    // disable test signal, enable lead-off comparator
    ads1292_write_reg(ADS1292_REG_CONFIG2,
                      ADS1292_CONFIG2_RESERVED |
                      ADS1292_CONFIG2_PDB_REFBUF_ON |
                      ADS1292_CONFIG2_VREF_242V |
                      ADS1292_CONFIG2_CLK_DIS |
                      ADS1292_CONFIG2_INT_TEST_OFF |
                      ADS1292_CONFIG2_LOFF_COMP_OFF |
                      ADS1292_CONFIG2_TEST_FREQ_DC);

//    // LOFF (0x03): Enable lead-off detection, 6nA current, 90/10 threshold, DC mode
    ads1292_write_reg(ADS1292_REG_LOFF,
                      ADS1292_LOFF_COMP_TH_90_10 |
                      ADS1292_LOFF_ILEAD_OFF_6NA |
                      ADS1292_LOFF_FREQ_DC |
                      ADS1292_LOFF_RESERVED);

    // CH1SET (0x04): Power ON, Gain = 2, Normal electrode input
    ads1292_write_reg(ADS1292_REG_CH1SET,
                      ADS1292_CHnSET_GAIN_2 |
                      ADS1292_CHnSET_MUX_NORMAL);
		//ads1292_send_cmd(ADS1292_CMD_OFFSETCAL);

    // CH2SET (0x05): Power ON, Gain = 6, Normal electrode input
    ads1292_write_reg(ADS1292_REG_CH2SET,
                      ADS1292_CHnSET_GAIN_6 |
                      ADS1292_CHnSET_MUX_NORMAL);
		//ads1292_send_cmd(ADS1292_CMD_OFFSETCAL);
					  
	// RLD_SENS (0x06): Enable CH2P and CH2N, RLD buffer ON, no lead-off, no chopper
	ads1292_write_reg(ADS1292_REG_RLD_SENS,
					  ADS1292_RLD_PDB_ON   |
					  ADS1292_RLD2N_ENABLE  |
					  ADS1292_RLD2P_ENABLE  |
						ADS1292_RLD_LOFF_SENSE_OFF |  // Disable lead-off detection
					  ADS1292_RLD_CHOP_OFF);      // Chopper off (default)

    // LOFF_SENS (0x07): Enable lead-off sense on all inputs
	ads1292_write_reg(ADS1292_REG_LOFF_SENS,  
					  ADS1292_LOFF_2N_ENABLE |
					  ADS1292_LOFF_2P_ENABLE );

//    // LOFF_STAT (0x08): Read-only, no need to configure

    // RESP1 (0x09): Disable respiration modulator/demodulator
    ads1292_write_reg(ADS1292_REG_RESP1,
						ADS1292_RESP1_DEMOD_EN      |
						ADS1292_RESP1_MOD_EN        |
						ADS1292_RESP1_PHASE_0110    |
						ADS1292_RESP1_CTRL_INTERNAL |
						ADS1292_RESP1_RESERVED);

    // RESP2 (0x0A): Calibration off, RESP freq = 64kHz, external RLDREF
    ads1292_write_reg(ADS1292_REG_RESP2,
						ADS1292_RESP2_CALIB_OFF       |
						ADS1292_RESP2_FREQ_64KHZ     |
						ADS1292_RESP2_RLDREF_INTERNAL|
						ADS1292_RESP2_RESERVED);

    // GPIO (0x0B): All outputs low
    ads1292_write_reg(ADS1292_REG_GPIO, 0x0C);
		
		uint8_t buffer[12] = {0};
		ads1292_read_regs(0x00,buffer,12);
		
		for(int i = 0; i < 12; i++)
		{
			printf("%x\n",buffer[i]);
		}
	
	ADS1292_DELAY_MS(1);
	ADS1292_CS_HIGH;
}


void ads1292_write_reg(uint8_t regAddress, uint8_t value)
{
	uint8_t tx[3];
	tx[0] = 0x40 | regAddress;
	tx[1] = 0x00;              
	tx[2] = value;
	
	HAL_SPI_Transmit(&hspi3, &tx[0], 1,1000);
	HAL_SPI_Transmit(&hspi3, &tx[1], 1,1000);
	HAL_SPI_Transmit(&hspi3, &tx[2], 1,1000);
}

void ads1292_read_reg(uint8_t regAddress,uint8_t *rx)
{
	uint8_t tx[2];

	tx[0] = 0x20 | regAddress;
	tx[1] = 0x00;              
	
	ADS1292_CS_LOW;
	ADS1292_DELAY_MS(1);
	HAL_SPI_Transmit(&hspi3, &tx[0], 1,1000);
	HAL_SPI_Transmit(&hspi3, &tx[1], 1,1000);
	HAL_SPI_Receive(&hspi3, rx, 1,1000);
	ADS1292_DELAY_MS(1);
	ADS1292_CS_HIGH;
	
}


void ads1292_send_cmd(uint8_t cmd)
{
	ADS1292_CS_LOW;
	ADS1292_DELAY_MS(1);
	HAL_SPI_Transmit(&hspi3, &cmd, 1,1000);
	ADS1292_DELAY_MS(1);
	ADS1292_CS_HIGH;
}

void ads1292_write_regs(uint8_t regAddress, uint8_t *data, uint8_t len)
{
    if (len == 0) return;

    uint8_t cmd[2];
    cmd[0] = 0x40 | regAddress;
    cmd[1] = len - 1;                  
	
	ADS1292_CS_LOW;
	ADS1292_DELAY_MS(1);
	HAL_SPI_Transmit(&hspi3, &cmd[0], 1,1000);
	HAL_SPI_Transmit(&hspi3, &cmd[1], 1,1000);
    HAL_SPI_Transmit(&hspi3, data, len,1000);
	ADS1292_DELAY_MS(1);
	ADS1292_CS_HIGH;
}

void ads1292_read_regs(uint8_t regAddress, uint8_t *buffer, uint8_t len)
{
    if (len == 0) return;

    uint8_t cmd[2];
    cmd[0] = 0x20 | regAddress;
    cmd[1] = len - 1;
	
	ADS1292_CS_LOW;
	ADS1292_DELAY_MS(1);
	HAL_SPI_Transmit(&hspi3, &cmd[0], 1,1000);
	HAL_SPI_Transmit(&hspi3, &cmd[1], 1,1000);
  HAL_SPI_Receive(&hspi3, buffer, len,1000);
	ADS1292_DELAY_MS(1);
	ADS1292_CS_HIGH;
}

void ads1292_sampling_start()
{
	ads1292_send_cmd(ADS1292_CMD_RDATAC);
    ADS1292_DELAY_MS(1);
    ads1292_send_cmd(ADS1292_CMD_START);
    ADS1292_DELAY_MS(1);
	ADS1292_START_HIGH;
}

void ads1292_read_frame(float* data)
{
  uint8_t raw_data[ADS1292_FRAME_SIZE];
	uint8_t tx_data[ADS1292_FRAME_SIZE] = {0};
  int32_t channel1 = 0, channel2 = 0;

	ADS1292_CS_LOW;
	HAL_SPI_TransmitReceive(&hspi3,tx_data, raw_data, ADS1292_FRAME_SIZE,1);
	ADS1292_CS_HIGH;

    // 解析通道1数据（24位有符号，MSB first）
    channel1 = ((int32_t)(raw_data[3]) << 16) |
               ((int32_t)(raw_data[4]) << 8) |
               (int32_t)(raw_data[5]);

    if (channel1 & 0x800000) channel1 |= 0xFF000000;

    // 解析通道2数据
    channel2 = ((int32_t)(raw_data[6]) << 16) |
               ((int32_t)(raw_data[7]) << 8) |
               (int32_t)(raw_data[8]);

    if (channel2 & 0x800000) channel2 |= 0xFF000000;
	
		data[0] = channel1;
		data[1] = channel2;
}

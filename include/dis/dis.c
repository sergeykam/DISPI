#include "dis.h"

/*
 * ПРОТОТИПЫ
 *
 */
void chipSelect(uint8_t DIS_num);
uint8_t Crc8(uint8_t *pcBlock, uint8_t len);
int get(uint8_t DIS_num, uint8_t cmd);

/*
 * Пакет для чтения с датчика
 */
union read_packet_union {
	struct {
		uint8_t data[4];
		uint8_t crc;
	} read_pos;

	uint8_t read_frame[5];

} read_packet;

/*
 * Пакет для отправки на датчик
 */
union write_packet_union {
	struct {
		uint8_t start_byte;
		uint8_t cmd_number;
		uint8_t data[4];
		uint8_t crc;
	} write_pos;
	uint8_t write_frame[7];

} write_packet;

//static char read_tmp_buf[7];
//static char dummy_byte = 0x55;
//static char tmp = 0x55;
//static uint8_t success = 0;
static uint8_t * usr_data_buf_ptr;

//uint8_t curr_total_att = ATT_TOTAL;
//uint8_t curr_a5_att = ATT_A5;
//uint8_t curr_got_a5_att = ATT_GOT_A5;

// ********************************************************

int DIS_init() {
	if (!bcm2835_init())
		return 1;

	bcm2835_gpio_fsel(DIS_CS1, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(DIS_CS2, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(DIS_CS3, BCM2835_GPIO_FSEL_OUTP);
	bcm2835_gpio_fsel(DIS_CS4, BCM2835_GPIO_FSEL_OUTP);

	chipSelect(DIS_NONE);

	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
	bcm2835_spi_setClockDivider(DIS_CLOCK_DIVIDER);
	bcm2835_spi_chipSelect(BCM2835_SPI_CS_NONE);
	return 0;
}

/*
// ********************************************************
 *
 */
void chipSelect(uint8_t DIS_num) {
	switch (DIS_num) {
		case (DIS1):
			bcm2835_gpio_write(DIS_CS1, LOW);
			bcm2835_gpio_write(DIS_CS2, HIGH);
			bcm2835_gpio_write(DIS_CS3, HIGH);
			bcm2835_gpio_write(DIS_CS4, HIGH);
			break;
		case (DIS2):
			bcm2835_gpio_write(DIS_CS1, HIGH);
			bcm2835_gpio_write(DIS_CS2, LOW);
			bcm2835_gpio_write(DIS_CS3, HIGH);
			bcm2835_gpio_write(DIS_CS4, HIGH);
			break;
		case (DIS3):
			bcm2835_gpio_write(DIS_CS1, HIGH);
			bcm2835_gpio_write(DIS_CS2, HIGH);
			bcm2835_gpio_write(DIS_CS3, LOW);
			bcm2835_gpio_write(DIS_CS4, HIGH);
			break;
		case (DIS4):
			bcm2835_gpio_write(DIS_CS1, HIGH);
			bcm2835_gpio_write(DIS_CS2, HIGH);
			bcm2835_gpio_write(DIS_CS3, HIGH);
			bcm2835_gpio_write(DIS_CS4, LOW);
			break;
		case (DIS_NONE):
		default:
			bcm2835_gpio_write(DIS_CS1, HIGH);
			bcm2835_gpio_write(DIS_CS2, HIGH);
			bcm2835_gpio_write(DIS_CS3, HIGH);
			bcm2835_gpio_write(DIS_CS4, HIGH);
			break;
	}
}

// ********************************************************

int get(uint8_t DIS_num, uint8_t cmd) {
	uint8_t curr_total_att = ATT_TOTAL;
	uint8_t curr_a5_att = ATT_A5;
	uint8_t curr_got_a5_att = ATT_GOT_A5;

	char read_tmp_buf[7];
	char dummy_byte = 0x55;
	char tmp = 0x55;
	uint8_t success = 0;

	// Стандартный стартовый байт
	write_packet.write_pos.start_byte = 0xAA;

	// Запишем команду
	write_packet.write_pos.cmd_number = cmd;

	// Данные - нули
	for (uint8_t i = 0; i < 4; i++) {
		write_packet.write_pos.data[i] = 0x00;
	}
	// Считаем CRC
	write_packet.write_pos.crc = Crc8(write_packet.write_frame, 6);

	// Начинаем пробовать получить 0хА5
	while (curr_total_att) {
		curr_total_att--;
//		printf("Chip select... \n");
		chipSelect(DIS_num);
//		printf("Done Chip select... \n");

		bcm2835_spi_transfernb((char*) write_packet.write_frame, read_tmp_buf, 7);
//		printf("Transferred cmd... \n");

		// ждем получения 0хА5
		while (curr_a5_att) {
			curr_a5_att--;
			bcm2835_spi_transfernb(&dummy_byte, &tmp, 1);
			bcm2835_delay(1);

			// если получили, то перестаем ждать
			if (0xA5 == tmp) {
				break;
			}
		}

		// если был получен А5
		if (tmp == 0xA5) {
//			printf("Got 0xA5 \n");
			// ждем пока пройдут все A5
			while (((tmp == 0xA5) || (tmp == 0x55)) && curr_got_a5_att) {
				curr_got_a5_att--;
				bcm2835_spi_transfernb(&dummy_byte, &tmp, 1);
			}
			// получаем сами данные
			if (curr_got_a5_att) {
//				printf("Got data \n");
				bcm2835_spi_transfernb(&dummy_byte, (char*) read_packet.read_frame, 5);

				success = 1;
				chipSelect(DIS_NONE);
				break;
			}
			curr_got_a5_att = ATT_GOT_A5;
		} else {
			curr_a5_att = ATT_A5;
		}

		chipSelect(DIS_NONE);
	}

	if (success == 1) {
		// скопируем в буфер данные
		for(uint8_t i = 0; i < 4; i++){
			usr_data_buf_ptr[i] = read_packet.read_pos.data[i];
			read_packet.read_pos.data[i] = 0;
		}
		return 0;
	} else {
		printf("Failed to get \n");
		return 1;
	}
}

// ********************************************************

int DIS_getData(uint8_t DIS_num, uint8_t* Rx_buf) {
	usr_data_buf_ptr = Rx_buf;

	return get(DIS_num, CMD_DATA);
}

int DIS_getConf(uint8_t DIS_num, uint8_t* Rx_buf) {
	usr_data_buf_ptr = Rx_buf;

//	printf("DIS_get_conf %d \n", DIS_num);

	return get(DIS_num, CMD_CONF);
}

void DIS_close(void){
	bcm2835_spi_end();
	bcm2835_close();
}

/*
 * CRC 8
 */
uint8_t Crc8(uint8_t *pcBlock, uint8_t len) {
	uint8_t crc = 0xFF;
	uint8_t i;

	while (len--) {
		crc ^= *pcBlock++;

		for (i = 0; i < 8; i++)
			crc = crc & 0x80 ? (crc << 1) ^ 0x31 : crc << 1;
	}

	return crc;
}

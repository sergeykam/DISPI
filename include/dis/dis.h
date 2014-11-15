/*
 * dis_API.h
 *
 *  Created on: 30 сент. 2014 г.
 *      Author: sergey
 */

#ifndef DIS_API_H_
#define DIS_API_H_

#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <bcm2835.h>

/*
 *
 *  КОМАНДЫ
 */
#define CMD_CONF	0x01
#define CMD_DATA	0x02

/*
 * CS settings
*/
#define	DIS_CS1		RPI_V2_GPIO_P1_12
#define DIS_CS2		RPI_V2_GPIO_P1_22
#define	DIS_CS3		RPI_V2_GPIO_P1_16
#define	DIS_CS4		RPI_V2_GPIO_P1_18

// RESET PORT
//#define DIS_RESET_PORT		RPI_V2_GPIO_P1_22

/* CLOCK DIVIDERS
 * BCM2835_SPI_CLOCK_DIVIDER_65536 65536 = 262.144us = 3.814697260kHz
 * BCM2835_SPI_CLOCK_DIVIDER_32768 32768 = 131.072us = 7.629394531kHz
 * BCM2835_SPI_CLOCK_DIVIDER_16384 16384 = 65.536us = 15.25878906kHz
 * BCM2835_SPI_CLOCK_DIVIDER_8192 	8192 = 32.768us = 30.51757813kHz
 * BCM2835_SPI_CLOCK_DIVIDER_4096 	4096 = 16.384us = 61.03515625kHz
 * BCM2835_SPI_CLOCK_DIVIDER_2048 	2048 = 8.192us = 122.0703125kHz
 * BCM2835_SPI_CLOCK_DIVIDER_1024 	1024 = 4.096us = 244.140625kHz
 * BCM2835_SPI_CLOCK_DIVIDER_512 	512 = 2.048us = 488.28125kHz
 * BCM2835_SPI_CLOCK_DIVIDER_256 	256 = 1.024us = 976.5625MHz
 * BCM2835_SPI_CLOCK_DIVIDER_128 	128 = 512ns = = 1.953125MHz
 * BCM2835_SPI_CLOCK_DIVIDER_64 	64 = 256ns = 3.90625MHz
 * BCM2835_SPI_CLOCK_DIVIDER_32 	32 = 128ns = 7.8125MHz
 * BCM2835_SPI_CLOCK_DIVIDER_16 	16 = 64ns = 15.625MHz
 * BCM2835_SPI_CLOCK_DIVIDER_8 	8 = 32ns = 31.25MHz
 * BCM2835_SPI_CLOCK_DIVIDER_4 	4 = 16ns = 62.5MHz
 * BCM2835_SPI_CLOCK_DIVIDER_2 	2 = 8ns = 125MHz, fastest you can get
 * BCM2835_SPI_CLOCK_DIVIDER_1 	1 = 262.144us = 3.814697260kHz, same as 0/65536
*/
#define DIS_CLOCK_DIVIDER 	BCM2835_SPI_CLOCK_DIVIDER_8192

/*
 * ATT_TOTAL	- total attempts to get answer
 * ATT_A5	- attempts to get ready state from DIS
*/

#define ATT_TOTAL 	150 // Всего попыток
#define ATT_A5 		20 // Попыток получить А5
#define ATT_GOT_A5	5 // Сколько раз видим А5 перед считыванием данных

/*
 * VARIABLES
*/
enum DIS_num
{
	DIS1 = 1,
	DIS2,
	DIS3,
	DIS4,
	DIS_NONE,
};

/*
 * API FUNCTIONS
 */
int DIS_init();
int DIS_getData (uint8_t DIS_num, uint8_t* Rx_buf);
int DIS_getConf (uint8_t DIS_num, uint8_t* Rx_buf);
void DIS_close(void);

#endif /* DIS_API_H_ */

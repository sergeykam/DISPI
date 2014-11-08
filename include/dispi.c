#include <getopt.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "dis_API.h"

static uint8_t tmp[4];


int main(int argc, char **argv) {
	uint8_t DIS_num;
	uint8_t cmd;
	int opt; // каждая следующая опция попадает сюда
	extern char *optarg;

	if (argc == 1) { // если запускаем без аргументов, выводим справку
		printf("dis\n");
		printf("usage:\n");
		printf(
				"specify -n as dis number (1 - 4) and -c to get configuration or -d - data)\n");
		printf("example:\n");
		printf(" $ dis -n 1 -d\n");
		printf(" get data from 1st DIS\n");
		return 1;
	}

	// разбираем параметры
	while ((opt = getopt(argc, argv, "n:cd")) != -1) { // вызываем getopt пока она не вернет -1
		switch (opt) {
		case 'n':
			DIS_num = atoi(optarg);
			break;
		case 'c':
			cmd = 1; // команда конфигурация
			break;
		case 'd':
			cmd = 2; // команда данных
			break;
		}
	}

	// проверим правильность номера датчика
	if ((DIS_num < 1) || (DIS_num > 4)) {
		printf("DIS number must be 1..4\n");
		return 1;
	}

	if (DIS_init()) {
		printf("DIS initialization failed\n");
		return 1;
	}

	// выполняем команду
	if (cmd == 1) {
		if(!DIS_getConf(DIS_num, tmp)){

		} else {
			return 1;
		}
	} else if (cmd == 2) {
		if(!DIS_getData(DIS_num, tmp)){

		} else {
			return 1;
		}
	} else {
		printf("Wrong command specified %d\n", cmd);
		printf("-c - get config, -d - get data\n");
		return 1;
	}

	return 0;
}

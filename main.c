#define _XOPEN_SOURCE

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>

#include "dis_db.h"
#include "dis.h"

/*
 * DEFINE SECTION
 */
#define CONF_ASK_PERIOD 55
#define DATA_ASK_PERIOD 10
#define TIMER_MAX_TICK	60
#define MAX_TASKS		20
#define SUSPEND_ATTEMPTS		20

#define MAX(x,y) (x > y)? x : y

/*
 * Статус датчика
 */
typedef enum {
	DIS_STATUS_ACTIVE = 0, // активен
	DIS_STATUS_SUSPENDED,  // недоступен (повис)
	DIS_STATUS_NONE		// нет
} dis_status_t;

/* Режимы работы программы */
typedef enum {
	IDLE = 0,
	DATA,
	CONF,
	BUSY,
	NONE
} status_t;
/*
 * Дескриптор задачи
 */
//typedef struct st_task {
//	mode_t state;
//	mode_t state_next;
//	void * handler(void);
//} task_t;

/*
 * Дескриптор датчика
 */
typedef struct st_dis {
	uint8_t num;
	dis_status_t status;
} dis_t;

/*
 * ПРОТОТИПЫ
 */
//void task_init(void);
//uint8_t task_add(mode_t state, mode_t state_next, void *handler(void));
//void task_count(void);
void clr_buf(void);

/*
 * Обработчики сигналов
 */
void timer_handler (int signum);
void int_handler (int signum);

/*
 * Обработчики задач
 */
void data_cout(void);
void conf_cout(void);

/*
 * Валидация данных
 */
uint8_t validate_data(void);
uint8_t validate_conf(void);

// Расписание
void schedule(void);


/*
typedef struct st_dis_db_config {
	uint8_t dis_num;
	uint8_t sensor_type;
	uint8_t calibrated;
	uint8_t gas;
	uint8_t dim;
	uint8_t voltage;
} DIS_DB_CONF;
 */
static DIS_DB_CONF config;

// Режимы работы
status_t status = CONF;
status_t status_next = NONE;
static uint8_t data_buf[4];
union
{
	float data_var;
	uint8_t data_buf[4];
} data;
static uint8_t timer_cnt = 1;

// Инициализация матрицы датчиков
static dis_t dis[4];

//static task_t task_stack[MAX_TASKS];
int error;

int main(int argc, char **argv) {
	struct sigaction sa;
	struct sigaction sint;
	struct itimerval timer;

	/* Инициализация базы данных */
	if(dis_db_init()){
		printf("Failed to connect database \n");
		return 1;
	}

	/* Инициализация DIS */
	if(DIS_init()){
		printf("Failed to init DIS \n");
		return 1;
	}

	/* Назначим функции обработчика таймера для данных */
	memset(&sa, 0, sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM, &sa, NULL);

	/* Назначим функции обработчика ctrl c */
	memset(&sint, 0, sizeof(sint));
	sint.sa_handler = &int_handler;
	sigaction(SIGINT, &sint, NULL);

	/* Установим таймер на 1 сек */
	timer.it_value.tv_sec = 1;
	timer.it_value.tv_usec = 0;
	/* Интервал таймера */
	timer.it_interval.tv_sec = 1;
	timer.it_interval.tv_usec = 0;

	// инициализация матрицы датчиков
	dis[0].num = DIS1;
	dis[0].status = DIS_STATUS_NONE;

	dis[1].num = DIS2;
	dis[1].status = DIS_STATUS_NONE;

	dis[2].num = DIS3;
	dis[2].status = DIS_STATUS_NONE;

	dis[3].num = DIS4;
	dis[3].status = DIS_STATUS_NONE;

	/* Запуск таймера */
	setitimer(ITIMER_REAL, &timer, NULL);

	while(1){
		schedule();
		data_cout();
		conf_cout();
	}
}

/*
 * Переключатор состояний
 */

void schedule(void){
	if(0 == (timer_cnt % DATA_ASK_PERIOD)){
		if(IDLE == status){
			status = DATA;
		}
	} else if(0 == (timer_cnt % CONF_ASK_PERIOD)){
		if(IDLE == status){
			status = CONF;
		}
	}

//	if(0 == (timer_cnt % CONF_ASK_PERIOD)){
//		task_add(CONF, mode_t state_next, void *handler(void));
//	}
}

/*
 * Таймер
 */
void timer_handler (int signum) {
	/* Переключатор режимов */
	if(NONE != status_next){
		status = status_next;
		status_next = NONE;
	}

	/* Тик таймера */
	if(timer_cnt >= TIMER_MAX_TICK){
		timer_cnt = 1;
	} else {
		timer_cnt++;
	}

	printf("Timer tick... %d \n", timer_cnt);
}

void data_cout(void){
	if(DATA == status){
		printf("Asking data \n");
		status = BUSY;

		for(uint8_t i = 0; i < 1; i++){
			printf("Asking data from %d DIS, \n", dis[i].num);
			if(dis[i].status == DIS_STATUS_ACTIVE){
				if(!DIS_getData(dis[i].num, data_buf) && validate_data()){
					printf("Got data %X, %X, %X, %X, \n", data_buf[0],data_buf[1],data_buf[2],data_buf[3]);

					memcpy(data.data_buf, data_buf, 4);
					printf("Got data %f \n", data.data_var);
					clr_buf();

					error = dis_db_set_data(dis[i].num, data.data_var);

					if(error){
						printf("Failed to set data error: %d \n", error);
					}
				}
			}
		}

		status_next = IDLE;
	}
}

void conf_cout(void){
	if(CONF == status){
		status = BUSY;

		for(uint8_t i = 0; i < 1; i++){
			printf("Asking conf from %d DIS \n", dis[i].num);

			if(!DIS_getConf(dis[i].num, data_buf)){
				printf("Got config %X, %X, %X, %X \n", data_buf[0],data_buf[1],data_buf[2],data_buf[3]);

				config.dis_num = dis[i].num;

				config.sensor_type = data_buf[0];
				config.gas = data_buf[1];
				config.dim = data_buf[2];
				config.voltage = data_buf[3];

				clr_buf();

				if(validate_conf()){
					error = dis_db_set_conf(&config);

					if(error){
						dis[i].status = DIS_STATUS_NONE;
						dis_db_del_conf(dis[i].num);
						printf("Failed to set conf error: %d \n", error);
					} else {
						dis[i].status = DIS_STATUS_ACTIVE;
					}
				} else {
					printf("Not valid CONF \n");
				}
			} else {
				printf("Failed to get conf DIS #%d \n", dis[i].num);
				dis[i].status = DIS_STATUS_NONE;
				dis_db_del_conf(dis[i].num);
			}
		}

		status_next = IDLE;
	}
}

/*
 * Валидация данных
 */
uint8_t validate_data(void){
	uint8_t validated = 1;

	return validated;
}

/*
 * Валидация конфигурации
 */
uint8_t validate_conf(void){
	uint8_t validated = 1;

	/* проверим номер датчика */
	if((config.dis_num == 0) || (config.dis_num > 4)){
		printf("Wrong DIS num (%d) \n", config.dis_num);
		validated = 0;
	}

	// check if calibrated
	if(config.sensor_type & 0x80){
		config.sensor_type &= 0x7F;
		config.calibrated = 1;
	} else {
		config.calibrated = 0;
	}

	/* тип элемента */
	if(++config.sensor_type > 0x06){
		printf("Wrong sensor type  (%d) \n", config.sensor_type);
		validated = 0;
	}

	/* тип газа */
	if(++config.gas > 0x13){
		printf("Wrong gas (%d) \n", config.gas);
		validated = 0;
	}

	/* размерность */
	if(++config.dim > 0x07){
		printf("Wrong dim \n");
		validated = 0;
	}
	/* питание */
	switch (config.voltage){
		case 0x1E:
			config.voltage = 1;
			break;
		case 0x32:
			config.voltage = 2;
			break;
		default:
			printf("Wrong voltage \n");
			validated = 0;
			break;
	}

	return validated;
}

void int_handler (int signum){
	printf("Closing connection \n");
	status = NONE;
	dis_db_close_conn();
	DIS_close();
	exit(1);
}

/*
 * Очистка буфера с данными
 */
void clr_buf(void){
	uint8_t i;

	for(i = 0; i < 4; i++){
		data_buf[i] = 0;
	}
}

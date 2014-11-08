/*
 * dis_API.h
 *
 *  Created on: 30 сент. 2014 г.
 *      Author: sergey
 */

#ifndef DIS_DB_H_
#define DIS_DB_H_

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include <mysql/mysql.h>

/*
 *
 *  НАСТРОЙКИ БАЗЫ ДАННЫХ
 */

#define DB_NAME		"DIS"
#define DB_USER		"root"
#define DB_HOST		"localhost"
#define DB_PASS		"password"

typedef struct st_dis_db_config {
	uint8_t dis_num;
	uint8_t sensor_type;
	uint8_t calibrated;
	uint8_t gas;
	uint8_t dim;
	uint8_t voltage;
} DIS_DB_CONF;


/*
 * API FUNCTIONS
 */
int dis_db_init();
int dis_db_set_data(uint8_t dis_num, float data_buf);
int dis_db_set_conf(DIS_DB_CONF * config);
int dis_db_del_conf(uint8_t dis_num);
void dis_close_conn(void);

#endif /* DIS_API_H_ */

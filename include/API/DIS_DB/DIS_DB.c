#include "DIS_DB.h"

/*
 * ПРОТОТИПЫ
 *
 */


MYSQL *mysql;

char insert_data_row_format[] = "INSERT INTO Data (id, created_at, data, conf_id) VALUES (NULL, CURRENT_TIMESTAMP, %f, %d)";
char insert_data_row[256];

char insert_conf_row_format[] = "INSERT INTO Config (id, gas_id, sensor_id, voltage_id, dimension_id, calibrated, created_at) VALUES (%d, %d, %d, %d, %d, %d, CURRENT_TIMESTAMP)";
char insert_conf_row[256];

char delete_conf_row_format[] = "DELETE FROM Config WHERE id = %d";
char delete_conf_row[256];

int dis_db_init(){
	mysql = mysql_init(NULL);
	mysql_options(mysql, MYSQL_OPT_RECONNECT, "true");

	if(mysql_real_connect(mysql,DB_HOST,DB_USER,DB_PASS,DB_NAME,0,NULL,0) == NULL){
		return 1;
	} else {
		return 0;
	}
}

int dis_db_set_data(uint8_t dis_num, float data_buf){
	uint8_t rval = 0;

	sprintf(insert_data_row, insert_data_row_format, data_buf, dis_num);

	if (mysql_query(mysql,insert_data_row))	{
		printf("%s \n", mysql_error(mysql));
		rval = 2;
	}
	return rval;
}

int dis_db_set_conf(DIS_DB_CONF * config){
	uint8_t rval = 0;

	// преобразуем строку в запрос на удаление
	sprintf(delete_conf_row, delete_conf_row_format, config->dis_num);
	sprintf(insert_conf_row, insert_conf_row_format
			, config->dis_num
			, config->gas
			, config->sensor_type
			, config->voltage
			, config->dim
			, config->calibrated);

	// удалим ненужную конфигурацию
	if (mysql_query(mysql,delete_conf_row)){

		rval = 2;
	}
	// вставим новую конфигурацию
	if ((rval == 0) && mysql_query(mysql,insert_conf_row)){
		printf("%s \n", mysql_error(mysql));
		rval = 2;
	}

	return rval;
}

int dis_db_del_conf(uint8_t dis_num){
	sprintf(delete_conf_row, delete_conf_row_format, dis_num);

	return mysql_query(mysql,delete_conf_row);
}

void dis_close_conn(void){
	mysql_close(mysql);
}

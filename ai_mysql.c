#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
/*引入连接Mysql的头文件*/
#include "/usr/include/mysql/mysql.h"
#include "ai_mysql.h"

/*定义一些数据库连接需要的宏*/
#define MYSQL_HOST "10.21.19.17" /*MySql服务器地址*/
#define MYSQL_USERNAME "root" /*用户名*/
#define MYSQL_PASSWORD "root6316" /*数据库连接密码*/
#define MYSQL_DATABASE "freeswitch" /*需要连接的数据库*/

struct ai_mysql {
	MYSQL *conn;
	pthread_mutex_t lock;

};



ai_mysql* ai_mysql_conn_new(char* host, char* name, char*pwd,char* dtb) {
	struct ai_mysql *ai_mysql;
	ai_mysql = (struct ai_mysql *)malloc(sizeof(struct ai_mysql));
	if (ai_mysql == NULL) {
		return NULL;
	}
	mysql_thread_init();
	ai_mysql->conn = mysql_init(NULL);
	ai_mysql->conn->reconnect = 1;

	pthread_mutex_init(&ai_mysql->lock, NULL);

	if (ai_mysql->conn == NULL) {
		printf("Error: mysql_init failed!\n");
		return ai_mysql;
	}

	if (mysql_real_connect(ai_mysql->conn, host, name, pwd, dtb, 0, NULL, CLIENT_FOUND_ROWS) == NULL) {
		printf("Database connection failed! Error: %s\n", mysql_error(ai_mysql->conn));
		mysql_close(ai_mysql->conn);
		ai_mysql->conn = NULL;
	}
	else {
		mysql_query(ai_mysql->conn, "set names utf8");
	}
	return ai_mysql;

}

//查询
ai_sql_res* ai_mysql_query(ai_mysql *ai_mysql, char* sql) {
	int res;
	struct ai_sql_res *ai_sql_res;

	
	if (ai_mysql->conn == NULL) {
		printf("Error: conn is null !\n");
		return NULL;
	}
	pthread_mutex_lock(&ai_mysql->lock);
	res = mysql_query(ai_mysql->conn, sql);
	pthread_mutex_unlock(&ai_mysql->lock);
	if (res) {
		printf("Error: mysql_query !\n");
		return NULL;
	}
	else {
		ai_sql_res =  ai_sql_res_new(mysql_store_result(ai_mysql->conn));
	}

	return ai_sql_res;
}
//更新 删除 等
int ai_mysql_exe(ai_mysql *ai_mysql, char* sql) {
	int res;
	int affected_rows = -1;
	pthread_mutex_lock(&ai_mysql->lock);
	res = mysql_query(ai_mysql->conn, sql);
	pthread_mutex_unlock(&ai_mysql->lock);
	if (res) {
		return res;
	}
	return 0;

}
//销毁
void ai_sql_res_dealloc(ai_sql_res* ai_sql_res) {
	if (ai_sql_res_get_result(ai_sql_res) != NULL) {
		mysql_free_result(ai_sql_res_get_result(ai_sql_res));
		ai_sql_res_set_result(ai_sql_res, NULL);
	}
	free(ai_sql_res);
}
//销毁
void ai_mysql_dealloc(ai_mysql *ai_mysql) {
	if (ai_mysql->conn != NULL) {
		mysql_close(ai_mysql->conn);
	}
	mysql_thread_end();
	free(ai_mysql);
}








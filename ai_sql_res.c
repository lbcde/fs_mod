#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*引入连接Mysql的头文件*/
#include "/usr/include/mysql/mysql.h"


typedef struct ai_sql_res {
	MYSQL_RES *result;
}ai_sql_res;


MYSQL_RES* ai_sql_res_get_result(ai_sql_res* res) {
	return res->result;
}

void ai_sql_res_set_result(ai_sql_res* res, MYSQL_RES* new_result) {
	res->result = new_result;
}


ai_sql_res*  ai_sql_res_new(void* data) {
	struct ai_sql_res *ai_sql_res;
	ai_sql_res = (struct ai_sql_res *)malloc(sizeof(struct ai_sql_res));
	if (ai_sql_res == NULL) {
		return NULL;
	}
	ai_sql_res->result = (MYSQL_RES*)data;

	return ai_sql_res;
}

int row_count(ai_sql_res* sql_res) {
	if (sql_res==NULL)
	{
		return 0;
	}
	return mysql_num_rows(sql_res->result);
	
}

int column_count(ai_sql_res* sql_res) {
	return mysql_num_fields(sql_res->result);
}

char* char_res(ai_sql_res* sql_res, int row, int col) {
	int i;
	MYSQL_ROW mysql_row=NULL;

	// 跳到结果集的开始位置
	mysql_data_seek(sql_res->result, 0);

	// 遍历结果集，找到目标行
	for (i = 0; i <= row; i++) {
		mysql_row = mysql_fetch_row(sql_res->result);
		if (mysql_row == NULL) {
			fprintf(stderr, "Error: Row index out of range.\n");
			return NULL;
		}
	}

	// 返回目标行中目标列的字符串
	return mysql_row[col];
}

int int_res(ai_sql_res* sql_res, int row, int col) {
	int i;
	MYSQL_ROW mysql_row=NULL;

	// 跳到结果集的开始位置
	mysql_data_seek(sql_res->result, 0);

	// 遍历结果集，找到目标行
	for (i = 0; i <= row; i++) {
		mysql_row = mysql_fetch_row(sql_res->result);
		if (mysql_row == NULL) {
			fprintf(stderr, "Error: Row index out of range.\n");
			return 0;
		}
	}

	// 返回目标行中目标列的字符串
	return atoi(mysql_row[col]);
}



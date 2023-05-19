#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*引入连接Mysql的头文件*/
#include "/usr/include/mysql/mysql.h"


#ifndef MYSQL_EXESQL_RES_H
#define MYSQL_EXESQL_RES_H

typedef struct ai_sql_res ai_sql_res;



//辅助访问ai_sql_res结构体
MYSQL_RES* ai_sql_res_get_result(ai_sql_res* res);
//修改结构体内值
void ai_sql_res_set_result(ai_sql_res* res, MYSQL_RES* new_result);


//初始化资源
ai_sql_res*  ai_sql_res_new(void* data);
//返回行数
int row_count(ai_sql_res* sql_res);
//返回列数
int column_count(ai_sql_res* sql_res);
//返回字符串类型
char* char_res(ai_sql_res* sql_res,int row, int col);
//返回整型
int int_res(ai_sql_res* sql_res,int row, int col);



#endif // MYSQL_EXESQL_H

//查询语句
//SELECT * FROM ai_ivvr_res WHERE file_state = 0;



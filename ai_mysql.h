#ifndef MYSQL_EXESQL_H
#define MYSQL_EXESQL_H
#include "ai_sql_res.h"


typedef struct ai_mysql ai_mysql;

/*初始化连接*/
ai_mysql* ai_mysql_conn_new(char* host,char* name,char* pwd, char* dtb);

//查询
ai_sql_res* ai_mysql_query(ai_mysql *ai_mysql, char* sql);
//更新 删除 等
int ai_mysql_exe(ai_mysql *ai_mysql, char* sql);
//销毁res
void ai_sql_res_dealloc(ai_sql_res* ai_sql_res);
//销毁
void ai_mysql_dealloc(ai_mysql *ai_mysql);


#endif // MYSQL_EXESQL_H
#ifndef MYSQL_EXESQL_H
#define MYSQL_EXESQL_H
#include "ai_sql_res.h"


typedef struct ai_mysql ai_mysql;

/*��ʼ������*/
ai_mysql* ai_mysql_conn_new(char* host,char* name,char* pwd, char* dtb);

//��ѯ
ai_sql_res* ai_mysql_query(ai_mysql *ai_mysql, char* sql);
//���� ɾ�� ��
int ai_mysql_exe(ai_mysql *ai_mysql, char* sql);
//����res
void ai_sql_res_dealloc(ai_sql_res* ai_sql_res);
//����
void ai_mysql_dealloc(ai_mysql *ai_mysql);


#endif // MYSQL_EXESQL_H
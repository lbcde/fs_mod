#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*��������Mysql��ͷ�ļ�*/
#include "/usr/include/mysql/mysql.h"


#ifndef MYSQL_EXESQL_RES_H
#define MYSQL_EXESQL_RES_H

typedef struct ai_sql_res ai_sql_res;



//��������ai_sql_res�ṹ��
MYSQL_RES* ai_sql_res_get_result(ai_sql_res* res);
//�޸Ľṹ����ֵ
void ai_sql_res_set_result(ai_sql_res* res, MYSQL_RES* new_result);


//��ʼ����Դ
ai_sql_res*  ai_sql_res_new(void* data);
//��������
int row_count(ai_sql_res* sql_res);
//��������
int column_count(ai_sql_res* sql_res);
//�����ַ�������
char* char_res(ai_sql_res* sql_res,int row, int col);
//��������
int int_res(ai_sql_res* sql_res,int row, int col);



#endif // MYSQL_EXESQL_H

//��ѯ���
//SELECT * FROM ai_ivvr_res WHERE file_state = 0;



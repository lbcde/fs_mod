
#ifndef AI_CACULATE_H
#define AI_CACULATE_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>



//�����ַ�����md5ֵ
char *string_md5(const char *input_str);
//�������͵�md5ֵ
char *int_md5(int input_int);
//�������ַ�����������
char *concatenate_strings(const char *str1, const char *str2);



#endif //AI_CACULATE_H
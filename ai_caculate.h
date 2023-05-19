
#ifndef AI_CACULATE_H
#define AI_CACULATE_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <openssl/md5.h>



//计算字符串的md5值
char *string_md5(const char *input_str);
//计算整型的md5值
char *int_md5(int input_int);
//将三个字符串连接起来
char *concatenate_strings(const char *str1, const char *str2);



#endif //AI_CACULATE_H
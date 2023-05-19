#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <openssl/md5.h>



char *string_md5(const char *input_str) {
	unsigned char digest[MD5_DIGEST_LENGTH];
	static char output_str[(MD5_DIGEST_LENGTH * 2) + 1];
	int i;
	MD5((unsigned char *)input_str, strlen(input_str), (unsigned char *)&digest);
	for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&output_str[i * 2], "%02x", (unsigned int)digest[i]);
	}
	return output_str;
}

char *int_md5(int input_int) {
	unsigned char digest[MD5_DIGEST_LENGTH];
	static char output_str[(MD5_DIGEST_LENGTH * 2) + 1];
	int i;
	// 将 int 转换为字符串
	char input_str[12]; // 一个整数最多有 10 位（包括负号），加上末尾的 '\0'
	snprintf(input_str, sizeof(input_str), "%d", input_int);
	MD5((unsigned char *)input_str, strlen(input_str), (unsigned char *)&digest);
	for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
		sprintf(&output_str[i * 2], "%02x", (unsigned int)digest[i]);
	}
	return output_str;
}

char *concatenate_strings(const char *str1, const char *str2) {
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);
	
	size_t total_len = len1 + len2 ;

	char *result = (char *)malloc(total_len + 1); // +1 是为了存放字符串结尾的 '\0'

	if (result == NULL) {
		printf("Memory allocation failed.\n");
		return NULL;
	}

	strcpy(result, str1);
	strcat(result, str2);

	return result;
}


/*
int main() {
	const char *input_str = "Hello, World!";
	char *md5_str1 = string_md5(input_str);
	int input_int = 42;
	char *md5_str2 = int_md5(input_int);
	const char *str1 = "Hello, ";
	const char *str2 = "World";
	const char *str3 = "!";
	printf("Input string: %s\n", input_str);
	printf("MD5: %s\n", md5_str1);
	printf("Input integer: %d\n", input_int);
	printf("MD5: %s\n", md5_str2);
	char *concatenated_str = concatenate_strings(str1, str2, str3);
	if (concatenated_str != NULL) {
		printf("Concatenated string: %s\n", concatenated_str);
		free(concatenated_str); // 记得释放内存
	}
	return 0;
}
*/
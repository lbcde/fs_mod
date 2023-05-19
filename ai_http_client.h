
#ifndef _HTTP_CLIENT_H_
#define _HTTP_CLIENT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/un.h>
#include <signal.h>
#include <sys/prctl.h>
#include <pthread.h>
#ifdef HAVE_LIBSSL
#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/crypto.h>
#endif

typedef struct http_response {
    int    code;
    char*  errMsg;
    //数据
    char*  data;
    //长度
    int    length;
    //用户信息
	void*  userData;
	void*  ext;

}http_response;

typedef struct ai_http_client ai_http_client;

/**
 * 生成http_client
 */
ai_http_client *ai_http_client_new();

/**
 * post 请求
 */
int ai_http_post(ai_http_client* client,char* url,char* header,char* param,void* userData);

/**
 * 文件下载接口
 */
int ai_download(ai_http_client* client,char* url,char* header,char* param,void* userData,void* ext,char*des_path);


/**
 * get 请求
 */
int ai_http_get(ai_http_client* client,char* url,char* header,char* param,void* userData);


/**
 * 请求结束回调
 */
void ai_http_on_request_complete(ai_http_client* client,void (*onComplete)(ai_http_client *c,http_response* response));



/**
 * stream 回调
 */
void ai_http_on_stream_write(ai_http_client* client,void (*onStreamWrite)(ai_http_client *c,void *ptr,int len));

/**
 * 销毁对象
 */
void ai_http_dealloc(ai_http_client* client);





#endif /* _HTTP_CLIENT_H_ */


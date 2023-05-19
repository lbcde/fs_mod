
#include <curl/curl.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <stddef.h>
#include <stdbool.h>
#include "ai_http_client.h"
#define HTTP_RCVBUF_SIZE_MAX            (2*1024*1024)

void * ai_http_client_run_thread(void *ptr);
size_t ai_http_client_write_stream(void *ptr, size_t size, size_t nmemb, void *stream);
int start_request(ai_http_client* client, char* url, char* header, char* param, char* method, void* userData);

struct ai_http_client {

    char* method;
    //请求地址
    char* url;
    //请求参数
    char* param;
    //结果
    char *rcvBuffer;
    //缓冲区长度
    int rcvBufLen;
    //接收数据大小
    int rcvSize;
    //文件存储全路径
    char* des_path;
	void* userData;
    void* ext;
	FILE* fp;
    //请求结束
	void(*onComplete)(ai_http_client *c, http_response *response);
	void(*onStreamWrite)(ai_http_client *c, void *ptr, int len);
	struct curl_slist *headers;
    //thread lock
    pthread_mutex_t lock;
    pthread_t run_thread;


    
};


ai_http_client *ai_http_client_new(){

    ai_http_client *client = NULL;
    client = (ai_http_client*)malloc(sizeof(ai_http_client));
    if(!client) {
        //fprintf(stderr, "Unable to allocate memory in libwsclient_new.\n");
        exit(-1);
    }
    memset(client, 0, sizeof(ai_http_client));
    if(pthread_mutex_init(&client->lock, NULL) != 0) {
        exit(-2);
    }
	curl_global_init(CURL_GLOBAL_ALL);
    return client;
}

int ai_http_post(ai_http_client* client,char* url,char* header,char* param,void* userData){
    return start_request(client,url,header,param,"POST",userData);
}

int ai_http_get(ai_http_client* client,char* url,char* header,char* param,void* userData){
    return start_request(client,url,header,param,"GET",userData);
}

int ai_download(ai_http_client* client,char* url,char* header,char* param,void* userData,void* ext,char*des_path){
	FILE *fp;
    client->des_path = (char *)malloc(strlen(des_path)+1);
    memset(client->des_path, 0, strlen(des_path)+1);
	client->ext = ext;
    strncpy(client->des_path, des_path, strlen(des_path));
	if (client->des_path != NULL)
	{
		fp = fopen(client->des_path, "wb");
		client->fp = fp;
	}

    return start_request(client,url,header,param,"GET",userData);
}

void ai_http_on_request_complete(ai_http_client* client,void (*onComplete)(ai_http_client *c,http_response* response)){
    client->onComplete = onComplete;
}


void ai_http_on_stream_write(ai_http_client* client,void (*onStreamWrite)(ai_http_client *c,void *ptr,int len)){
    client->onStreamWrite = onStreamWrite;
}


void ai_http_dealloc(ai_http_client* client){
	if (client == NULL) {
		return;
	}
	if (client->method != NULL) {
		free(client->method);
	}
	if (client->url != NULL) {
		free(client->url);
	}
	if (client->param != NULL) {
		free(client->param);
	}
	if (client->rcvBuffer != NULL) {
		free(client->rcvBuffer);
	}
	if (client->des_path != NULL) {
		free(client->des_path);
	}
	if (client->headers != NULL) {
		curl_slist_free_all(client->headers);
	}
	if (client->fp != NULL) {
		fclose(client->fp);
	}
	pthread_mutex_destroy(&client->lock);
	free(client);
}


size_t ai_http_client_write_stream(void *ptr, size_t size, size_t nmemb, void *stream)
{

	
    //int writeLength = 0;
    int written  = size*nmemb;
	ai_http_client *client = (ai_http_client*)stream;

	//printf("写文件 url =%s 。。。。。。\n", client->url);
	client->rcvSize += written;
    if (client->fp!=NULL)
    {
        written = fwrite(ptr, size, nmemb, client->fp);
        return written;
    }

	

    if (client!=NULL&&client->onStreamWrite!=NULL) {
        client->onStreamWrite(client,ptr,written);
        return written;
    }

   if(client->rcvBuffer != NULL && (written <= (client->rcvBufLen - client->rcvSize)))
   {
       int writeLength = (written <= (client->rcvBufLen - client->rcvSize))?(written):(client->rcvBufLen - client->rcvSize);
       memcpy((client->rcvBuffer) +client->rcvSize, ptr, writeLength);
       client->rcvSize += writeLength;
   }

   memcpy((client->rcvBuffer) +client->rcvSize, ptr, written);

   return written;
    
}




int start_request(ai_http_client* client,char* url,char* header,char* param,char* method,void* userData){
    client->rcvSize    = 0;
    client->rcvBuffer = (char *)malloc(HTTP_RCVBUF_SIZE_MAX);
    memset(client->rcvBuffer,0,HTTP_RCVBUF_SIZE_MAX);
    client->rcvBufLen = 0;
    client->url = (char *)malloc(strlen(url)+1);
    memset(client->url, 0, strlen(url)+1);
    strncpy(client->url, url, strlen(url));
    client->method = (char *)malloc(strlen(method)+1);
    memset(client->method, 0, strlen(method)+1);
    strncpy(client->method, method, strlen(method));
    client->userData = userData;

    if (param!=NULL) {
        client->param = (char *)malloc(strlen(param)+1);
        memset(client->param, 0, strlen(param)+1);
        strncpy(client->param, param, strlen(param));
    }
    
    if (client->url==NULL) {
        exit(-2);
        return 0;
    }
    // if ((pthread_create(&client->run_thread, NULL, ai_http_client_run_thread, (void *)client))== -1)
    // {
    //     return 0;
    // }

    ai_http_client_run_thread(client);
	return 1;
}


void *ai_http_client_run_thread(void *ptr) {
	
    ai_http_client *client = (ai_http_client *)ptr;
    CURL *curl = NULL;
    
    curl = curl_easy_init();
    if(curl)
    {
	
        http_response *reponse = NULL;
		struct curl_slist *headers= NULL;
		CURLcode code = (CURLcode)0;
		CURLcode response_code = (CURLcode)-1;
        reponse = (http_response *)malloc(sizeof(http_response));
        reponse->code = 200;

        //设置header
        
		
        curl_easy_setopt(curl, CURLOPT_URL,client->url);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER,0L); // if want to use https
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST,0L); // set peer and host verify false
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 0);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
        curl_easy_setopt(curl, CURLOPT_HEADER, 1);
        //curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);//允许重定向
        curl_easy_setopt(curl, CURLOPT_HEADER, 0L);
        curl_easy_setopt(curl, CURLOPT_FORBID_REUSE, 1); 
        
        //设置请求结果 回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ai_http_client_write_stream);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA,client);
        
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST,client->method);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15l);//请求超时时长
        curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 15l);

        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 1L);
        curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME,  6L);
        

        //设置param
        if (client->param!=NULL) {
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS,client->param);
        }
		
        if (client->headers!=NULL)
        {
            
        }else{
            headers = curl_slist_append(headers, "Content-Type: application/json");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        }
		
        
		

		/*
		**错误表明你正在使用一个不被libcurl支持的协议
		*/
	
        code = curl_easy_perform(curl);
		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		if (code == CURLE_OK && response_code == 200) {
            //下载成功
            reponse->code = 200;
            reponse->data = client->rcvBuffer;
            reponse->length = client->rcvSize;
            reponse->userData = client->userData;
			reponse->ext = client->ext;
            curl_easy_cleanup(curl);
            curl_slist_free_all(headers);

			if (client->rcvSize<0.01) {
				reponse->code = -200;
			}

            if(client->onComplete != NULL) {
                printf("下载结束  onComplete  111 code = %d url = %s。。。。。。\n", reponse->code, client->url);
                client->onComplete(client,reponse);
            }

        }else{
            char* error_str = (char*)curl_easy_strerror(response_code);
            reponse->code = response_code;
			reponse->ext = client->ext;
			reponse->userData = client->userData;
            reponse->errMsg = (char *)malloc(strlen(error_str) + 1);
            memset(reponse->errMsg, 0, strlen(error_str)+1);
            strncpy(reponse->errMsg, error_str, strlen(error_str));
            if(client->onComplete != NULL) {
                printf("下载结束  onComplete  222 code = %d url = %s。。。。。。\n", reponse->code, client->url);
                client->onComplete(client,reponse);
            }


        }
    }else{
        if(client->onComplete != NULL) {
			http_response* reponse;
            reponse = (http_response *)malloc(sizeof(http_response));
            reponse->code = -1;
            reponse->errMsg = "libcurl init failed";
            reponse->userData = client->userData;
			reponse->ext = client->ext;
			printf("下载结束 下载结束  onComplete 222  %s。。。。。。\n", client->url);
            client->onComplete(client,reponse);
        }
    }

    return NULL;
}





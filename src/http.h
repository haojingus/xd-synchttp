#ifndef __HTTP_H_
#define __HTTP_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <errno.h>

#if ( !defined( _WIN32 ) && !defined( _WIN32_WCE ) ) || defined( __linux__ ) || defined( __linux )
# include <arpa/inet.h>
# include <sys/types.h>
# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <netdb.h>
#include <sys/time.h>
typedef int SOCKET;
#elif ( defined( _WIN32 ) || defined( _WIN32_WCE ) ) && !defined( __linux__ ) && !defined( __linux ) 
# include <winsock2.h>
# include <windows.h>
# include <ws2tcpip.h>
#pragma comment (lib, "ws2_32.lib")
# include <Ws2tcpip.h>
#pragma comment(lib,"Ws2_32.lib")
int gettimeofday(struct timeval *tp, void *tzp);
typedef int socklen_t;
#endif

#include <limits.h>
#include <ctype.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>

//#define DEBUG 1

#define HEADERSIZE 4096
#define SENDBUFFSZIE 1024
#define MAXREDIRECT 3
#define MIMESIZE 96
#define URLSIZE 1024

typedef enum
{
	FORMAT_UNKNOWN=0,FORMAT_PNG,FORMAT_JPEG,FORMAT_GIF
} IMAGE_FORMAT; 

typedef enum 
{
	GET=1,POST,PATCH,PUT,DEL
} HTTP_METHOD;

typedef struct
{
	char* t_string;
	int t_size;
	int t_used;
} DSTRING;

typedef struct 
{
	DSTRING* hd_response;//很大 heap
	int hd_size;
	int hd_used;
	int hd_redirect_count;
	HTTP_METHOD hd_method;
	char* hd_response_header;//一般大 heap
	int hd_response_code;
	DSTRING* hd_request_body;//很大 heap
	char hd_request_url[URLSIZE];//不太大 stack
	int hd_timeout;
	char hd_content_type[MIMESIZE];

} HTTPDATA;


#define INIT_DSTRING(string,size) \
	string->t_string = (char*)malloc(sizeof(char)*size);\
	memset(string->t_string,0,sizeof(char)*size);\
	string->t_size = size;\
	string->t_used = 0;

#define FREE_DSTRING(string) \
	free(string->t_string);\
	free(string);

#define INIT_HTTPDATA(data) \
	data->hd_response = (DSTRING*)malloc(sizeof(DSTRING));\
	INIT_DSTRING(data->hd_response,2048);\
	data->hd_redirect_count = 0;\
	data->hd_method = GET;\
	data->hd_response_header = (char*)malloc(HEADERSIZE);\
	data->hd_request_body = (DSTRING*)malloc(sizeof(DSTRING));\
	INIT_DSTRING(data->hd_request_body ,2048);\
	memset(data->hd_response_header,0,sizeof(char)*HEADERSIZE);\
	memset(data->hd_content_type,0,sizeof(char)*MIMESIZE);

#define FREE_HTTPDATA(data) \
	FREE_DSTRING(data->hd_response);\
	FREE_DSTRING(data->hd_request_body);\
	free(data->hd_response_header);\
	free(data);

#if ( !defined( _WIN32 ) && !defined( _WIN32_WCE ) ) || defined( __linux__ ) || defined( __linux )
	#define CLOSE_SOCK(fd) \
			close(fd);
#else
	#define CLOSE_SOCK(fd) \
			closesocket(fd); \
			WSACleanup();
#endif

#ifdef DEBUG
	#define _DEBUG(msg) fprintf(stderr,"%s\n",msg);
#else
	#define _DEBUG(msg);
#endif
void append_dstring(DSTRING* dest,char* source,int size);

//void dstring_append(DSTRING* dest,char* source,int size);

int get_header_value(HTTPDATA* data, const char* field,char* value);



//从2个字符串中间抽取一段
int str_pick(char* source,char* dest,const char* start,const char* end);
void byteToHexStr(const unsigned char* source, char* dest, int sourceLen); 
int byteToInt(const unsigned char* source,int len);
//chunk解码器
void chunk_decode(HTTPDATA* source);

/********************************************
功能：搜索字符串右边起的第一个匹配字符
********************************************/
char * Rstrchr(char * s, char x);
/********************************************
功能：把字符串转换为全小写
********************************************/
void ToLowerCase(char * s);
/**************************************************************
功能：从字符串src中分析出网站地址和端口，并得到用户要下载的文件
***************************************************************/
void get_host(char * src, char * web, char * file, int * port);
int http(HTTPDATA* http_body,char* url,int timeout);
void dump_httpdata(HTTPDATA* ptr);
IMAGE_FORMAT get_image_type(const char* mime);
#endif


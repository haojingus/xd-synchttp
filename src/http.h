#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <limits.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <time.h>
#include <sys/time.h>
#include <assert.h>
#include <fcntl.h>



#define HEADERSIZE 4096
#define SENDBUFFSZIE 1024
#define MAXREDIRECT 3

typedef enum 

{
	GET=1,POST,PATCH,PUT,DELETE
} HTTP_METHOD;

typedef struct 
{
	char* hd_response;//很大 heap
	int hd_size;
	int hd_used;
	int hd_redirect_count;
	HTTP_METHOD hd_method;
	char* hd_response_header;//一般大 heap
	int hd_response_code;
	char* hd_request_body;//很大 heap
	char hd_request_url[1024];//不太大 stack
	int hd_timeout;

} HTTPDATA;

void append_string(HTTPDATA* dest,char* source,int size);
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


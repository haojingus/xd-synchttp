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

#include <node_api.h>
#include "../common.h"


typedef struct 
{
	char* ds_string;
	int ds_size;
	int ds_used;
} DSTRING;

void append_string(DSTRING* dest,char* source,int size)
{
	int len = dest->ds_used;
	//int len = strlen(dest->ds_string);
	if (len+size>dest->ds_size)
	{
		//不够用了重分配
		int buff_size = dest->ds_size+2048+size;
		char* sz_buff = (char*)malloc(buff_size);
		bzero(sz_buff,buff_size);
		memcpy(sz_buff,dest->ds_string,len);
		memcpy(sz_buff+len,source,size);
		free(dest->ds_string);
		dest->ds_string = sz_buff;
		dest->ds_size = buff_size;
		
	}
	else
	{
		//够用就直接拷贝
		memcpy(dest->ds_string+len,source,size);
	}
	dest->ds_used += size;

	//printf("%s\n",dest->ds_string);
	
	return;
}

//从2个字符串中间抽取一段
int str_pick(char* source,char* dest,const char* start,const char* end)
{
	char *ptr_start = strstr(source,start);
	if (ptr_start==NULL)
		return 0;
	char *ptr_end = strstr(ptr_start,end);
	if (ptr_end==NULL)
		return 0;
	int i_start = ptr_start - source + strlen(start);
	int i_end = ptr_end - source;
	memcpy(dest,source+i_start,i_end-i_start);
	return i_start;
}

void byteToHexStr(const unsigned char* source, char* dest, int sourceLen)  
{  
    short i;  
    unsigned char highByte, lowByte;  
    for (i = 0; i < sourceLen; i++)  
    {  
        highByte = source[i] >> 4;  
        lowByte = source[i] & 0x0f ;  
  
        highByte += 0x30;  
  
        if (highByte > 0x39)  
                dest[i * 2] = highByte + 0x07;  
        else  
                dest[i * 2] = highByte;  
  
        lowByte += 0x30;  
        if (lowByte > 0x39)  
            dest[i * 2 + 1] = lowByte + 0x07;  
        else  
            dest[i * 2 + 1] = lowByte;  
    }  
    return ;  
}

int byteToInt(const unsigned char* source,int len)
{
	char bytebuf[4];
	bzero(bytebuf,4);
	byteToHexStr(source,bytebuf,len);
	printf("%s\n",bytebuf);
	int result = (int)strtol(bytebuf,NULL,16);
	return result;
}

//chunk解码器
void chunk_decode(DSTRING* source)
{
	char* ptr_http_header = strstr(source->ds_string,"\r\n\r\n");
	if (ptr_http_header==NULL)
	{
		fprintf(stderr,"Error http header\nData:%s\n",source->ds_string);
		return;
	}
	int body_start_pos = ptr_http_header - source->ds_string+4;
	char *ptr_field_header = strstr(source->ds_string,"Transfer-Encoding: chunked");
	int body_length = 0;
	if (ptr_field_header==NULL)
	{
		//非chunked协议，做定长检查
		char sz_content_length[32];
		int length_pos = str_pick(source->ds_string,sz_content_length,"Content-Length:","\r\n");
		if (length_pos>=body_start_pos||length_pos==0)
			body_length = source->ds_used - body_start_pos;
		else
			body_length = atoi(sz_content_length);
		//printf("Content length:%d\n",body_length);
		memmove(source->ds_string,source->ds_string+body_start_pos,body_length);
		source->ds_used = body_length;
		bzero(source->ds_string+source->ds_used,source->ds_size-source->ds_used);
		
		return;
	}

	char *sz_chunk_header = (char*)malloc(4096);
	int chunk_pos = ptr_field_header - source->ds_string;
	if (body_start_pos-chunk_pos<=0)
		return;
	//游标指向body开始
	char *p_cursor = source->ds_string+body_start_pos;

//	fprintf(stderr,"http header:\n%s\n\n",debug);
	int chunk_header_pos,chunk_size;
	DSTRING* p_chunked_content = (DSTRING*)malloc(sizeof(DSTRING));
	p_chunked_content->ds_string = (char*)malloc(source->ds_size);
	bzero(p_chunked_content->ds_string,source->ds_size);
	p_chunked_content->ds_used = 0;
	p_chunked_content->ds_size = source->ds_size;

	//char *sz_chunk_body = (char*)malloc(128000);
	int chunk_count = 0;
	while(1)
	{
	//printf("chunk index %d\n",chunk_count);	
		chunk_header_pos = strstr(p_cursor,"\r\n")-p_cursor;
		bzero(sz_chunk_header,4096);
		if(chunk_header_pos>4096)
			fprintf(stderr,"ERROR memcpy header pos is %d\n",chunk_header_pos);
		memcpy(sz_chunk_header,p_cursor,chunk_header_pos);
		//strncpy(sz_chunk_header,p_cursor,chunk_header_pos);
		chunk_size = (int)strtol(sz_chunk_header,NULL,16);
		if (chunk_size<=0)
			break;

		//bzero(sz_chunk_body,128000);
		p_cursor += chunk_header_pos+2;
		//if(chunk_size>128000)
        //     printf("ERROR memcpy chunk size:%d\n",chunk_size);
		//memcpy(sz_chunk_body,p_cursor,chunk_size);
		//strncpy(sz_chunk_body,p_cursor,chunk_size);
		//append_string(p_chunked_content,sz_chunk_body,strlen(sz_chunk_body));
		append_string(p_chunked_content,p_cursor,chunk_size);
		p_cursor += chunk_size;
		chunk_count++;
		//查找下个chunk头
		p_cursor+=2;
	}
	memcpy(source->ds_string,p_chunked_content->ds_string,source->ds_size);
	source->ds_used = p_chunked_content->ds_used;
	free(sz_chunk_header);
	//free(sz_chunk_body);
	free(p_chunked_content->ds_string);
	free(p_chunked_content);
	//printf("\n\n===============\nChunk count:%d================\n",chunk_count);
		
	//e
	//charchunk = strstr(ptr->ds_string,"chunked");
	
}


/********************************************
功能：搜索字符串右边起的第一个匹配字符
********************************************/
char * Rstrchr(char * s, char x)  {
  int i = strlen(s);
  if(!(*s))  return 0;
  while(s[i-1]) if(strchr(s + (i - 1), x))  return (s + (i - 1));  else i--;
  return 0;
}

/********************************************
功能：把字符串转换为全小写
********************************************/
void ToLowerCase(char * s)  {
  while(*s!=0)  
  {
	*s = tolower(*s);
	s++;
  }
  //*s=tolower(*s++);
}

/**************************************************************
功能：从字符串src中分析出网站地址和端口，并得到用户要下载的文件
***************************************************************/
void get_host(char * src, char * web, char * file, int * port)  {
  char * pA;
  char * pB;
  memset(web, 0, sizeof(web));
  memset(file, 0, sizeof(file));
  *port = 0;
  if(!(*src))  return;
  pA = src;
  if(!strncmp(pA, "http://", strlen("http://")))  pA = src+strlen("http://");
  else if(!strncmp(pA, "https://", strlen("https://")))  pA = src+strlen("https://");
  //printf("PA:%s\n",pA);
  pB = strchr(pA, '/');
  //printf("PB:%s\nPA:%s\n",pB,pA);
  if(pB)  {
    memcpy(web, pA, strlen(pA) - strlen(pB));
    if(pB+1)  {
      memcpy(file, pB + 1, strlen(pB) - 1);
      file[strlen(pB) - 1] = 0;
    }
  }
  else  memcpy(web, pA, strlen(pA));
  if(pB)  web[strlen(pA) - strlen(pB)] = 0;
  else  web[strlen(pA)] = 0;
  pA = strchr(web, ':');
  if(pA)  *port = atoi(pA + 1);
  else *port = 80;
  //printf("Web is:%s\n File is:%s\n port is %d\n ",web,file,*port);
}

//http请求
int http(DSTRING* http_body,char* url,int timeout)
{
  int sockfd;
  char buffer[1024];
  struct sockaddr_in server_addr;
  struct hostent *host;
  int portnumber,nbytes;
  char host_addr[256];
  char host_file[1024];
  char local_file[256];
  FILE * fp;
  char request[1024];
  int send_length, total_send, total_recv;
  int i;
  char * pt;

/*
  if(argc!=2)
  {
    fprintf(stderr,"Usage:%s web-address\a\n",argv[0]);
    exit(1);
  }
  */
  //printf("parameter.1 is: %s\n", url);
  //ToLowerCase(url);/*将参数转换为全小写*/
  //printf("lowercase parameter.1 is: %s\n", url);

  bzero(host_file,1024);
  bzero(host_addr,256);
  get_host(url, host_addr, host_file, &portnumber);/*分析网址、端口、文件名等*/
  //printf("webhost:%s\n", host_addr);
  //printf("hostfile:%s\n", host_file);
  //printf("portnumber:%d\n\n", portnumber);

  if((host=gethostbyname(host_addr))==NULL)/*取得主机IP地址*/
  {
    fprintf(stderr,"Gethostname error, %s\n", strerror(errno));
    return -2;
  }

  /* 客户程序开始建立 sockfd描述符 */
  if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)/*建立SOCKET连接*/
  {
    fprintf(stderr,"Socket Error:%s\a\n",strerror(errno));
    return -2;
  }

  /* 客户程序填充服务端的资料 */
  bzero(&server_addr,sizeof(server_addr));
  server_addr.sin_family=AF_INET;
  server_addr.sin_port=htons(portnumber);
  server_addr.sin_addr=*((struct in_addr *)host->h_addr);




  //修改为异步+select模式
  int flags = fcntl(sockfd, F_GETFL, 0);
  if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0)
  {
	fprintf(stderr,"Handle error by fcntl\n");
	return -2;
  }

  fd_set rfds,wfds; 
  FD_ZERO(&rfds);
  FD_ZERO(&wfds);

  FD_SET(sockfd, &rfds);
  FD_SET(sockfd, &wfds);

  //printf("Start conn\n");
  connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr));
  //printf("conning...\n");
  int maxfd = sockfd+1;
  int select_result;
  int conn_enable = 0;
  struct timeval t_timeout;
  t_timeout.tv_sec = 1;
  t_timeout.tv_usec = 0;
  struct timeval t_start, t_end;
  gettimeofday(&t_start, NULL);

  while(conn_enable==0)
  {
	select_result = select(maxfd, &rfds,  &wfds, NULL, &t_timeout);
	  switch(select_result)
	  {
		case -1:
			printf("select error!\n");
			return -2;
		case 0:
			break;
		default:
			//printf("%d",select_result);
			if(FD_ISSET(sockfd,&wfds))
			{
				conn_enable = 1;
				//printf("Conn OK!\n");
			}
			break;
	  }
	  gettimeofday(&t_end, NULL);
	  if ((t_end.tv_sec-t_start.tv_sec)>=timeout&&timeout!=0)
		{
			return -1;
		}
	  //printf("+");
  }

  gettimeofday(&t_end, NULL); 
  //printf("Conn spend: %ld us\n", t_end.tv_usec-t_start.tv_usec) ;

  //发送、接收改为同步，接收通过select实现异步
  flags = fcntl(sockfd, F_GETFL, 0);
  if (fcntl(sockfd, F_SETFL, 0) < 0)
  {
	fprintf(stderr,"Handle error by fcntl\n");
	return -2;
  }


  sprintf(request, "GET /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\nUser-Agent: CG Spider\r\nHost: %s:%d\r\nConnection: Close\r\n\r\n", host_file, host_addr, portnumber);
  //printf("%s", request);/*准备request，将要发送给主机*/

  /*发送http请求request*/
  send_length = 0;
  total_send = 0;
  /*
  nbytes=strlen(request);
  while(total_send < nbytes) {
    send = write(sockfd, request + total_send, nbytes - total_send);
    if(send==-1)  {printf("send error!%s\n", strerror(errno));exit(0);}
    total_send+=send;
    printf("%d bytes send OK!\n", total_send);
  }
*/
  int nBytes = send(sockfd,request,strlen(request),0);
  if (nBytes!=strlen(request))
  {
	printf("Send Error\n");
	close(sockfd);
	return -2;
  }

  //printf("\nThe following is the response header:\n");
  i=0;
  /* 连接成功了，接收http响应，response */

  total_recv = 0;
  int recv_enable = 1;
  int recv_finish = 0;
  
  gettimeofday(&t_start, NULL);
  //int is_display = 0;
  while(recv_finish==0)
  {
	    FD_SET(sockfd,&rfds);
		select_result = select(maxfd,&rfds,NULL,NULL,&t_timeout);
		switch(select_result)
		{
			case -1:
				printf("error");
				return -2;
				break;
			case 0:
				break;
			default:
				if (FD_ISSET(sockfd,&rfds))
				{
					bzero(buffer,1024);
					nbytes =  recv(sockfd, buffer, 1024, 0);
					if (nbytes>0)
						{
							append_string(http_body,buffer,nbytes);
							//printf("RECV<<<%s\nRECV SIZE:%d\nBUFF SIZE:%d\nRECV END<<<\n\n",buffer,nbytes,strlen(buffer));
							total_recv+=nbytes;
						}
					if (nbytes==0){
						//buffer[0] = '\0';
						//append_string(http_body,buffer,1);
						//printf("Conn close!!!\n");
						recv_finish = 1;
						}
					if (nbytes<0)
						fprintf(stderr,"ERR:%d RET:%d",errno,nbytes);
				}
				break;
		}
		/*
		if (total_recv>1024&&is_display==0)
		{
			is_display=1;
			fprintf(stderr,"RECV<<<\n%s\n\n",http_body->ds_string);
		}*/
	gettimeofday(&t_end, NULL);
	if((t_end.tv_sec-t_start.tv_sec)>=timeout&&timeout!=0)
	{
		close(sockfd);
		return -1;
	}
  }
	//printf("Total recv:%ld\n\n",total_recv);
	//printf("RECV:%s\n\nSIZE:%d\n",ptr->ds_string,ptr->ds_size);
  //printf("RECV Length:%d\n",total_recv);
  //fclose(fp);
  /* 结束通讯 */
  close(sockfd);
  //exit(0);
}



static napi_value HttpGET(napi_env env, napi_callback_info info) {
  size_t argc = 2;
  napi_value args[2];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

  napi_valuetype valuetype0;
  NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));

  napi_valuetype valuetype1;
  NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));

  NAPI_ASSERT(env, valuetype0 == napi_string ,"Wrong argument type. String expected.");

  char value0[1024];
  int n_result;
  NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], value0, 1024,&n_result));

  int value1;
  NAPI_CALL(env, napi_get_value_int32(env, args[1], &value1));

  napi_value result;

  DSTRING* ptr = (DSTRING*)malloc(sizeof(DSTRING));
  ptr->ds_string = (char*)malloc(2048);
  bzero(ptr->ds_string,2048);
  ptr->ds_size = 2048;
  ptr->ds_used = 0;
  bzero(ptr->ds_string,ptr->ds_size);
  int ret = http(ptr,value0,value1);
  switch(ret)
  {
	case 0:
		chunk_decode(ptr);
		break;
	case -1:
		NAPI_CALL(env, napi_throw_error(env,"-1","time out"));
		break;
	case -2:
		NAPI_CALL(env, napi_throw_error(env,"-2","socket/select error"));
		break;
	default:
		break;
  }
  //chunk_decode(ptr);

  char* native_result = ptr->ds_string;
  int native_result_len = ptr->ds_used;
  //char url[64];
  //const char* u = "http://www.66rpg.com/index.html";
  //memcpy(url,u,strlen(u));
  //http(url);


  NAPI_CALL(env, napi_create_string_utf8(env, native_result, native_result_len, &result));
  free(ptr->ds_string);
  free(ptr);
  return result;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_PROPERTY("http_get", HttpGET);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);


/*
int main(int argc,char** argv)
{
char url[1024];
const char* u = "http://pic.cgyouxi.com/orange/title/8860c99fd2145630f85e30be5fb33d85_60.jpg";
memcpy(url,u,strlen(u)+1);
  DSTRING* ptr = (DSTRING*)malloc(sizeof(DSTRING));
  ptr->ds_string = (char*)malloc(2048);
  ptr->ds_size = 2048;
  ptr->ds_used = 0;
  bzero(ptr->ds_string,ptr->ds_size);
  int ret = http(ptr,url,0);
  int before = ptr->ds_used;
  chunk_decode(ptr);
  printf("HTTP BODY>>>\n%s\n\n",ptr->ds_string);
  printf("before chunked length:%d\n\n",before);
 
  printf("after chunked length:%d\n\n",ptr->ds_used);
 
}

*/

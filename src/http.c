#include "http.h"

//#include <node_api.h>
//#include "../common.h"
void append_string(HTTPDATA* dest,char* source,int size)
{
	int len = dest->hd_used;
	//int len = strlen(dest->hd_response);
	if (len+size>dest->hd_size)
	{
		//不够用了重分配
		int buff_size = dest->hd_size+2048+size;
		char* sz_buff = (char*)malloc(buff_size);
		bzero(sz_buff,buff_size);
		memcpy(sz_buff,dest->hd_response,len);
		memcpy(sz_buff+len,source,size);
		free(dest->hd_response);
		dest->hd_response = sz_buff;
		dest->hd_size = buff_size;
		
	}
	else
	{
		//够用就直接拷贝
		memcpy(dest->hd_response+len,source,size);
	}
	dest->hd_used += size;

	//printf("%s\n",dest->hd_response);
	
	return;
}

int get_header_value(HTTPDATA* data, const char* field,char* value)
{
	char sz_field[32];
	memset(sz_field,0,sizeof(char)*32);
	memcpy(sz_field,field,strlen(field));
	strcat(sz_field,": ");
	return str_pick(data->hd_response_header,value,sz_field,"\r\n");
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
void chunk_decode(HTTPDATA* source)
{
	char* ptr_http_header = strstr(source->hd_response,"\r\n\r\n");
	if (ptr_http_header==NULL)
	{
		fprintf(stderr,"Error http header\nData:%s\n",source->hd_response);
		return;
	}
	char sz_http_status[4];
	memset(sz_http_status,0,sizeof(char)*4);
	memcpy(sz_http_status,source->hd_response+9,3);
	source->hd_response_code = atoi(sz_http_status);
	//printf("HTTP CODE:%d\n",source->hd_response_code);
	int body_start_pos = ptr_http_header - source->hd_response+4;
	//初始化header
	bzero(source->hd_response_header,HEADERSIZE);
	
	memcpy(source->hd_response_header,source->hd_response,body_start_pos);
	//printf("HTTP HEADER:\n%s\n",source->hd_response_header);
	//char *ptr_field_header = strstr(source->hd_response,"Transfer-Encoding: chunked");
	int body_length = 0;

	char sz_chunk_flag[10];
	char sz_content_length[32];
	char sz_location[1024];
	memset(sz_location,0,sizeof(char)*1024);
	memset(sz_content_length,0,sizeof(char)*32);
	memset(sz_chunk_flag,0,sizeof(char)*10);

	get_header_value(source,"Transfer-Encoding",sz_chunk_flag);
	get_header_value(source,"Content-Length",sz_content_length);
	get_header_value(source,"Location",sz_location);

	//printf("Location %s\n",sz_location);
	
	//处理重定向
	if((source->hd_response_code==301||source->hd_response_code==302)&&*sz_location&&source->hd_redirect_count<MAXREDIRECT)
	{	
		//跳转，计数+1
		source->hd_redirect_count++;
		source->hd_used = 0;
		memset(source->hd_response,0,source->hd_size);
		if(!http(source,sz_location,source->hd_timeout))
			chunk_decode(source);
		return;
	}
	if(source->hd_redirect_count>=MAXREDIRECT)
	{
		fprintf(stderr,"too many redirect\n");
		return;
	}

	//处理正常http报文
	if (!*sz_chunk_flag)
	{
		//非chunked协议，做定长检查
		if(sz_content_length)
			body_length = atoi(sz_content_length);
		else
			body_length = source->hd_used - body_start_pos;
		memmove(source->hd_response,source->hd_response+body_start_pos,body_length);
		source->hd_used = body_length;
		bzero(source->hd_response+source->hd_used,source->hd_size-source->hd_used);
		
		return;
	}

	char *sz_chunk_header = (char*)malloc(4096);
	//int chunk_pos = ptr_field_header - source->hd_response;
	//if (body_start_pos-chunk_pos<=0)
	//	return;
	//游标指向body开始
	char *p_cursor = source->hd_response+body_start_pos;

//	fprintf(stderr,"http header:\n%s\n\n",debug);
	int chunk_header_pos,chunk_size;
	HTTPDATA* p_chunked_content = (HTTPDATA*)malloc(sizeof(HTTPDATA));
	p_chunked_content->hd_response = (char*)malloc(source->hd_size);
	bzero(p_chunked_content->hd_response,source->hd_size);
	p_chunked_content->hd_used = 0;
	p_chunked_content->hd_size = source->hd_size;
	//p_chunked_content->hd_redirect_count = 0;

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
		append_string(p_chunked_content,p_cursor,chunk_size);
		p_cursor += chunk_size;
		chunk_count++;
		//查找下个chunk头
		p_cursor+=2;
	}
	memcpy(source->hd_response,p_chunked_content->hd_response,source->hd_size);
	source->hd_used = p_chunked_content->hd_used;
	free(sz_chunk_header);
	free(p_chunked_content->hd_response);
	free(p_chunked_content);
	//printf("\n\n===============\nChunk count:%d================\n",chunk_count);
		
	//charchunk = strstr(ptr->hd_response,"chunked");
	
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

int http(HTTPDATA* http_body,char* url,int timeout)
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

  http_body->hd_timeout = timeout;
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

  const char* sz_method = (http_body->hd_method==GET)?"GET":"POST";
  char sz_header[128];
  memset(sz_header,0,sizeof(char)*128);
  if (http_body->hd_method==POST)
	sprintf(sz_header,"Content-Length: %d\r\nContent-Type: application/x-www-form-urlencoded\r\n",strlen(http_body->hd_request_body));
  sprintf(request, "%s /%s HTTP/1.1\r\nAccept: */*\r\nAccept-Language: zh-cn\r\nUser-Agent: xd-synchttp\r\nHost: %s:%d\r\n%sConnection: Close\r\n\r\n%s", sz_method,host_file, host_addr, portnumber,sz_header, http_body->hd_request_body);
  //printf("REQ:%s\n\nREQ Length:%d\n", request,strlen(request));/*准备request，将要发送给主机*/

  /*发送http请求request*/
  send_length = strlen(request);
  total_send = 0;

  int nBytes = 0;
  char* ptr_send_cursor = request;
  while(total_send<send_length)
  {
	FD_SET(sockfd,&rfds);
	select_result = select(maxfd,NULL,&wfds,NULL,&t_timeout);
	switch(select_result)
	{
		case -1:
			fprintf(stderr,"select error");
			return -2;
			break;
		case 0:
			break;
		default:
			nBytes = send(sockfd,ptr_send_cursor,strlen(ptr_send_cursor)>SENDBUFFSZIE?SENDBUFFSZIE:strlen(ptr_send_cursor) ,0);
			ptr_send_cursor += nBytes;
			total_send += nBytes;
			//printf("Send length:%d>>>\n",nBytes);

	}
  }

  //printf("\nThe following is the response header:\n");
  i=0;
  /* 发送成功了，接收http响应，response */

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
			fprintf(stderr,"RECV<<<\n%s\n\n",http_body->hd_response);
		}*/
	gettimeofday(&t_end, NULL);
	if((t_end.tv_sec-t_start.tv_sec)>=timeout&&timeout!=0)
	{
		close(sockfd);
		return -1;
	}
  }
	//printf("Total recv:%ld\n\n",total_recv);
	//printf("RECV:%s\n\nSIZE:%d\n",ptr->hd_response,ptr->hd_size);
  //printf("RECV Length:%d\n",total_recv);
  //fclose(fp);
  /* 结束通讯 */
  close(sockfd);
  //exit(0);
}
/*
int main(int argc,char** argv)
{
char url[1024];
const char* u = "http://c2.cgyouxi.com/website/orange/img/common/entry/logo.png";
//const char* u = "http://www.66rpg.com";
memcpy(url,u,strlen(u)+1);
  HTTPDATA* ptr = (HTTPDATA*)malloc(sizeof(HTTPDATA));
  ptr->hd_response = (char*)malloc(2048);
  ptr->hd_size = 2048;
  ptr->hd_used = 0;
  ptr->hd_method = GET;
//  ptr->hd_request_body = (char*)malloc(2048);
  ptr->hd_response_header = (char*)malloc(HEADERSIZE);
  //memset(ptr->hd_request_body,0,sizeof(char)*2048);
  bzero(ptr->hd_response,ptr->hd_size);

  //memcpy(ptr->hd_request_body,&"a=111&b=sadasd",strlen("a=111&b=sadasd"));

  int ret = http(ptr,url,0);
  int before = ptr->hd_used;
  //printf("RECV<<<\n%s\n",ptr->hd_response);
  chunk_decode(ptr);
  printf("HTTP BODY>>>\n%s\n===============\n",ptr->hd_response);
  printf("before chunked length:%d\n\n",before);
 
  printf("after chunked length:%d\n\n",ptr->hd_used);
 
}

*/

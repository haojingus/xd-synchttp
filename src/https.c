#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>
#include <openssl/crypto.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>
#include <sys/select.h>

#define MAXBUF 1024

void show_certs(SSL *ssl){

	X509 *cert;
	char *line;
	cert = SSL_get_peer_certificate(ssl);
	if(cert!=NULL){
		printf("cert info===>\n");
		line = X509_NAME_oneline(X509_get_subject_name(cert), 0, 0);
		printf("Certification:%s\n",line);
		free(line);
		line = X509_NAME_oneline(X509_get_issuer_name(cert), 0, 0);
		printf("Issuer:%s\n",line);
		free(line);
		X509_free(cert);
	
	}else
	{
		printf("no cert detail!\n");
	}

}

int main(int argc,char** argv){

int re = 0;
SSL *ssl;
SSL_CTX *ctx;

//init ssl library
SSL_library_init();

//load all algorithms
OpenSSL_add_all_algorithms();

//load all errors
SSL_load_error_strings();

ctx = SSL_CTX_new(SSLv23_client_method());
printf("ctx address:%ld\n",ctx);
if(ctx == NULL){
	ERR_print_errors_fp(stdout);
	exit(1);
}

int sockfd, len;
//build socket
if((sockfd=socket(AF_INET, SOCK_STREAM, 0))<0){

perror("Socket create err");
exit(1);
}

printf("socket created!\n");

//make socket addr struct
struct sockaddr_in dest;
bzero(&dest,sizeof(dest));
dest.sin_family = AF_INET;
dest.sin_port = htons(443);
if(inet_aton("175.24.44.40",&dest.sin_addr)==0){
printf("ip convert error!\n");
exit(1);
}
printf("ip address is %ld\n",dest.sin_addr.s_addr);

//start no-ssl connect
if(connect(sockfd,(struct sockaddr *)&dest,sizeof(dest))!=0)
{
 perror("Connect error!\n");
 exit(1);
}
printf("connected!\n");

//make ssl context by sockfd
ssl = SSL_new(ctx);
SSL_set_fd(ssl,sockfd);
printf("SSL context build\n");

//connect ssl
if(SSL_connect(ssl) == -1){
 //ERR_print_errors_fp(stderr);
 printf("SSL connect error\n");
}
else{
printf("SSL Connect with %s encryption\n",SSL_get_cipher(ssl));
show_certs(ssl);

}

//send and receive
char buffer[MAXBUF+1];

bzero(buffer,MAXBUF+1);
strcpy(buffer,"GET / HTTP/1.0\r\nAccept: */*\r\n\r\n");
len = SSL_write(ssl,buffer,strlen(buffer));
if(len<0)
{	
	printf("send error!\n");
	exit(1);
}
printf("send ok!\n");


bzero(buffer,MAXBUF+1);

char* headers = (char*)malloc(MAXBUF*1000+1);
char* start = headers;
bzero(headers,MAXBUF*1000+1);
int i = 0;

//加入select支持
int select_ret;
fd_set rd_fds;
FD_ZERO(&rd_fds);
FD_SET(sockfd,&rd_fds);
int buff_size = 0;
int read_size = 0;
while(1)
{
	select_ret = select(FD_SETSIZE,&rd_fds,NULL,NULL,0);
	if(select_ret>=1){
		if(FD_ISSET(sockfd,&rd_fds))
		{
			buff_size = SSL_pending(ssl);
			if(buff_size==0)
				break;
			printf("pending size:%d\n",buff_size);

			while((len=SSL_read(ssl,buffer,1)) == 1){
				read_size++;
				if(read_size>=buff_size)
				{
					break;
				}
				*headers = buffer[0];
				headers++;
				if(i<4){
					if(buffer[0]=='\r'||buffer[0]=='\n'){
						i++;
						if(i>=4)
						{
							//printf("size is:%d\n",headers-start);
							//break;
						}
					}
				}
				else
				{
					i = 0;
				}
			}
		}
    }
		
}
printf("header:===>%s\n",start);
free(start);

SSL_shutdown(ssl);
SSL_free(ssl);
close(sockfd);
SSL_CTX_free(ctx);
return 0;
}
 

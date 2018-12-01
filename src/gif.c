#include "gif.h"

#define GIF_BYTES_TO_CHECK 10
/*
void gif_load_webimg(HTTPDATA* ptr,const char* url,int* size)
{
  char ptr_url[1024];
  memset(ptr_url,0,sizeof(char)*1024);
  memcpy(ptr_url,url,strlen(url));
  int ret = http(ptr,ptr_url,0);
  *size = 0;
  if (!ret)
  {
	  chunk_decode(ptr);
	  *size = ptr->hd_response->t_used;

  }
}
*/		 

//int load_png_image( const char *filepath)
int load_gif_image(const char* uri,HTTPDATA* ptr_http_data,IMAGE_SIZE* size)
{
		volatile unsigned short is_file = 1;

		if ((!(strstr(uri,"http://")-uri))||(!(strstr(uri,"https://")-uri)))
			is_file = 0;
        
		//用于文件
		FILE *fp;
		//用于http请求
		char* ptr_image_data;
		
        int w, h, temp;
		//int x,y,color_typer;
 
		//加载http数据
		int img_size;

		char buf[GIF_BYTES_TO_CHECK];

		memset(buf,0,GIF_BYTES_TO_CHECK);

		//char* IMAGEDATA;
		if(!is_file)
		{
			//const char* URL = "http://c2.cgyouxi.com/website/orange/img/common/entry/logo.png";
			//HTTPDATA* ptr_http_data = (HTTPDATA*)malloc(sizeof(HTTPDATA));
			//INIT_HTTPDATA (ptr_http_data);
			//gif_load_webimg(ptr_http_data,uri,img_size);
			img_size = ptr_http_data->hd_response->t_used;
			ptr_image_data = (char*)malloc(sizeof(char)*img_size);
			memcpy(ptr_image_data,ptr_http_data->hd_response->t_string,img_size);
			//FREE_HTTPDATA(ptr_http_data);

			//LOAD_IMG(ptr_image_data,"http://c2.cgyouxi.com/website/orange/img/common/entry/logo.png",img_size);
		}
		else
		{
		//printf("Size:%d",*img_size);
			fp = fopen( uri, "rb" );
			if( fp == NULL ) { 
				fprintf(stderr,"file open error!\n");
			    return -1;
			}
        }
		memset(buf,0,GIF_BYTES_TO_CHECK);

		//获取图像头和高宽
		if (is_file)
		{
			temp = fread( buf, 1, GIF_BYTES_TO_CHECK, fp );
		}
		else
		{
			temp = (img_size>GIF_BYTES_TO_CHECK)?GIF_BYTES_TO_CHECK:img_size;
			memcpy(buf,ptr_image_data,temp);

		}

		if (temp<GIF_BYTES_TO_CHECK||(!strstr(buf,"GIF")))
		{
			fprintf(stderr,"gif format error\n");
			return -2;
		}
		
		char info_buf[4] = {'\0','\0','\0','\0'};
		char *sz_stop;
		memcpy(info_buf,buf+6,4);
		w = (info_buf[0]&0xff)|((info_buf[1]<<8)&0xff00);
		h = (info_buf[2]&0xff)|((info_buf[3]<<8)&0xff00);
		if (!is_file)
			free(ptr_image_data);
		else
			fclose(fp);
		size->width = w;
		size->height = h;
		//printf("Width %d Height %d\n",w,h);
        return 0;
}
/*
int main(int argc,char** argv)
{
	//load_gif_image("./4.gif");
	IMAGE_SIZE t_size = {0,0};
	if(load_gif_image("http://storage.slide.news.sina.com.cn/slidenews/77_ori/2018_20/74766_823041_827212.gif",&t_size)==0)
		printf("W:%d H:%d\n",t_size.width,t_size.height);

//	printf("This is 2\n");
//	load_png_image("http://d.lanrentuku.com/down/png/1712/23haidiyulei-png/haidiyuleipng-013.png");

	return 0;
}
 
*/

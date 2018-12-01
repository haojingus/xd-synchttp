#include "imagesize.h"

int right(const char* src,char* dest,char c)
{
	if (strlen(src)==0)
		return -1;
	const char *p = src+strlen(src)-1;
	while(p>src)
	{
		if(*p==c)		
		{
			memcpy(dest,p,strlen(p));
			while(*dest!=0)
			{
			*dest = tolower(*dest);
			dest++;
			}

			return p-src+1;
		}
		p--;
	}
	return -1;
}

void load_webimg(const char* url,HTTPDATA* ptr)
{
  char ptr_url[1024];
  memset(ptr_url,0,sizeof(char)*1024);
  memcpy(ptr_url,url,strlen(url));
  int ret = http(ptr,ptr_url,0);
  //*size = 0;
  if (!ret)
  {
	  chunk_decode(ptr);
	  //*size = ptr->hd_response->t_used;

  }
}

IMAGE_FORMAT get_file_format(const char* path)
{
	char extname[16];
	memset(extname,0,sizeof(char)*16);
	if(right(path,extname,'.')<0)
		return FORMAT_UNKNOWN;
	if (strcmp(extname,".png")==0)
		return FORMAT_PNG;
	if (strcmp(extname,".jpeg")==0||strcmp(extname,".jpg")==0)
		return FORMAT_JPEG;
	if (strcmp(extname,".gif")==0)
		return FORMAT_GIF;
	return FORMAT_UNKNOWN;
	
}

int get_image_size(const char* uri,IMAGE_SIZE* size)
{
	int ret;
	IMAGE_FORMAT type = get_file_format(uri);
	int is_file = ((!(strstr(uri,"http://")-uri))||(!(strstr(uri,"https://")-uri)))?0:1;

	HTTPDATA* ptr_http_data = (HTTPDATA*)malloc(sizeof(HTTPDATA));

	if (!is_file)
	{	
		INIT_HTTPDATA(ptr_http_data);
		load_webimg(uri,ptr_http_data);
		if (type==FORMAT_UNKNOWN)
			type = get_image_type(ptr_http_data->hd_content_type);
	}
	
	switch(type)
	{
		case FORMAT_PNG:
			ret = load_png_image(uri,ptr_http_data,size);
			break;
		case FORMAT_JPEG:
			ret = load_jpeg_image(uri,ptr_http_data,size);
			break;
		case FORMAT_GIF:
			ret = load_gif_image(uri,ptr_http_data,size);
			break;
		default:
			ret = -1;
			break;
	}
	if (!is_file)
		{FREE_HTTPDATA(ptr_http_data);}

	return ret;
}
/*
int main(int argc,char** argv)
{
	IMAGE_SIZE t_size = {0,0};
	
	 if(get_image_size("http://pic.cgyouxi.com/orange/upload/201806/517168_26530f062578ce042e2cbf402e7f5860.jpg",&t_size)==0)
		printf("JPEG W:%d H:%d\n",t_size.width,t_size.height);
	 if(get_image_size("http://d.lanrentuku.com/down/png/1712/23haidiyulei-png/haidiyuleipng-013.png",&t_size)==0)
		printf("PNG W:%d H:%d\n",t_size.width,t_size.height);
		
	 //if(get_image_size("http://storage.slide.news.sina.com.cn/slidenews/77_ori/2018_20/74766_823041_827212.gif",&t_size)==0)
	 if(get_image_size("http://wmod.66rpg.com/res/loading.gif",&t_size)==0)
		printf("GIF W:%d H:%d\n",t_size.width,t_size.height);
	
	printf("Start file\n");
	
	if(get_image_size("./1.jpg",&t_size)==0)
		 printf("JPEG W:%d H:%d\n",t_size.width,t_size.height);

	if(get_image_size("./logo.png",&t_size)==0)
		 printf("PNG W:%d H:%d\n",t_size.width,t_size.height);
	if(get_image_size("./3.gif",&t_size)==0)
		 printf("GIF W:%d H:%d\n",t_size.width,t_size.height);
	
	HTTPDATA* ptr = (HTTPDATA*)malloc(sizeof(HTTPDATA));
	INIT_HTTPDATA(ptr);
	http(ptr,"http://www.66rpg.com",0);
	chunk_decode(ptr);
	FREE_HTTPDATA(ptr);
	return 0;
}


*/

#include "png.h"
#define PNG_BYTES_TO_CHECK 4

typedef struct 
{
	char* m_data;
	int   m_size;
	int   m_offset;
} PNG_IMAGEDATA;

/*
void png_load_webimg(HTTPDATA* ptr,const char* url,int* size)
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

void customerReadDataCallBack(png_structp png_ptr, png_bytep data, png_size_t length)
{
 PNG_IMAGEDATA* pImageData = (PNG_IMAGEDATA*)png_get_io_ptr(png_ptr);
 if ((int)(pImageData->m_offset + length) <= pImageData->m_size)
 {
     memcpy(data, pImageData->m_data+ pImageData->m_offset, length);
	 pImageData->m_offset += length;
 }
 else
 {
	fprintf(stderr,"customerReadDataCallBack failed");
 }
}
			 

//int load_png_image( const char *filepath)
int load_png_image(const char* uri,HTTPDATA* ptr_http_data,IMAGE_SIZE* t_size)
{
		volatile unsigned short is_file = 1;

		//if (ptr_data!=NULL)
		//	is_file = 0;
		if ((!(strstr(uri,"http://")-uri))||(!(strstr(uri,"https://")-uri)))
			is_file = 0;
        
		//用于文件
		FILE *fp;
		//用于http请求
		char* ptr_image_data;
		
        png_structp png_ptr;
        png_infop info_ptr;
        //png_bytep* row_pointers;
        char buf[PNG_BYTES_TO_CHECK];
        int w, h, temp;
		//int x,y,color_typer;
 
		//加载http数据
		int img_size;
		//char* IMAGEDATA;
		if(!is_file)
		{
			//const char* URL = "http://c2.cgyouxi.com/website/orange/img/common/entry/logo.png";
			//HTTPDATA* ptr_http_data = (HTTPDATA*)malloc(sizeof(HTTPDATA));
			//INIT_HTTPDATA (ptr_http_data);
			//png_load_webimg(ptr_http_data,uri,img_size);
			img_size = ptr_http_data->hd_response->t_used;
			ptr_image_data = (char*)malloc(sizeof(char)*img_size);
			memcpy(ptr_image_data,ptr_http_data->hd_response->t_string,img_size);
			//FREE_HTTPDATA(ptr_http_data);
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
        png_ptr = png_create_read_struct( PNG_LIBPNG_VER_STRING, 0, 0, 0 );
        info_ptr = png_create_info_struct( png_ptr );
        setjmp( png_jmpbuf(png_ptr) ); 
        /* 读取PNG_BYTES_TO_CHECK个字节的数据 */
		if (is_file)
		{
			temp = fread( buf, 1, PNG_BYTES_TO_CHECK, fp );
		}
		else
		{
		//====================地雷，小心
			temp = PNG_BYTES_TO_CHECK;
			memcpy(buf,ptr_image_data,PNG_BYTES_TO_CHECK);
		}

        /* 若读到的数据并没有PNG_BYTES_TO_CHECK个字节 */
        if( temp < PNG_BYTES_TO_CHECK ) {
				if (is_file)
					fclose(fp);
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
				fprintf(stderr,"no png format!\n");
                return -2;
        }
        /* 检测数据是否为PNG的签名 */
        temp = png_sig_cmp( (png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK );
        /* 如果不是PNG的签名，则说明该文件不是PNG文件 */
        if( temp != 0 ) {
				if(is_file)
	                fclose(fp);
                png_destroy_read_struct( &png_ptr, &info_ptr, 0);
				fprintf(stderr,"png signature error!\n");
                return -3;
        }
        
        /* 复位文件指针 */
		if (is_file)
			rewind( fp );

        /* 开始读文件 */
		if (is_file)
		{
			png_init_io( png_ptr, fp ); 
		}
		else
		{
		//替换为内存IO
			PNG_IMAGEDATA imageData;
			imageData.m_data = ptr_image_data;
			imageData.m_size = img_size;
			imageData.m_offset = 0;
			png_set_read_fn(png_ptr,(void*)&imageData,customerReadDataCallBack);
		}
        /* 读取PNG图片信息和像素数据 */
        png_read_png( png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0 );
        /* 获取图像的色彩类型 */
        //color_type = png_get_color_type( png_ptr, info_ptr );
        /* 获取图像的宽高 */
        w = png_get_image_width( png_ptr, info_ptr );
        h = png_get_image_height( png_ptr, info_ptr );
        /* 获取图像的所有行像素数据，row_pointers里边就是rgba数据 */
        //row_pointers = png_get_rows( png_ptr, info_ptr );
        png_destroy_read_struct( &png_ptr, &info_ptr, 0);
		if (!is_file)
			free(ptr_image_data);
		else
			fclose(fp);
		t_size->width = w;
		t_size->height = h;
		//printf("Width %d Height %d\n",w,h);
        return 0;
}
/*
int main(int argc,char** argv)
{
	printf("This is 1\n");
	IMAGE_SIZE t_size = {0,0};
	if(load_png_image("http://d.lanrentuku.com/down/png/1712/23haidiyulei-png/haidiyuleipng-013.png",&t_size)==0)
		printf("This is W:%d H:%d\n",t_size.width,t_size.height);
//	load_png_image("http://d.lanrentuku.com/down/png/1712/23haidiyulei-png/haidiyuleipng-013.png");

	return 0;
}
 

*/

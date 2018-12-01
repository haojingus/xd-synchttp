#include "jpeg.h"
#include <setjmp.h>
#define PUT_2B(array,offset,value)  \
         (array[offset] = (char) ((value) & 0xFF), \
          array[offset+1] = (char) (((value) >> 8) & 0xFF))
 #define PUT_4B(array,offset,value)  \
         (array[offset] = (char) ((value) & 0xFF), \
          array[offset+1] = (char) (((value) >> 8) & 0xFF), \
          array[offset+2] = (char) (((value) >> 16) & 0xFF), \
          array[offset+3] = (char) (((value) >> 24) & 0xFF))

struct my_error_mgr { struct jpeg_error_mgr pub; jmp_buf setjmp_buffer; };
typedef struct my_error_mgr * my_error_ptr;
METHODDEF(void) my_error_exit (j_common_ptr cinfo) 
{ 
	my_error_ptr myerr = (my_error_ptr) cinfo->err; 
	(*cinfo->err->output_message) (cinfo);
	fprintf(stderr,"Find a error from jpeg decompress!\n");
	longjmp(myerr->setjmp_buffer, 1); 
}
/*
void jpeg_load_webimg(HTTPDATA* ptr,const char* url,int* size)
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
 //读JPEG文件相当于解压文件


 int load_jpeg_image(const char *uri,HTTPDATA* ptr_http_data,IMAGE_SIZE* size)
 {
         struct jpeg_decompress_struct cinfo;
         //struct jpeg_error_mgr jerr;
		 struct my_error_mgr jerr;
         FILE *input_file;
//         FILE *output_file;
         JSAMPARRAY buffer;
         int row_width;
		char* ptr_image_data;
  //       unsigned char *output_buffer;
         unsigned char *tmp = NULL;
		 int img_size;

		 volatile unsigned short is_file = 1;
		if ((!(strstr(uri,"http://")-uri))||(!(strstr(uri,"https://")-uri)))
		{
			is_file = 0;
			//HTTPDATA* ptr_http_data = (HTTPDATA*)malloc(sizeof(HTTPDATA));
			//INIT_HTTPDATA (ptr_http_data);
			//jpeg_load_webimg(ptr_http_data,uri,img_size);
			img_size = ptr_http_data->hd_response->t_used;
			ptr_image_data = (char*)malloc(sizeof(char)*img_size);
			memcpy(ptr_image_data,ptr_http_data->hd_response->t_string,img_size);
			//FREE_HTTPDATA(ptr_http_data);
			//printf("Size %d\n",*img_size);

		}
		else
		{
	         if ((input_file = fopen(uri, "rb")) == NULL) {
                 fprintf(stderr, "can't open %s\n", uri);
                 return -1;
		     }

		}

         cinfo.err = jpeg_std_error(&jerr.pub);
		 jerr.pub.error_exit = my_error_exit;

		

         // Initialization of JPEG compression objects
	
		size_t size_jds = (size_t)sizeof(struct jpeg_decompress_struct);
		jpeg_CreateDecompress(&cinfo, JPEG_LIB_VERSION, size_jds);
		if (setjmp(jerr.setjmp_buffer)) 
		 {
			//fprintf(stderr,"Goto setjmp\n");
			jpeg_destroy_decompress(&cinfo); 
			if(is_file)
				fclose(input_file);
			else
				free(ptr_image_data);
			return -3; 
		 }


		if (is_file)
			jpeg_stdio_src(&cinfo, input_file);
		else
			jpeg_mem_src(&cinfo,ptr_image_data,img_size);

         (void) jpeg_read_header(&cinfo, TRUE);

         
		 size->width = cinfo.image_width;
		 size->height = cinfo.image_height;
		 //printf("step 4 Width:%u Height:%u\n",cinfo.image_width,cinfo.image_height);
         //(void) jpeg_start_decompress(&cinfo);

		//printf("step 5\n");
         row_width = cinfo.output_width * cinfo.output_components;



		 // 3.跳过读取的头文件字节Make a one-row-high sample array that will go away when done with image 
         //buffer = (*cinfo.mem->alloc_sarray)
     //            ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_width, 1);

         //write_bmp_header(&cinfo, output_file);
         //(void) jpeg_finish_decompress(&cinfo);
		//printf("step 6\n");
         jpeg_destroy_decompress(&cinfo);

         // Close files, if we opened them 
		 if(is_file)
	         fclose(input_file);
		else
			free(ptr_image_data);
         //fclose(output_file);
        return 0;
 }

/*

 int main(int argc, char *argv[])
 {      
	IMAGE_SIZE t_size = {0,0};
	 if(load_jpeg_image("http://pic.cgyouxi.com/orange/upload/201806/517168_26530f062578ce042e2cbf402e7f5860.jpg",&t_size)==0)
		printf("W:%d H:%d\n",t_size.width,t_size.height);
		
   
     return 0;
 }

 
*/			

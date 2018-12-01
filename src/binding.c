#include "http.h"
#include <node_api.h>
#include "common.h"
#include "imagesize.h"

static napi_value image_size(napi_env env, napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

  napi_valuetype valuetype0;
  NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));

  NAPI_ASSERT(env, valuetype0 == napi_string ,"Wrong argument type. String expected.");

  char value0[1024];
  size_t n_result;
  NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], value0, 1024,&n_result));
  IMAGE_SIZE t_size = {0,0};
  get_image_size(value0,&t_size);
		//printf("JPEG W:%d H:%d\n",t_size.width,t_size.height);
	//sleep(5);
  //初始化返回值
  napi_value ret;
  NAPI_CALL(env, napi_create_object(env, &ret));

  napi_value width;
  NAPI_CALL(env, napi_create_int32(env, t_size.width, &width));
  NAPI_CALL(env, napi_set_named_property(env, ret, "width",width));

  napi_value height;
  NAPI_CALL(env, napi_create_int32(env, t_size.height, &height));
  NAPI_CALL(env, napi_set_named_property(env, ret, "height",height));
//load_png_image(value0);
	return ret;
}


static napi_value http_get(napi_env env, napi_callback_info info) {
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
  size_t n_result;
  NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], value0, 1024,&n_result));

  int value1;
  NAPI_CALL(env, napi_get_value_int32(env, args[1], &value1));

  napi_value result;
  HTTPDATA* ptr = (HTTPDATA*)malloc(sizeof(HTTPDATA));
 
  INIT_HTTPDATA(ptr);
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

  char* native_result = ptr->hd_response->t_string;
  int native_result_len = ptr->hd_response->t_used;
  NAPI_CALL(env, napi_create_string_utf8(env, native_result, native_result_len, &result));
  FREE_HTTPDATA(ptr);
  return result;
}


static napi_value http_post(napi_env env, napi_callback_info info) {
//fprintf(stderr,"Warning,this function will be replaced by http()!\n");
  size_t argc = 3;
  napi_value args[3];
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");

  napi_valuetype valuetype0;
  NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));

  napi_valuetype valuetype1;
  NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));

  napi_valuetype valuetype2;
  NAPI_CALL(env, napi_typeof(env, args[2], &valuetype2));

  NAPI_ASSERT(env, valuetype0 == napi_string ,"Wrong argument type. String expected.");

  NAPI_ASSERT(env, valuetype1 == napi_object ,"Wrong argument type. Object expected.");

  NAPI_ASSERT(env, valuetype2 == napi_number ,"Wrong argument type. Number expected.");

  char value0[1024];
  size_t n_result;
  NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], value0, 1024,&n_result));

  int value2;
  NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2));

  //构建POST数据包
  napi_value obj = args[1];
  napi_value propertynames;
  NAPI_CALL(env, napi_get_property_names(env, obj, &propertynames));

  uint32_t i, length;
  NAPI_CALL(env, napi_get_array_length(env, propertynames, &length));

  HTTPDATA* ptr = (HTTPDATA*)malloc(sizeof(HTTPDATA));
  INIT_HTTPDATA(ptr);
  ptr->hd_method = POST;
  char *sz_swap_buff = (char*)malloc(sizeof(char)*2000000);
  for (i = 0; i < length; i++) {
    napi_value property_str;
    NAPI_CALL(env, napi_get_element(env, propertynames, i, &property_str));

    napi_value property_value;
    NAPI_CALL(env, napi_get_property(env, obj, property_str, &property_value));

	//获取属性名
	char sz_attr_name[32];
	memset(sz_attr_name,0,32);
	NAPI_CALL(env, napi_get_value_string_utf8(env, property_str, sz_attr_name, 32,&n_result));

	//获取属性值
	memset(sz_swap_buff,0,2000000);
	napi_value new_property_value;
	NAPI_CALL(env, napi_coerce_to_string(env, property_value, &new_property_value));
	NAPI_CALL(env, napi_get_value_string_utf8(env, new_property_value, sz_swap_buff, 2000000, &n_result));
		
	strcat(sz_attr_name,"=");
	strcat(sz_swap_buff,"&");

	//写入request_body
	append_dstring(ptr->hd_request_body,sz_attr_name,strlen(sz_attr_name));
	append_dstring(ptr->hd_request_body,sz_swap_buff,strlen(sz_swap_buff));

  }
  free(sz_swap_buff);

  //定义返回结果
  napi_value result;

  int ret = http(ptr,value0,value2);
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

  char* native_result = ptr->hd_response->t_string;
  int native_result_len = ptr->hd_response->t_used;

  NAPI_CALL(env, napi_create_string_utf8(env, native_result, native_result_len, &result));

  FREE_HTTPDATA(ptr);
  return result;
}


static napi_value sync_http(napi_env env, napi_callback_info info) {
  size_t argc = 4;
  napi_value args[4];
  char sz_url[1024];
  char sz_method[10];
  int timeout;

  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, NULL, NULL));

  NAPI_ASSERT(env, argc >= 1, "Wrong number of arguments");
//	fprintf(stderr,"argc:%d\n",argc);
  napi_valuetype valuetype0;
  NAPI_CALL(env, napi_typeof(env, args[0], &valuetype0));
  NAPI_ASSERT(env, valuetype0 == napi_string ,"Wrong argument type. String expected.");
  size_t n_result;
  NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], sz_url, 1024,&n_result));
  
  //初始化HTTP结构
  HTTPDATA* ptr = (HTTPDATA*)malloc(sizeof(HTTPDATA));
  INIT_HTTPDATA(ptr);
  ptr->hd_method = GET;


  if(argc>=2)
  {
	napi_valuetype valuetype1;
	NAPI_CALL(env, napi_typeof(env, args[1], &valuetype1));
	NAPI_ASSERT(env, valuetype1 == napi_string ,"Wrong argument type. String expected.");
	NAPI_CALL(env, napi_get_value_string_utf8(env, args[1], sz_method, 10,&n_result));
	ToLowerCase(sz_method);
	if(strcmp(sz_method,"post")==0)
		ptr->hd_method = POST;
	else if(strcmp(sz_method,"patch")==0)
		ptr->hd_method = PATCH;
	else if(strcmp(sz_method,"put")==0)
		ptr->hd_method = PUT;
	else if(strcmp(sz_method,"delete")==0)
		ptr->hd_method = DEL;
	else
		ptr->hd_method = GET;	
  }


  if (argc>=3)
  {
	napi_valuetype valuetype2;
	NAPI_CALL(env, napi_typeof(env, args[2], &valuetype2));
    NAPI_ASSERT(env, valuetype2 == napi_object ,"Wrong argument type. Object expected.");
	//构建request body数据包
	napi_value obj = args[2];
	napi_value propertynames;
	NAPI_CALL(env, napi_get_property_names(env, obj, &propertynames));

	char *sz_swap_buff = (char*)malloc(sizeof(char)*2000000);
	uint32_t i, length;
	NAPI_CALL(env, napi_get_array_length(env, propertynames, &length));

	for (i = 0; i < length; i++) {
		napi_value property_str;
		NAPI_CALL(env, napi_get_element(env, propertynames, i, &property_str));

	    napi_value property_value;
		NAPI_CALL(env, napi_get_property(env, obj, property_str, &property_value));

		//获取属性名
		char sz_attr_name[32];
		memset(sz_attr_name,0,32);
		NAPI_CALL(env, napi_get_value_string_utf8(env, property_str, sz_attr_name, 32,&n_result));

		//获取属性值
		memset(sz_swap_buff,0,2000000);
		napi_value new_property_value;
		NAPI_CALL(env, napi_coerce_to_string(env, property_value, &new_property_value));
		NAPI_CALL(env, napi_get_value_string_utf8(env, new_property_value, sz_swap_buff, 2000000, &n_result));
		
		strcat(sz_attr_name,"=");
		strcat(sz_swap_buff,"&");

		//写入request_body
		append_dstring(ptr->hd_request_body,sz_attr_name,strlen(sz_attr_name));
		append_dstring(ptr->hd_request_body,sz_swap_buff,strlen(sz_swap_buff));
	}
	free(sz_swap_buff);


  }


  if(argc>=4)
  {
	napi_valuetype valuetype3;
	NAPI_CALL(env, napi_typeof(env, args[3], &valuetype3));
	NAPI_ASSERT(env, valuetype3 == napi_number ,"Wrong argument type. Number expected.");
	NAPI_CALL(env, napi_get_value_int32(env, args[3], &timeout));
  }

  //定义返回结果
  napi_value result;
  //fprintf(stderr,"url:%s\ntimeout:%d\nmethod:%s\n",sz_url,timeout,sz_method);
  int ret = http(ptr,sz_url,timeout);
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

  char* native_result = ptr->hd_response->t_string;
  int native_result_len = ptr->hd_response->t_used;

  NAPI_CALL(env, napi_create_string_utf8(env, native_result, native_result_len, &result));

  FREE_HTTPDATA(ptr);
  return result;
}

//register function

static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor descriptors[] = {
    DECLARE_NAPI_PROPERTY("http_get", http_get),
    DECLARE_NAPI_PROPERTY("http_post", http_post),
	DECLARE_NAPI_PROPERTY("get_image_size", image_size),
	DECLARE_NAPI_PROPERTY("sync_http", sync_http),
   };

  NAPI_CALL(env, napi_define_properties(
      env, exports, sizeof(descriptors) / sizeof(*descriptors), descriptors));

  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);

 
  

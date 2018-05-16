#include "http.c"

#include <node_api.h>
#include "common.h"

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
  int n_result;
  NAPI_CALL(env, napi_get_value_string_utf8(env, args[0], value0, 1024,&n_result));

  int value1;
  NAPI_CALL(env, napi_get_value_int32(env, args[1], &value1));

  napi_value result;

  HTTPDATA* ptr = (HTTPDATA*)malloc(sizeof(HTTPDATA));
  ptr->hd_response = (char*)malloc(2048);
  ptr->hd_size = 2048;
  ptr->hd_used = 0;
  ptr->hd_method = GET;
  ptr->hd_redirect_count = 0;
  ptr->hd_request_body = (char*)malloc(2048);
  ptr->hd_response_header = (char*)malloc(HEADERSIZE);
  memset(ptr->hd_request_body,0,sizeof(char)*2048);
  bzero(ptr->hd_response,ptr->hd_size);

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

  char* native_result = ptr->hd_response;
  int native_result_len = ptr->hd_used;
  //char url[64];
  //const char* u = "http://www.66rpg.com/index.html";
  //memcpy(url,u,strlen(u));
  //http(url);


  NAPI_CALL(env, napi_create_string_utf8(env, native_result, native_result_len, &result));
  free(ptr->hd_response_header);
  free(ptr->hd_response);
  free(ptr);
  return result;
}


static napi_value http_post(napi_env env, napi_callback_info info) {
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
  int n_result;
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
  ptr->hd_request_body = (char*)malloc(sizeof(char)*2000000);
  char* ptr_post_cursor = ptr->hd_request_body;
  char *sz_swap_buff = (char*)malloc(sizeof(char)*2000000);
  memset(ptr->hd_request_body,0,2000000);
  int buff_length = 0;
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
    NAPI_CALL(env, napi_get_value_string_utf8(env, property_value, sz_swap_buff, 2000000, &n_result));
	strcat(sz_attr_name,"=");
	strcat(sz_swap_buff,"&");

	//写入request_body
	memcpy(ptr_post_cursor,sz_attr_name,strlen(sz_attr_name));
	ptr_post_cursor += strlen(sz_attr_name);
	memcpy(ptr_post_cursor,sz_swap_buff,strlen(sz_swap_buff));
	ptr_post_cursor += strlen(sz_swap_buff);

  }
  free(sz_swap_buff);
  


  //定义返回结果
  napi_value result;

  ptr->hd_response = (char*)malloc(2048);
  ptr->hd_size = 2048;
  ptr->hd_used = 0;
  ptr->hd_redirect_count = 0;
  ptr->hd_method = POST;
  ptr->hd_response_header = (char*)malloc(HEADERSIZE);
  bzero(ptr->hd_response,ptr->hd_size);

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

  char* native_result = ptr->hd_response;
  int native_result_len = ptr->hd_used;
  //char url[64];
  //const char* u = "http://www.66rpg.com/index.html";
  //memcpy(url,u,strlen(u));
  //http(url);


  NAPI_CALL(env, napi_create_string_utf8(env, native_result, native_result_len, &result));
  free(ptr->hd_response_header);
  free(ptr->hd_response);
  free(ptr->hd_request_body);
  free(ptr);
  return result;
}


//register function

static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor descriptors[] = {
    DECLARE_NAPI_PROPERTY("http_get", http_get),
    DECLARE_NAPI_PROPERTY("http_post", http_post),
   };

  NAPI_CALL(env, napi_define_properties(
      env, exports, sizeof(descriptors) / sizeof(*descriptors), descriptors));

  return exports;
}

/*
static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor desc = DECLARE_NAPI_PROPERTY("http_get", HttpGET);
  NAPI_CALL(env, napi_define_properties(env, exports, 1, &desc));
  return exports;
}
*/
NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);

/*

int main(int argc,char** argv)
{
char url[1024];
const char* u = "http://api_test.17k.com/demo1.php";
//const char* u = "http://www.66rpg.com";
memcpy(url,u,strlen(u)+1);
  HTTPDATA* ptr = (HTTPDATA*)malloc(sizeof(HTTPDATA));
  ptr->hd_response = (char*)malloc(2048);
  ptr->hd_size = 2048;
  ptr->hd_used = 0;
  ptr->hd_method = POST;
  ptr->hd_request_body = (char*)malloc(2048);
  ptr->hd_response_header = (char*)malloc(HEADERSIZE);
  memset(ptr->hd_request_body,0,sizeof(char)*2048);
  bzero(ptr->hd_response,ptr->hd_size);

  memcpy(ptr->hd_request_body,&"a=111&b=sadasd",strlen("a=111&b=sadasd"));

  int ret = http(ptr,url,0);
  int before = ptr->hd_used;
  //printf("RECV<<<\n%s\n",ptr->hd_response);
  chunk_decode(ptr);
  printf("HTTP BODY>>>\n%s\n===============\n",ptr->hd_response);
  printf("before chunked length:%d\n\n",before);
 
  printf("after chunked length:%d\n\n",ptr->hd_used);
 
}

*/



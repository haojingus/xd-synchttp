#include "http.h"

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

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init);

 

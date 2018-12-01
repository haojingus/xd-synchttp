#ifndef _XD_PNG_H_
#define _XD_PNG_H_

#include <png.h>
#include "image.h"
#include "http.h"

int load_png_image(const char* uri,HTTPDATA* ptr_http_data,IMAGE_SIZE* t_size);

#endif

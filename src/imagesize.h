#ifndef _XD_IMAGESIZE_H
#define _XD_IMAGESIZE_H

#include "http.h"
#include "jpeg.h"
#include "gif.h"
#include "png.h"

void load_webimg(const char* uri,HTTPDATA* ptr);
int get_image_size(const char* uri,IMAGE_SIZE* size);
#endif

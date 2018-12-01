#ifndef _XD_GIF_H
#define _XD_GIF_H

#include "image.h"
#include "http.h"
int load_gif_image(const char* uri,HTTPDATA* ptr_http_data,IMAGE_SIZE* size);

#endif

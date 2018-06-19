#ifndef _XD_JPEG_H_
#define _XD_JPEG_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <jpeglib.h>
#include "http.h"
#include "image.h"

int load_jpeg_image(const char *uri,HTTPDATA* ptr_http_data,IMAGE_SIZE* size);
#endif

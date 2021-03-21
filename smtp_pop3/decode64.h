#ifndef BASE64_H
#define BASE64_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BASE64_PAD64 '='

/*----------------------------------------
 *描述：base64 编码
 *参数：data:需要编码的数据
 *返回值：返回编码好的数据
----------------------------------------*/
char* base64_encode(const char *data);

/*----------------------------------------
 *描述：base64 解码
 *参数：data:需要解码的数据
 *返回值：返回解码好的数据
----------------------------------------*/
char* base64_decode(const char *bdata);

#endif

#ifndef HTTPRESQUEST_H
#define HTTPRESQUEST_H

#include <Arduino.h>

String httpsAddressBaidu(String dataTranslate,String APP_ID,String salt,String secretKey);
void baiduHttpReq(String resource);

#endif
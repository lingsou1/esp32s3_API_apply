#ifndef BAIDUACCESSTOKENGET_H
#define BAIDUACCESSTOKENGET_H

#include <Arduino.h>

String httpsAddress_baiduToken(String client_id,String client_secret);
void httpsRequest_baiduToken(String requestAddress);


#endif
//用来对HTTPS进行请求,获取百度的 token

#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include "JSONParseData.h"

//这就是CA证书,是对应的https://aip.baidubce.com/oauth/2.0/token的,其余的网站需要重新导入
const char* rootCACertificateToken = \
"-----BEGIN CERTIFICATE-----\n\
MIIDXzCCAkegAwIBAgILBAAAAAABIVhTCKIwDQYJKoZIhvcNAQELBQAwTDEgMB4G\n\
A1UECxMXR2xvYmFsU2lnbiBSb290IENBIC0gUjMxEzARBgNVBAoTCkdsb2JhbFNp\n\
Z24xEzARBgNVBAMTCkdsb2JhbFNpZ24wHhcNMDkwMzE4MTAwMDAwWhcNMjkwMzE4\n\
MTAwMDAwWjBMMSAwHgYDVQQLExdHbG9iYWxTaWduIFJvb3QgQ0EgLSBSMzETMBEG\n\
A1UEChMKR2xvYmFsU2lnbjETMBEGA1UEAxMKR2xvYmFsU2lnbjCCASIwDQYJKoZI\n\
hvcNAQEBBQADggEPADCCAQoCggEBAMwldpB5BngiFvXAg7aEyiie/QV2EcWtiHL8\n\
RgJDx7KKnQRfJMsuS+FggkbhUqsMgUdwbN1k0ev1LKMPgj0MK66X17YUhhB5uzsT\n\
gHeMCOFJ0mpiLx9e+pZo34knlTifBtc+ycsmWQ1z3rDI6SYOgxXG71uL0gRgykmm\n\
KPZpO/bLyCiR5Z2KYVc3rHQU3HTgOu5yLy6c+9C7v/U9AOEGM+iCK65TpjoWc4zd\n\
QQ4gOsC0p6Hpsk+QLjJg6VfLuQSSaGjlOCZgdbKfd/+RFO+uIEn8rUAVSNECMWEZ\n\
XriX7613t2Saer9fwRPvm2L7DWzgVGkWqQPabumDk3F2xmmFghcCAwEAAaNCMEAw\n\
DgYDVR0PAQH/BAQDAgEGMA8GA1UdEwEB/wQFMAMBAf8wHQYDVR0OBBYEFI/wS3+o\n\
LkUkrk1Q+mOai97i3Ru8MA0GCSqGSIb3DQEBCwUAA4IBAQBLQNvAUKr+yAzv95ZU\n\
RUm7lgAJQayzE4aGKAczymvmdLm6AC2upArT9fHxD4q/c2dKg8dEe3jgr25sbwMp\n\
jjM5RcOO5LlXbKr8EpbsU8Yt5CRsuZRj+9xTaGdWPoO4zzUhw8lo/s7awlOqzJCK\n\
6fBdRoyV3XpYKBovHd7NADdBj+1EbddTKJd+82cEHhXXipa0095MJ6RMG3NzdvQX\n\
mcIfeg7jLQitChws/zyrVQ4PkX4268NXSb7hLi18YIvDQVETI53O9zJrlAGomecs\n\
Mx86OyXShkDOOyyGeMlhLxS67ttVb9+E7gUJTb0o2HLO02JQZR7rkpeDMdmztcpH\n\
WD9f\n\
-----END CERTIFICATE-----\n";




/**
* @brief 根据参数输出需要请求的网址的字符串
*
* @param String client_id:
* @param String client_secret:
* @return String:会返回一个请求地址的字符串
*/
String httpsAddress_baiduToken(String client_id,String client_secret){
  String requestAddress = "https://aip.baidubce.com/oauth/2.0/token?grant_type=client_credentials&client_id="
                          + client_id + "&client_secret=" 
                          + client_secret;

  //返回的请求地址
  return requestAddress;
}


/**
* @brief 进行HTTPS请求,同时串口输出相关的Token数据,不会将全部的返回数据串口输出
*
* @param String requestAddress:需要连接的网站的地址
* @return 无
*/
void httpsRequest_baiduToken(String requestAddress){
  //这是那个用来满足HTTPS请求的库
  WiFiClientSecure *client = new WiFiClientSecure;

  if(client) {
    //这是在使用证书
    client -> setCACert(rootCACertificateToken);
    {
      //为 HTTPClient https 添加一个范围块，以确保它在 WiFiClientSecure *client 被销毁之前被销毁(不懂) 
      HTTPClient https;

      Serial.print("[HTTPS] begin...\n");
      //开始请求网站(以HTTPS方法)
      if (https.begin(*client,requestAddress)) {  
        Serial.print("[HTTPS] GET...\n");
        // 开始连接并发送请求头
        int httpCode = https.GET();
  
        // 对HTTP响应码进行处理
        if (httpCode > 0) {
          //串口输出响应码
          Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
  
          // 如果响应码正常则获取返回的内容
          if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
            String payload = https.getString();   //将返回的内容以字符串的形式储存
            //Serial.println(payload);    //串口输出获取的内容
            Serial.print("\n \n \n");
            //对返回的内容进行JSON数据解析并返回一个字符串,不是HTTPS必须的
            String Token = access_token_JSONParse(payload);
            Serial.print(Token);
            Serial.print("\n");
          }
        } else {
          Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
        }
        //结束连接
        https.end();
      } else {
        Serial.printf("[HTTPS] Unable to connect\n");
      }
    }
    delete client;
  } else {
    Serial.println("Unable to create client");
  }
}
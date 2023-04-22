//用来对HTTPS进行请求
//用于获取"一言"数据("一言"就是一句有哲理的句子)
//"一言"详见内容 :https://developer.hitokoto.cn/sentence/

#include <WiFiClientSecure.h>
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

#include "JSONParseData.h"  //解析得到的JSON数据

//这就是CA证书,是对应的https://v1.hitokoto.cn/?c=f&encode=json的,其余的网站需要重新导入
const char* rootCACertificate = \
"-----BEGIN CERTIFICATE-----\n\
MIIDdTCCAl2gAwIBAgILBAAAAAABFUtaw5QwDQYJKoZIhvcNAQEFBQAwVzELMAkG\n\
A1UEBhMCQkUxGTAXBgNVBAoTEEdsb2JhbFNpZ24gbnYtc2ExEDAOBgNVBAsTB1Jv\n\
b3QgQ0ExGzAZBgNVBAMTEkdsb2JhbFNpZ24gUm9vdCBDQTAeFw05ODA5MDExMjAw\n\
MDBaFw0yODAxMjgxMjAwMDBaMFcxCzAJBgNVBAYTAkJFMRkwFwYDVQQKExBHbG9i\n\
YWxTaWduIG52LXNhMRAwDgYDVQQLEwdSb290IENBMRswGQYDVQQDExJHbG9iYWxT\n\
aWduIFJvb3QgQ0EwggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDaDuaZ\n\
jc6j40+Kfvvxi4Mla+pIH/EqsLmVEQS98GPR4mdmzxzdzxtIK+6NiY6arymAZavp\n\
xy0Sy6scTHAHoT0KMM0VjU/43dSMUBUc71DuxC73/OlS8pF94G3VNTCOXkNz8kHp\n\
1Wrjsok6Vjk4bwY8iGlbKk3Fp1S4bInMm/k8yuX9ifUSPJJ4ltbcdG6TRGHRjcdG\n\
snUOhugZitVtbNV4FpWi6cgKOOvyJBNPc1STE4U6G7weNLWLBYy5d4ux2x8gkasJ\n\
U26Qzns3dLlwR5EiUWMWea6xrkEmCMgZK9FGqkjWZCrXgzT/LCrBbBlDSgeF59N8\n\
9iFo7+ryUp9/k5DPAgMBAAGjQjBAMA4GA1UdDwEB/wQEAwIBBjAPBgNVHRMBAf8E\n\
BTADAQH/MB0GA1UdDgQWBBRge2YaRQ2XyolQL30EzTSo//z9SzANBgkqhkiG9w0B\n\
AQUFAAOCAQEA1nPnfE920I2/7LqivjTFKDK1fPxsnCwrvQmeU79rXqoRSLblCKOz\n\
yj1hTdNGCbM+w6DjY1Ub8rrvrTnhQ7k4o+YviiY776BQVvnGCv04zcQLcFGUl5gE\n\
38NflNUVyRRBnMRddWQVDf9VMOyGj/8N7yy5Y0b2qvzfvGn9LhJIZJrglfCm7ymP\n\
AbEVtQwdpf5pLGkkeB6zpxxxYu7KyJesF12KwvhHhm4qxFYxldBniYUr+WymXUad\n\
DKqC5JlR3XC321Y9YeRq4VzW9v493kHMB65jUr9TU/Qr6cf9tveCX4XSQRjbgbME\n\
HMUfpIBvFSDJ3gyICh3WZlXi/EjJKSZp4A==\n\
-----END CERTIFICATE-----\n";



/**
* @brief 根据参数输出需要请求的网址的字符串
*
* @param String dataFrom:请求资源的来自地点(可以用a~i)(详见https://developer.hitokoto.cn/sentence/)
* @param String type:需要返回的内容的格式(一般用json,也建议用JSON)(详见https://developer.hitokoto.cn/sentence/)
* @return String:会返回一个请求地址的字符串
*/
String httpsAddress(String dataFrom,String type){
  String requestAddress = "https://v1.hitokoto.cn/?c="
                          + dataFrom + "&encode=" 
                          + type;
  return requestAddress;
}



/**
* @brief 进行HTTPS请求,同时解析JSON数据,会返回"一言"数据(以字符串的形式)
*
* @param String requestAddress:需要请求的网站的地址
* @return String:返回解析出的"一言"数据
*/
String httpsRequest(String requestAddress){
  //这是那个用来满足HTTPS请求的库
  WiFiClientSecure *client = new WiFiClientSecure;

  if(client) {
    //这是在使用证书
    client -> setCACert(rootCACertificate);
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
            //对返回的内容进行JSON数据解并返回一个字符串
            return parseData(payload);
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
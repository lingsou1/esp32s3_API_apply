#include "md5_generate.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "JSONParseData.h"

/**
* @brief 解析https请求获取的JSON数据,解析出一言,是根据网站代码生成的,详见(https://fanyi-api.baidu.com/product/113)
*
* @param String dataTranslate:需要翻译的数据
* @param String APP_ID:百度要求的一个东西,在百度的服务台查看
* @param String salt:随机数,百度要求使用的,好像自己随便写
* @param String secretKey:密钥,百度要求使用的,在百度的服务台查看
* @return String:会返回一个HTTP请求的地址
*/
String httpsAddressBaidu(String dataTranslate,String APP_ID,String salt,String secretKey){
  String sign = MD5_generate(APP_ID+dataTranslate+salt+secretKey);

  String requestAddress = "http://api.fanyi.baidu.com/api/trans/vip/translate?q="
                          + dataTranslate +"&from=zh&to=en&appid=" 
                          + APP_ID + "&salt=" +salt + "&sign=" + sign;
  return requestAddress;
}

/**
* @brief 
*
* @param 
* @return 
*/
void baiduHttpReq(String resource)
{
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");
  // configure traged server and url
  //http.begin("https://www.howsmyssl.com/a/check", ca); //HTTPS
  http.begin(resource); //HTTP

  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) 
  {

    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // file found at server
    if(httpCode == HTTP_CODE_OK) 
    {
      String payload = http.getString();
      //Serial.print(payload);    //输出响应信息(JSON)
      parseData_baidu(payload);
    }
  } else 
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}


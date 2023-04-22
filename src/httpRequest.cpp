//使用百度翻译(HTTP请求)

#include "md5_generate.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "JSONParseData.h"



/**
* @brief 生成进行HTTP请求的链接,详见https://fanyi-api.baidu.com/product/113
*
* @param String dataTranslate:需要翻译的数据,就是"一言"获取的数据
* @param String APP_ID:百度要求的一个东西,在百度的服务台查看
* @param String salt:随机数,百度要求使用的,好像自己随便写
* @param String secretKey:密钥,百度要求使用的,在百度的服务台查看
* @return String:会返回一个HTTP请求的地址
*/
String httpsAddressBaidu(String dataTranslate,String APP_ID,String salt,String secretKey){
  String sign = MD5_generate(APP_ID+dataTranslate+salt+secretKey);    //进行MD5加密,百度的要求

  String requestAddress = "http://api.fanyi.baidu.com/api/trans/vip/translate?q="
                          + dataTranslate +"&from=zh&to=en&appid=" 
                          + APP_ID + "&salt=" +salt + "&sign=" + sign;
  return requestAddress;
}



/**
* @brief 请求API,翻译需要翻译的数据,同时对返回的JSON数据进行解析,串口输出翻译数据
*
* @param String resource:HTTP请求的地址,包含了需要翻译的数据
* @return 无
*/
void baiduHttpReq(String resource)
{
  HTTPClient http;

  Serial.print("[HTTP] begin...\n");

  //向指定链接进行HTTP请求
  http.begin(resource);

  Serial.print("[HTTP] GET...\n");

  // 开始连接并发送请求头
  int httpCode = http.GET();

  // httpCode will be negative on error
  if(httpCode > 0) 
  {

    // 请求头已发送同时响应头已进行处理
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);

    // 在响应体中找需要的数据
    if(httpCode == HTTP_CODE_OK) 
    {
      String payload = http.getString();
      //Serial.print(payload);    //输出响应信息(JSON)
      parseData_baidu(payload);   //JSON解析数据,串口输出翻译数据
    }
  } else 
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}


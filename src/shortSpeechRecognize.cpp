//这是将音频文件转化为汉字的一个操作
//文档在这:https://cloud.baidu.com/doc/SPEECH/s/qlcirqhz0
//需要特别注意内存溢出会导致硬件不断重启
//在Arduino环境下，根据板子的内存容量和其他资源的使用情况，可能会存在内存限制。
//这可能导致您的代码尝试分配超过可用内存的空间，从而导致程序崩溃。您的音频文件大小为126KB，如果一次性将整个文件读入内存可能会导致内存不足。
//尝试分块读取文件，每次读取一小块数据，将其附加到请求正文中，然后再发送请求。
#include "md5_generate.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "JSONParseData.h"
#include <SPIFFS.h>






/**
* @brief 生成进行HTTP请求的链接
*
* @param String dataTranslate:需要翻译的数据,就是"一言"获取的数据
* @param String APP_ID:百度要求的一个东西,在百度的服务台查看
* @param String salt:随机数,百度要求使用的,好像自己随便写
* @param String secretKey:密钥,百度要求使用的,在百度的服务台查看
* @return String:会返回一个HTTP请求的地址
*/
String httpAddressSpeech(String cuid,String token){

  String requestAddress = "http://vop.baidu.com/server_api?dev_pid=1537&cuid="
                          + cuid +"&token=" 
                          + token;
  return requestAddress;
}



/**
* @brief 请求API,对返回的JSON数据进行解析,串口输出翻译数据
*
* @param String resource:HTTP请求的地址,包含了需要翻译的数据
* @return 无
*/
String translateSpeechToText(String url) {
  // Set up HTTP request
  HTTPClient http;
  http.begin(url);
  http.addHeader("Content-Type", "audio/pcm;rate=16000");
  //http.addHeader("Content-Type", "audio/pcm;rate=16000;channels=1");

  // Open audio file
  File audiofile = SPIFFS.open("/16k.pcm", "r");
  if (!audiofile) {
    Serial.println("Failed to open audio file");
    return "";
  }

  // Read audio file content into a buffer
  const size_t bufferSize = 1024;
  uint8_t buffer[bufferSize];
  size_t bytesRead = 0;
  String requestBody = "";
  while (audiofile.available()) {
    bytesRead = audiofile.read(buffer, bufferSize);
    requestBody += String((char*)buffer, bytesRead);
  }


  // Send HTTP request
  int httpcode = http.POST(requestBody);
  if (httpcode == HTTP_CODE_OK) {
    // Get HTTP response
    String httpResponse = http.getString();
    Serial.println(httpResponse);
  }
  else{
    Serial.println("Failed to translate speech to text");
    http.end();
    audiofile.close();
    return "";
  }

  // Clean up
  http.end();
  audiofile.close();
}

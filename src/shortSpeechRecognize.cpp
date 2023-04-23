//这是将音频文件转化为汉字的一个操作
//文档在这:https://cloud.baidu.com/doc/SPEECH/s/qlcirqhz0
//需要特别注意内存溢出会导致硬件不断重启,需要将录音文件进行分块读取
//或者直接使用psram进行录音文件的读取
//在Arduino环境下，根据板子的内存容量和其他资源的使用情况，可能会存在内存限制。
//这可能导致您的代码尝试分配超过可用内存的空间，从而导致程序崩溃。您的音频文件大小为126KB，如果一次性将整个文件读入内存可能会导致内存不足。
//尝试分块读取文件，每次读取一小块数据，将其附加到请求正文中，然后再发送请求。
#include "md5_generate.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include "JSONParseData.h"
#include <SPIFFS.h>
#include "JSONParseData.h"

#include <ESP.h>
 


/**
* @brief 生成进行HTTP请求的链接,参数要求详见:https://cloud.baidu.com/doc/SPEECH/s/Vk38lxily
*
* @param String cuid:用户唯一标识，用来区分用户，计算 UV 值。建议填写能区分用户的机器 MAC 地址或 IMEI 码，长度为 60 字符以内。
* @param String token:百度需要的token,就是这个文件(baiduAccessTokenGet.cpp)获取的那个token,不过建议直接电脑获取,每个TOKEN时间30天
* @return String:会返回一个HTTP请求的地址
*/
String httpAddressSpeech(String cuid,String token){

  String requestAddress = "http://vop.baidu.com/server_api?dev_pid=1537&cuid="
                          + cuid +"&token=" 
                          + token;
  return requestAddress;
}



/**
* @brief 向指定网址(百度的短语音识别平台)发送HTTP请求,在请求体中包含待转文字的录音文件,会串口输出识别后的文字
*
* @param String url:HTTP请求的地址
* @return String :
*/
String translateSpeechToText(String url) {
  // 建立HTTP请求
  HTTPClient http;
  http.begin(url);
  //添加请求头相关的参数,百度的要求
  http.addHeader("Content-Type", "audio/pcm;rate=16000");
  //http.addHeader("Content-Type", "audio/pcm;rate=16000;channels=1");

  // 打开录音文件,这个文件需要提前上传至闪存
  File audiofile = SPIFFS.open("/16k.pcm", "r");
  if (!audiofile) {
    Serial.println("Failed to open audio file\n");
    return "";
  }

  // 将录音文件读取至缓存区,并将其二进制文件放在HTTP请求的请求体中
  /*
    这段程序的作用是从音频文件中读取数据，并将读取到的数据拼接为一个字符串，
    作为HTTP请求的请求体。具体来说，该程序使用while循环不断读取文件中的数据，
    每次读取的数据大小为bufferSize，将读取的数据存储在名为buffer的缓存区中，
    并将buffer中的数据转换为字符串形式，存储在名为requestBody的字符串中。
    最终，requestBody中存储的数据将作为HTTP请求的请求体，通过网络发送到服务器端。
  */
  const size_t bufferSize = 1024;
  uint8_t buffer[bufferSize];
  size_t bytesRead = 0;
  String requestBody = "";
  while (audiofile.available()) {
    bytesRead = audiofile.read(buffer, bufferSize);
    requestBody += String((char*)buffer, bytesRead);
  }



  // 将录音文件读取在PSRAM中去,这样可以不使用RAM来读取了(PSRAM有8MB),但是我不是很清楚怎么用PSRAM,最后一句总是会报错
  //当我注释掉PSRAM的内存释放后文件也能正常使用
  // const size_t bufferSize = 1024;
  // uint8_t *psramBuffer = (uint8_t *)ps_malloc(bufferSize); // 在 PSRAM 中分配缓冲区
  // size_t bytesRead = 0;
  // String requestBody = "";
  // while (audiofile.available()) {
  //   bytesRead = audiofile.read(psramBuffer, bufferSize);
  //   requestBody += String((char*)psramBuffer, bytesRead); // 从PARSM中读取文件
  // }
  // ps_free(psramBuffer); // 在PSRAM释放缓冲区



  // 发送HTTP请求
  int httpcode = http.POST(requestBody);
  if (httpcode == HTTP_CODE_OK) {
    // 获取HTTP响应
    String httpResponse = http.getString();
    String shortSpeech = JSONParse_shortSpeech(httpResponse);
    //Serial.println(shortSpeech);
    return shortSpeech;
  }
  else{
    Serial.println("Failed to translate speech to text\n");
    http.end();
    audiofile.close();
    return "";
  }

  // 结束HTTP以及关闭闪存文件
  http.end();
  audiofile.close();
}

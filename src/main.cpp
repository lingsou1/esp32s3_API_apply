/*
接线说明:无

程序说明:我对于这个程序的最大的想法是实现一个语音小助手,esp32S3外接一个INMP441麦克风进行录音,将录音文件暂存至flash
        ,再将录音文件上传百度转文字,再将返回的文字信息发送到chatGPT平台,获取相应的回答,再将回答播放出来.好像有点难,
        我不知道能做多少,将就着写吧,能不能完成不知道
        

注意事项:1:HTTPS请求需要请求网页的证书,证书有三个,只能用后两个证书
        2:HTTPS请求首先需要同步网络时间,这是由于协议的要求
        3:使用https请求向API发送GET请求,并将返回的内容打印,实现了向一言的请求,可以解析出相应的语句,间隔十秒
        4:对应的解析JSON的网站是:https://arduinojson.org/v6/assistant/
        5:同时可以将得到的中文通过百度翻译为英文,
        6:但是该程序有时候在翻译的时候会出错,好像是和内存释放有关系吗,还是两个请求间隔有要求,我不是太清楚


函数示例:无

作者:灵首

时间:2023_4_19

*/
//#define FLASH_SIZE 16384 // 16MB,设置开发板的flash,以最大限度的利用开发板
#define FLASH_SIZE 16777216 // 16MB,设置开发板的flash,以最大限度的利用开发板

//库文件
#include <Arduino.h>
#include <Ticker.h>
#include <SPIFFS.h>

//自己写的文件
#include "wifiSet.h"
#include "JSONParseData.h"
#include "md5_generate.h"
#include "clockSet.h"
#include "httpsResquest.h"
#include "httpRequest.h"
#include "baiduAccessTokenGet.h"
#include "shortSpeechRecognize.h"



Ticker ticker;
int counter;    //用来计数



//百度相关的参数,这是自己需要改的,不能和我的一样(我的也是改了的,是不能直接用的),详见 https://fanyi-api.baidu.com/product/113
String app_id = "202304175709";
String secret_key = "TckiM1JM0u0Np";
String baidu_salt = "12356";

//百度的相关设置,获取token的,需要自己修改(我的是改过的,是不能直接用的),其实用电脑获取方便一点(我最后也是用电脑获取的token)详见 https://ai.baidu.com/ai-doc/REFERENCE/Ck3dwjhhu
const String APIKey = "pIIzWqeatyk7A8i6snjFOKTd";
const String secretKey = "nMjByHUvrBG1zXN5NtkNVuyX33XPAZ6b";

//百度语音转文字需要的参数,(我的是改过的,是不能直接用的)详见 https://cloud.baidu.com/doc/SPEECH/s/Vk38lxily#raw-%E6%96%B9%E5%BC%8F%E4%B8%8A%E4%BC%A0%E9%9F%B3%E9%A2%91
const String cuid = "fe8042448e8f61fec5023";
const String token = "24.92165bcfb2fdfd4576f00ca411391843.2592000.1684682773.282335-32424903";



/**
* @brief 用来实现定时处理的函数,协同该函数ticker.attach(1,count);
*
* @param 无
* @return 无
*/
void count(){
  counter++;
}



void setup() {
  // 连接串口
  Serial.begin(9600);
  Serial.print("serial is OK\n");

  // wifi 连接设置
  wifi_multi_init();
  wifi_multi_con();

  //设置时钟
  setClock();

  //ticker定时
  //ticker.attach(1,count);

  //开启闪存文件系统,为了读取录音文件
  if(SPIFFS.begin()){
    Serial.print("\nSPIFFS Start!!!");
  }
  else{
    Serial.print("\nSPIFFS Failed to start!!!");
  }

  // //token的获取,获取一次就可以了,不要重复获取,容易搞混,建议在电脑端获取直接宏定义使用token即可
  // String res = httpsAddress_baiduToken(APIKey,secretKey);
  // httpsRequest_baiduToken(res);

  //测试音频数据的文字转换
  String testUrl = httpAddressSpeech(cuid,token);
  translateSpeechToText(testUrl);
}



void loop() {
  // //10s发送一次HTTPS请求,获取返回的诗歌
  // if(counter >= 10){
  //   String address = httpsAddress("i","json");
  //   String data_ch = httpsRequest(address);
  //   counter = 0;

  //   delay(1000);
    
  //   String info = httpsAddressBaidu(data_ch,app_id,baidu_salt,secret_key);
  //   baiduHttpReq(info);
  // }
}
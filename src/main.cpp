/*
接线说明:无

程序说明:使用https请求向API发送GET请求,并将返回的内容打印,实现了向一言的请求,可以解析出相应的语句,间隔十秒
        对应的解析JSON的网站是:https://arduinojson.org/v6/assistant/

注意事项:1:HTTPS请求需要请求网页的证书,证书有三个,只能用后两个证书
        2:HTTPS请求首先需要同步网络时间,这是由于协议的要求

函数示例:无

作者:灵首

时间:2023_4_17

*/

//库文件
#include <Arduino.h>
#include <Ticker.h>


//自己写的文件
#include "wifiSet.h"
#include "JSONParseData.h"
#include "md5_generate.h"
#include "clockSet.h"
#include "httpsResquest.h"
#include "httpRequest.h"



Ticker ticker;
int counter;    //用来计数



//百度相关的参数
String app_id = "20230417001645709";
String secret_key = "TckiM1jAl2X0NJM0u0Np";
String baidu_salt = "123456";



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
  ticker.attach(1,count); 
}



void loop() {
  //30s发送一次HTTPS请求,获取返回的诗歌
  if(counter >= 10){
    String address = httpsAddress("i","json");
    String data_ch = httpsRequest(address);
    counter = 0;

    delay(1000);
    
    String info = httpsAddressBaidu(data_ch,app_id,baidu_salt,secret_key);
    baiduHttpReq(info);
  }
}
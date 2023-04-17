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
#include <Arduino.h>
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Ticker.h>



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



WiFiMulti wifi_multi; // 建立WiFiMulti 的对象,对象名称是 wifi_multi
Ticker ticker;
int counter;    //用来计数



/**
* @brief 设置时钟的,需要同步时间(HTTPS请求)
*
* @param 无
* @return 无
*/
void setClock() {
  configTime(0, 0, "pool.ntp.org");

  Serial.print(F("Waiting for NTP time sync: "));
  time_t nowSecs = time(nullptr);
  while (nowSecs < 8 * 3600 * 2) {
    delay(500);
    Serial.print(F("."));
    yield();
    nowSecs = time(nullptr);
  }

  Serial.println();
  struct tm timeinfo;
  gmtime_r(&nowSecs, &timeinfo);
  Serial.print(F("Current time: "));
  Serial.print(asctime(&timeinfo));
}



/**
* @brief 连接WiFi的函数
*
* @param 无
* @return 无
*/
void wifi_multi_con(void)
{
  int i = 0;
  while (wifi_multi.run() != WL_CONNECTED)
  {
    delay(1000);
    i++;
    Serial.print(i);
  }
}



/**
* @brief 写入自己要连接的WiFi名称及密码,之后会自动连接信号最强的WiFi
*
* @param 无
* @return 无
*/
void wifi_multi_init(void)
{
  wifi_multi.addAP("haoze2938", "12345678");
  wifi_multi.addAP("LINGSOU1029", "12345678");
  wifi_multi.addAP("haoze1029", "12345678"); // 通过 wifi_multi.addAP() 添加了多个WiFi的信息,当连接时会在这些WiFi中自动搜索最强信号的WiFi连接
}



/**
* @brief 解析https请求获取的JSON数据,解析出一言,是根据网站代码生成的
*
* @param String data:需要解析的数据
* @return 无
*/
void parseData(String data){
  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc,data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  //JSON解析
  int id = doc["id"]; // 8290
  const char* uuid = doc["uuid"]; // "0d55fdc1-9adf-42af-8c50-6087c3d9c22e"
  const char* hitokoto = doc["hitokoto"]; // "我最喜欢你的那一年，那将是我永远不会再有的十八岁。"
  const char* type = doc["type"]; // "f"
  const char* from = doc["from"]; // "互联网"
  const char* from_who = doc["from_who"]; // "佚名"
  const char* creator = doc["creator"]; // "•᷄ࡇ•᷅"
  int creator_uid = doc["creator_uid"]; // 11568
  int reviewer = doc["reviewer"]; // 1
  const char* commit_from = doc["commit_from"]; // "app"
  const char* created_at = doc["created_at"]; // "1651754699"
  int length = doc["length"]; // 25

  //串口输出解析的一言数据以及作者数据
  String hitokoto_str = doc["hitokoto"].as<String>();
  String from_who_str = doc["from_who"].as<String>();
  Serial.print(hitokoto_str);
  Serial.print("      from who :");
  Serial.print(from_who_str);
  Serial.print("\n");
}



/**
* @brief 解析https请求获取的JSON数据,解析出一言,是根据网站代码生成的
*
* @param String dataFrom:请求资源的来自地点(可以用a~i)(详见https://developer.hitokoto.cn/sentence/)
* @param type dataFrom:需要返回的内容的格式(一般用json)(详见https://developer.hitokoto.cn/sentence/)
* @return 无
*/
String httpsAddress(String dataFrom,String type){
  String requestAddress = "https://v1.hitokoto.cn/?c="
                          + dataFrom + "&encode=" 
                          + type;
  return requestAddress;
}



/**
* @brief 解析https请求获取的JSON数据,解析出一言,是根据网站代码生成的
*
* @param String requestAddress:需要连接的网站
* @return 无
*/
void httpsRequest(String requestAddress){
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
            //对返回的内容进行JSON数据解析
            parseData(payload);
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
  Serial.print("wifi connected!!!\n");

  // 输出连接信息(连接的WIFI名称及开发板的IP地址)
  Serial.print("\nconnect wifi:");
  Serial.print(WiFi.SSID());
  Serial.print("\n");
  Serial.print("\nIP address:");
  Serial.print(WiFi.localIP());
  Serial.print("\n");

  //设置时钟
  setClock(); 

  //ticker定时
  ticker.attach(1,count); 
}



void loop() {
  //10s发送一次HTTPS请求,获取返回的诗歌
  if(counter >= 10){
    String address = httpsAddress("i","json");
    httpsRequest(address);
    counter = 0;
  }
}
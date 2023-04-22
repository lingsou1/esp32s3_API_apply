//这是专门处理JSON数据解析的部分程序 其中代码生成在 https://arduinojson.org/v6/assistant/
#include <Arduino.h>
#include <ArduinoJson.h>



/**
* @brief 解析https请求获取的JSON数据,解析出"一言",是根据网站代码生成的
*
* @param String data:需要解析的数据
* @return String:会返回一个字符串形式的"一言"数据
*/
String parseData(String data){
  StaticJsonDocument<512> doc;

  DeserializationError error = deserializeJson(doc,data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
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

  //return hitokoto_str;
  String data_ch = hitokoto_str;
  return data_ch;
}


/**
* @brief 解析http请求获取的JSON数据,是在解析百度翻译平台返回的数据,会通过串口打印出翻译的数据
*
* @param String data:需要解析的数据
* @return 无
*/
void parseData_baidu(String data){
 

  StaticJsonDocument<256> doc;

  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }

  const char* from = doc["from"]; // "zh"
  const char* to = doc["to"]; // "en"

  const char* trans_result_0_src = doc["trans_result"][0]["src"]; // "读书不觉已春深，一寸光阴一寸金。"
  const char* trans_result_0_dst = doc["trans_result"][0]["dst"]; // "Reading without realizing the depth ...


  //串口输出翻译的数据
  String trans_result_0_dst_en = doc["trans_result"][0]["dst"].as<String>();
  Serial.print(trans_result_0_dst_en);
  Serial.print("\n");

}



/**
* @brief 解析"baiduAccessTokenGet.cpp"中https请求获取的JSON数据,解析需要的Token
*
* @param String data:需要解析的数据,就是HTTPS请求的响应数据
* @return String:会解析请求得到的百度Token数据,有效期在30天,不用每次都获取这个数据
*/
String access_token_JSONParse(String data){
    
  StaticJsonDocument<1536> doc;

  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }
  else{
    const char* refresh_token = doc["refresh_token"];
    long expires_in = doc["expires_in"]; // 2592000
    const char* session_key = doc["session_key"];
    const char* access_token = doc["access_token"];
    const char* scope = doc["scope"]; // "audio_voice_assistant_get brain_enhanced_asr audio_tts_post ...
    const char* session_secret = doc["session_secret"]; // "1a3385b7fb9955cd48c46e36fb3bd484"

    //将Token转化为字符串并返回
    String Token = doc["access_token"].as<String>();
    return Token;
  }
}



/**
* @brief 解析百度短语音识别请求返回的数据,获取识别的中文
*
* @param String data:需要解析的数据,就是HTTP请求的响应数据
* @return String:可以得到识别出的中文,前提是请求成功了
*/
String JSONParse_shortSpeech(String data){

  StaticJsonDocument<256> doc;

  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
  }
  else{
    const char* corpus_no = doc["corpus_no"]; // "7224899475977736294"
    const char* err_msg = doc["err_msg"]; // "success."
    int err_no = doc["err_no"]; // 0

    const char* result_0 = doc["result"][0]; // "不是不是不是啊！"

    const char* sn = doc["sn"]; // "556335756311682177994"

    //将Token转化为字符串并返回
    String msg = doc["result"][0].as<String>();
    return msg;
  }
}
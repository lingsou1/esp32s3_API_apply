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


void parseData_baidu(String data){
  // String input;

  StaticJsonDocument<256> doc;

  DeserializationError error = deserializeJson(doc, data);

  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
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
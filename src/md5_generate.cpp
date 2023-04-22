//用来实现对字符串的MD5加密

#include <MD5Builder.h>


/**
* @brief 对输入的字符串进行MD5加密
*
* @param String data:需要加密的字符串
* @return String:会返回加密后的一段十六进制的数据(以字符串的形式)
*/
String MD5_generate(String data){
  String message = data;
  
  //加密
  MD5Builder md5;
  md5.begin();
  md5.add(message);
  md5.calculate();

  //返回加密结果
  return md5.toString();
}

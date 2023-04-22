

# 实现语音小助手

我有一个想法，希望能实现一个语音小助手。具体实现方式如下：

1. 使用 ESP32S3 板子外接一个 INMP441 麦克风进行录音，并将录音文件暂存至 flash 存储器中。
2. 将录音文件上传到百度语音转文字 API 进行语音识别，并获取返回的文字信息。
3. 将返回的文字信息发送到 ChatGPT 平台，获取相应的回答。
4. 将回答通过麦克风播放出来。

以上是我对于这个程序的最大的想法，实现起来可能有些困难。如果有任何问题，请通过我的电子邮件联系我：haoze20212021@outlook.com。

谢谢！


## 各个文件的简介

1. baiduAccessTokenGet.cpp 

- 该文件用于获取使用百度短语音识别的token参数
- 使用https请求
- 获取这个token的百度文档详见 : [https://ai.baidu.com/ai-doc/REFERENCE/Ck3dwjhhu]

2. clockSet.cpp

- 该文件用于校对时间(应该是)
- HTTPS请求需要先校对时间

3. httpRequest.cpp

- 该文件用于翻译数据(中译英,可自行修改)
- 使用HTTP请求,对百度翻译的API进行请求,会返回翻译数据,对翻译数据进行JSON解析即可
- 同时该程序中使用了MD5加密,百度要求对输入参数进行MD5加密
- 百度翻译的详细内容见 : [https://fanyi-api.baidu.com/product/113]

4. httpsResquest.cpp

- 该文件用于请求获取"一言"数据(中文)
- "一言"的内容详见:[https://developer.hitokoto.cn/sentence/]

5. JSONParseData.cpp

- 该文件用于解析JSON数据
- 其中的代码都是在[https://arduinojson.org/v6/assistant/]生成的

6. md5_generate.cpp

- 该文件是用于对字符串进行MD5加密的,并输出加密后的数据

7. shortSpeechRecognize.cpp

- 该文件用于将pcm格式的语音文件通过API的使用输出为文字
- 使用的是百度短语音识别
- 我使用的是以RAW方式上传音频
- 百度短语音识别的详细内容见:[https://cloud.baidu.com/doc/SPEECH/s/Vk38lxily]
- 同时这是使用的HTTP请求,自定义了请求头,请求体(需要转换的录音文件就放在请求体中的)
- 感谢某位大佬的指点,我尝试使用了PSRAM,但是我现在不是很会使用PSRAM,我写的最后一句释放PSRAM的内存总会报错,我就没有释放内存注释了最后一句,程序也任然可以,以后再研究一下PSRAM吧


8. wifiSet.cpp

- 该文件用于设置需要连接的WiFi,自己需要修改其中的WiFi名称及密码才能链接WiFi
- 同时由于使用了 WiFiMulti.h 所以可以在wifi_multi_init()这个函数中自己添加几个需要的WiFi

## 小提示

1. 我的录音文件放在data文件夹下的,名称是 16K.pcm 这是百度提供的文件,你可以替换成自己的文件,但是需要是.pcm格式
2. 需要先将录音文件上传至闪存中去再下载程序
3. 我尝试自己修改分区表来使用flash,在platformio.ini中添加了 board_build.partitions = partition.csv,同时生成了partition.csv该文件,编译没问题,flash显示程序占用的flash也变大了,下载程序也没报错,但是打开串口没有任何数据输出,取消自己分区后,程序又是正常的了,嗯,很困惑
4. 问题还挺多的,下次再改
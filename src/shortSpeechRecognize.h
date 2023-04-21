#ifndef SHORTSPEECHRECOGNIZE_H
#define SHORTSPEECHRECOGNIZE_H

#include <Arduino.h>

String httpAddressSpeech(String cuid,String token);
String translateSpeechToText(String url);


#endif
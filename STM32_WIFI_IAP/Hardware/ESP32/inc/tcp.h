#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f10x.h"


/*
*以下参数需要用户自行修改才能测试用过
*/

#define User_ESP8266_SSID     "Miley"          //wifi名
#define User_ESP8266_PWD      "manli921228"      //wifi密码

//#define User_ESP8266_SSID     "HONORV30"          //wifi名
//#define User_ESP8266_PWD      "12345678"      //wifi密码

#define User_ESP8266_TCPServer_IP     "192.168.3.154"     //服务器IP
#define User_ESP8266_TCPServer_PORT   "9999"      //服务器端口号


extern volatile uint8_t TcpClosedFlag;  //连接状态标志

void ESP8266_STA_TCPClient_Test(void);

#endif

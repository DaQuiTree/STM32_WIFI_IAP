#ifndef __TCP_H
#define __TCP_H 			   
#include "stm32f10x.h"


/*
*���²�����Ҫ�û������޸Ĳ��ܲ����ù�
*/

#define User_ESP8266_SSID     "Miley"          //wifi��
#define User_ESP8266_PWD      "manli921228"      //wifi����

//#define User_ESP8266_SSID     "HONORV30"          //wifi��
//#define User_ESP8266_PWD      "12345678"      //wifi����

#define User_ESP8266_TCPServer_IP     "192.168.3.154"     //������IP
#define User_ESP8266_TCPServer_PORT   "9999"      //�������˿ں�


extern volatile uint8_t TcpClosedFlag;  //����״̬��־

void ESP8266_STA_TCPClient_Test(void);

#endif

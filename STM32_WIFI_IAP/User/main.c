#include <stdio.h>

#include "sys.h"
#include "delay.h"
#include "flash.h"
#include "usart.h"

#include "led.h"
#include "key.h"

#include "tcp.h"
#include "esp8266.h"
#include "iap.h"

int main(void)
{     
    //system init
    RCC_Configuration(); 
    LED_Init();
    KEY_Init();
    
    
	//开机没有按下则跳转到APP程序中执行
	if(KEY_Read() == 0)
	{
        u32 addr = ApplicationAddress;
        char str[100]={0};
        
        //set irq
        ESP8266_Init(115200);
        
        printf("Hello Software Compiled Time: %s, %s.\n",__DATE__, __TIME__);	//获取软件编译时间  
        
        //AP Join
        ESP8266_AT_Test();
        printf("Configuing ESP32\r\n");
        ESP8266_Net_Mode_Choose(STA);
        while(!ESP8266_JoinAP(User_ESP8266_SSID, User_ESP8266_PWD));
        LEDn1 = 1;
        delay_s(1);
        LEDn1 = 0;
        //Work as TCP Client
        ESP8266_Enable_MultipleId ( DISABLE );
        while(!ESP8266_Link_Server(enumTCP, User_ESP8266_TCPServer_IP, User_ESP8266_TCPServer_PORT, Single_ID_0));
        LEDn1 = 1;
        printf("Config finished.\r\n");    
        
        //Normal传输模式
        sprintf(str, "WantUpdate");//格式化发送字符串到TCP服务器
        ESP8266_SendString(DISABLE, str, 10, Single_ID_0 );   
        
        u8 *ptr;
        int len = 0;
        
        while (ESP8266_RecieveData(&ptr, &len, 1024))
        {   
            printf("Recv len %d\n", len);
            iap_write_appbin(addr, ptr, len);
            addr += 1024;
        }  
        if(len > 0){ //last section
            printf("Recv len %d\n", len);
            iap_write_appbin(addr, ptr, len);
        }
        
        printf("Exit write appbin.\r\n");
        LEDn1 = 0;
        printf("load appbin.\r\n");        
    }
    
    LEDn2 = 1;
    iap_load_app(ApplicationAddress);
    
    while(1);
}



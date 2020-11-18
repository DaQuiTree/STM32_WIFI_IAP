# STM32_WIFI_IAP

## 硬件

洋桃1号开发板、AIThinker ESP32 Node MCU

## 使用

1、IAP TCP Server

(1) windows platform: ts.exe -i:<your ip> -p:<your port>
  
(2) 需要在ts.exe同级目录下存在UPDATE.BIN文件方可完成升级

2、STM32程序按住KEY1复位，程序进入IAP模式

## 总结

1、无论是TCP IAP Server还是在ESP32AT指令集的使用，都使用延迟控制传输速率，这部分做的不好。

2、第一次体验了ESP32的AT指令集，在安信可demo上做的修改，需要在AT指令间加入若干秒的延迟才可以保证wifi正确配网。

个人体验感觉：AT指令集仅适合快速验证（其实也不快），不适合做工程。真正在做IAP过程中，可以由ESP32将new APP存储在芯片内部，STM32空闲时间再将数据写入flash。



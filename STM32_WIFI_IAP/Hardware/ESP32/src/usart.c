#include "usart.h"
#include "esp8266.h"
#include "tcp.h"


#pragma import(__use_no_semihosting)             
//��׼����Ҫ֧�ֵĺ���
struct __FILE 
{
	int handle;
};

FILE __stdout;       
//����_sys_exit()�Ա��⹤���ڰ�����״̬
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc����
//�����Ҫ����MCU������ϣ��printf���ĸ����������ȷ�� __WAIT_TODO__
int fputc(int ch, FILE *f)
{
	//ע�⣺USART_FLAG_TXE�Ǽ�鷢�ͻ������Ƿ�Ϊ�գ����Ҫ�ڷ���ǰ��飬������������߷���Ч�ʣ����������ߵ�ʱ����ܵ������һ���ַ���ʧ
	//USART_FLAG_TC�Ǽ�鷢����ɱ�־������ڷ��ͺ��飬����������˯�߶�ʧ�ַ����⣬����Ч�ʵͣ����͹����з��ͻ������Ѿ�Ϊ���ˣ����Խ�����һ�������ˣ�������ΪҪ�ȴ�������ɣ�����Ч�ʵͣ�
	//��Ҫ����һ���ã�һ����Ч�����
	
	//ѭ���ȴ�ֱ�����ͻ�����Ϊ��(TX Empty)��ʱ���Է������ݵ�������
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    USART_SendData(USART1, (uint8_t) ch);

    /* ѭ���ȴ�ֱ�����ͽ���*/
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);

	return ch;
}


void uart2_Init(u32 bound)//����2  ����ΪPA2  PA3
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	    //ʹ��ָ���˿�ʱ��
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��GPIO
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	//��ʼ��GPIO
	
	//Usart2 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         
    NVIC_Init(&NVIC_InitStructure); 
	
	//USART2����
	USART_InitStructure.USART_BaudRate = bound;	//���ô��ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;	//�ֳ�Ϊ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;	//1��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;	//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
	USART_Init(USART2, &USART_InitStructure); //����USART����
	
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����˽����жϺ����߿����ж�
    USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART2, ENABLE);                    //ʹ��USART
    USART_ClearFlag(USART2, USART_FLAG_TC);
}

void USART2_IRQHandler(void)
{   
    u8 ucCh = ucCh;

    if(USART_GetITStatus( USART2, USART_IT_RXNE ) != RESET )
    {	  
        if(ESP8266_Fram_Record_Struct.FramLength < (RX_BUF_MAX_LEN - 1)) 
            ESP8266_Fram_Record_Struct.Data_RX_BUF[ESP8266_Fram_Record_Struct.FramLength++] = USART_ReceiveData(USART2);   
    }else if( USART_GetITStatus( USART2, USART_IT_IDLE ) != RESET ){                                         //������߿���
        ucCh = USART2->SR;
        ucCh = USART2->DR;
        ESP8266_Fram_Record_Struct.FramFinishFlag = 1;
        //printf("end. %d. \r\n", ESP8266_Fram_Record_Struct.FramLength);
        //ucCh = USART_ReceiveData( USART2 );                                                              //�������������жϱ�־λ���ȶ�USART_SR,Ȼ���USART_DR��
    }   

}


void uart1_Init(u32 bound)//����1
{
    GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	
     //USART1_TX   PA.9
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	
    GPIO_Init(GPIOA, &GPIO_InitStructure);  
    //USART1_RX	  PA.10
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

	USART_InitStructure.USART_BaudRate = bound;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	
    USART_Init(USART1, &USART_InitStructure); 

    USART_Cmd(USART1, ENABLE);   
    USART_ClearFlag(USART1,USART_FLAG_TC); 
}

void USART1_IRQHandler( void )
{   

}

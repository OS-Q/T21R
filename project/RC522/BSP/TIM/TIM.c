/*-----------------------------------------------------------------*/
/*********************��ʱ����ʼ�����жϺ���************************/
/*-----------------------------------------------------------------*/  
#include "TIM.h" 
#include "stm32f0xx.h"

extern uint8_t DATA,DATA1;  //DATA��ʱ����ʱ�洢����  DATA1�����������������
extern __IO uint8_t receivedFlag; //������ɱ�־λ

/***********************************************************************************
*     ע�⣺�����ڵ����ڼ䷢�ַ���ʱ��ҪС�ڽ���ʱ��
* 9600������ʱ    SendingDelay=104    TIME3_init(108,72c)
* 115200������ʱ    SendingDelay=8    TIME3_init(10,72c)
************************************************************************************/
void TIM3_init(u16 arr,u16 psc)
{
 
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��

	TIM_TimeBaseStructure.TIM_Period = arr -1;			//��0~��arr-1����������ʱ����Ϊarr��
	TIM_TimeBaseStructure.TIM_Prescaler = psc-1;		//Ƶ��48mhz/psc,һ����1Mhz��1us��
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;		//ʱ�Ӳ���Ƶ
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);     

	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;      //��ʼ�����жϵ�û��ʹ��
	NVIC_InitStructure.NVIC_IRQChannelPriority = 1;   
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

void TIM3_IRQHandler(void)
{  
	uint8_t tmp;
	static uint8_t i;
	if(TIM_GetFlagStatus(TIM3, TIM_FLAG_Update) != RESET)
	{
		tmp = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);
		if(tmp == 1)
		DATA |= (1 << i); 
		i++;
		if(i >= 8)
		{
			i = 0;
			DATA1=DATA; 
			DATA=0;
			receivedFlag = 1;
			EXTI->IMR |= 1<<10;   	//�����ⲿ�ж�
			TIM_Cmd(TIM3,DISABLE); 	//�ر�TIM3
		}
		TIM_ClearITPendingBit(TIM3, TIM_FLAG_Update);
	}
}
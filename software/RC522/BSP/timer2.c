#include "timer2.h"

//通用定时器2中断初始化
//arr：自动重装值。
//psc：时钟预分频数
//这里使用的是定时器2
static volatile uint16_t beep_times;

// 计算公式：
void TIM2_Int_Init(u16 arr,u16 psc)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;				//定时器配置结构体定义
	NVIC_InitTypeDef NVIC_InitStructure;						      //中断配置结构体定义

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  //时钟使能
	
	//定时器TIM2初始化: 定时器频率((1+TIM_Prescaler )/72M)*(1+TIM_Period )
	TIM_TimeBaseStructure.TIM_Period = arr;    //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc;  //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim	
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //使能指定的TIM2中断,允许更新中断	TIM_IT_Update

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM2中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	//TIM_Cmd(TIM2, ENABLE);  //使能TIM2			 
}

void beep_start(uint16_t times)
{
  beep_times = times;
  TIM_Cmd(TIM2, ENABLE);
}


//定时器2中断服务程序
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //检查TIM2更新中断发生与否
	{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );   //清除TIM2更新中断标志 
		GPIOB->ODR ^= GPIO_Pin_4;
	}
  
  if(beep_times-- <= 0)
  {
    TIM_Cmd(TIM2, DISABLE);
    GPIO_ResetBits(GPIOB,GPIO_Pin_4);
  }  
}













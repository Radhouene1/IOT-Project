#include "stm32f4xx.h"
  int x=0;
  int y=0;
  int z=1;
  int fin=1;
  
  uint8_t x1,x2,x3,x4;

int uart_off=0;
#include "stdio.h"
#include <stdlib.h>
//taille de tableau buffer 
#define RXBUFFERSIZE   0x04 

uint8_t aRxBuffer[RXBUFFERSIZE];
uint8_t ubNbrOfDataToRead = RXBUFFERSIZE; 
__IO uint16_t uhRxCounter = 0;
//UART interppution
void UART4_IRQHandler(void)
{
  if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)
  {
    
    aRxBuffer[uhRxCounter++] = (USART_ReceiveData(UART4) & 0x1FF);

    if(uhRxCounter == ubNbrOfDataToRead)
    {
     USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
    
     //x=(aRxBuffer[2]%30)*10+(aRxBuffer[3]%30);
     y=(aRxBuffer[2]-48)*10+(aRxBuffer[3]-48);
     x=(aRxBuffer[0]-48)*10+(aRxBuffer[1]-48);
     uart_off=1;
    }
    USART_ClearITPendingBit (UART4,USART_IT_RXNE);
  }

}

void GPIO_UART_Config(void)
{
  GPIO_InitTypeDef     GPIO_InitStruct;
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
  
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF_UART4);
  GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF_UART4);
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_10;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOC, &GPIO_InitStruct);
}
void USART_Config(void)
{
  USART_InitTypeDef USART_InitStructure;
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
  
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx;
  USART_Init (UART4, &USART_InitStructure);
  USART_Cmd (UART4, ENABLE);
}
void NVIC_UART_Config(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}




void Delay(int n)
{
while(n!=0)
n--;
}

void buzzer()
{
TIM3->CCR1=5000;
Delay(0xFFF);
    MT_WH1602_ClearDisplay();
    MT_WH1602_Delay(2000);
    Delay(0xFFF);
    MT_WH1602_WriteData(x1);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(x2);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(0x3A);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(x3);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(x4);
    MT_WH1602_Delay(100);
}

// timer 4 interruption 
void TIM4_IRQHandler()
{
if(TIM_GetITStatus(TIM4,TIM_IT_Update)!=RESET)
{
TIM_ClearITPendingBit(TIM4,TIM_IT_Update);

if (uart_off==1)
  {
buzzer();
  
if(z==1)
{
    if(y==0)
  {
  if(x==0){z=0;}
  x--;
  y=59;
  }
  else
  { y--; }
  x1=(x/10)+0x30;
  x2=(x%10)+0x30;
  x3=(y/10)+0x30;
  x4=(y%10)+0x30;
  
} 
else
{
x1=0x42;
x2=0x4F;
x3=0x4F;
x4=0x4D;
    MT_WH1602_ClearDisplay();
    MT_WH1602_Delay(2000);  
    MT_WH1602_WriteData(x1);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(x2);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(x2);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(x3);
    MT_WH1602_Delay(100);
    MT_WH1602_WriteData(x4);
    MT_WH1602_Delay(100);
 TIM_ITConfig(TIM4,TIM_IT_Update, DISABLE);// fermeteur de timer 4 
fin=0;
}
}
}
}
//reglage timer 3 et pwm2 pour la fréquence de buzzer
void PWM_Test (void)
{
  GPIO_InitTypeDef   GPIO_InitStruct;
  TIM_TimeBaseInitTypeDef TIM_BaseStruct;
  TIM_OCInitTypeDef TIM_OCStruct;
  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
  
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_TIM3);//laison tim3 et pwm
  
  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  TIM_BaseStruct.TIM_Prescaler = 0;
  TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_BaseStruct.TIM_Period = 8000;
  TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
  TIM_BaseStruct.TIM_RepetitionCounter = 0;
  
  TIM_TimeBaseInit(TIM3, &TIM_BaseStruct);
  TIM_PrescalerConfig(TIM3, 0, TIM_PSCReloadMode_Immediate);
  
  TIM_OCStruct.TIM_OCMode = TIM_OCMode_PWM2;
  TIM_OCStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCStruct.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCStruct.TIM_Pulse = 5000;//duty cycle  
  TIM_OC1Init(TIM3, &TIM_OCStruct);   /// oc1 chanel 1()
  TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Enable);
  
  TIM_Cmd(TIM3, ENABLE);
  TIM_CtrlPWMOutputs(TIM3, ENABLE);
  
}

// reglage timer 4  pour chaque seconde on fait une interppution
  void TM_TIMER_Init(void) {
    
      NVIC_InitTypeDef   NVIC_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_BaseStruct;
    
    /* Enable clock for TIM4 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_BaseStruct.TIM_Prescaler = 9999;
    /* Count up */
    TIM_BaseStruct.TIM_CounterMode = TIM_CounterMode_Up;

    TIM_BaseStruct.TIM_Period = 8399; 
    TIM_BaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_BaseStruct.TIM_RepetitionCounter = 0;
    /* Initialize TIM4 */
    TIM_TimeBaseInit(TIM4, &TIM_BaseStruct);
    /* Start count on TIM4 */
    TIM_Cmd(TIM4, ENABLE);
    //interruption
    TIM_ITConfig(TIM4,TIM_IT_Update, ENABLE);//timer apelle interrupt

    //periorety
  NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0; //
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

    
}

  
int main()
{

  GPIO_UART_Config();
  
  USART_Config();
  
  NVIC_UART_Config();
 
  
    MT_WH1602_Init();
 
    MT_WH1602_FunctionSet(1, 0, 0);
    MT_WH1602_Delay(1000);
    MT_WH1602_FunctionSet(1, 0, 0);
    MT_WH1602_Delay(1000);
    MT_WH1602_FunctionSet(1, 0, 0);
    MT_WH1602_Delay(1000);
    MT_WH1602_FunctionSet(1, 1, 1);
    MT_WH1602_Delay(1000);	
    MT_WH1602_DisplayOnOff(1, 0, 0);
    MT_WH1602_Delay(1000);
    MT_WH1602_ClearDisplay();
    MT_WH1602_Delay(2000);
  
    
  TM_TIMER_Init();// init timer 4 
  PWM_Test ();// init timer3 et pwm2
  while(1)
  {
  USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
  TIM3->CCR1=0;
    
    
  if (uart_off==1)  
  {if(fin==1)
    {TIM3->CCR1=0;}
    else
    {TIM3->CCR1=5000;}
  }
  
  }

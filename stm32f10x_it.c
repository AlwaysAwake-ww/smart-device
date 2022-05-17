/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

/* Keyseek Lew*/

#include "device_driver.h"

void Invalid_ISR(void)
{
  Uart1_Printf("Invalid_Exception: %d!\n", Macro_Extract_Area(SCB->ICSR, 0x1ff, 0));
  Uart1_Printf("Invalid_ISR: %d!\n", Macro_Extract_Area(SCB->ICSR, 0x1ff, 0)-16);
  for(;;);
}


#define STACK_DUMP  16

/*******************************************************************************
 * Function Name  : NMIException
 * Description    : This function handles NMI exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void NMI_Handler(void)
{
  Peri_Set_Bit(RCC->CIR, 23);
  Uart1_Printf("HSE fail!\n");
  
  /* Write rescue code */
}

/*******************************************************************************
 * Function Name  : HardFaultException
 * Description    : This function handles Hard Fault exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void HardFault_Handler(void)
{
  int i;
  unsigned int * sp = (unsigned int *)__get_MSP();
  
  Uart1_Printf("Hard Fault!\n");
  
  for(i=0; i<STACK_DUMP; i++)
  {
    Uart1_Printf("SP[%d]=0x%.8X\n", i, sp[i]);
  }    
  
  Uart1_Printf("HFSR => %#.8X\n", SCB->HFSR);
  Uart1_Printf("SHCSR => %#.8X\n", SCB->SHCSR);
  Uart1_Printf("Fault Reason => %#.8X\n", SCB->CFSR);
  Uart1_Printf("MMFAR => %#.8X\n", SCB->MMFAR);        
  Uart1_Printf("BFAR => %#.8X\n", SCB->BFAR);
  SCB->HFSR = (1u << 31)|(30 << 1)|(1 << 1);
  SCB->CFSR = 0xffff << 0;
  
  for(;;);
}

/*******************************************************************************
 * Function Name  : MemManageException
 * Description    : This function handles Memory Manage exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void MemManage_Handler(void)
{
  int i;
  unsigned int * sp = (unsigned int *)__get_MSP();

  Uart1_Printf("Memory Management Fault!\n");  
  
  for(i=0; i<STACK_DUMP; i++)
  {
    Uart1_Printf("SP[%d]=0x%.8X\n", i, sp[i]);
  }  
  
  Uart1_Printf("SHCSR => %#.8X\n", SCB->SHCSR);
  Uart1_Printf("Fault Reason => %#.8X\n", SCB->CFSR);
  Uart1_Printf("MMFAR Valid => %d\n", Macro_Check_Bit_Set(SCB->CFSR, 7));
  Uart1_Printf("MMFAR => %#.8X\n", SCB->MMFAR);   
  SCB->CFSR = 0xff << 0;
  
  for(;;);
}

/*******************************************************************************
 * Function Name  : BusFaultException
 * Description    : This function handles Bus Fault exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void BusFault_Handler(void)
{
  int i;
  unsigned int * sp = (unsigned int *)__get_MSP();
  
  Uart1_Printf("BUS Fault!\n");
  
  for(i=0; i<STACK_DUMP; i++)
  {
    Uart1_Printf("SP[%d]=0x%.8X\n", i, sp[i]);
  }  
  
  Uart1_Printf("SHCSR => %#.8X\n", SCB->SHCSR);
  Uart1_Printf("Fault Reason => %#.8X\n", SCB->CFSR);
  Uart1_Printf("BFAR Valid => %d\n", Macro_Check_Bit_Set(SCB->CFSR, 15));
  Uart1_Printf("BFAR => %#.8X\n", SCB->BFAR);
  SCB->CFSR = 0xff << 8;
  
  for(;;);
}

/*******************************************************************************
 * Function Name  : UsageFaultException
 * Description    : This function handles Usage Fault exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void UsageFault_Handler(void)
{
  int i;
  unsigned int * sp = (unsigned int *)__get_MSP();
  
  Uart1_Printf("Usage Fault!\n");
  
  for(i=0; i<STACK_DUMP; i++)
  {
    Uart1_Printf("SP[%d]=0x%.8X\n", i, sp[i]);
  }  
  
  Uart1_Printf("Saved CPSR(IPSR)=0x%.2X\n", sp[7] & 0xFF);
  Uart1_Printf("SHCSR => %#.8X\n", SCB->SHCSR);
  Uart1_Printf("Fault Reason => %#.8X\n", SCB->CFSR);
  SCB->CFSR = 0xffffu << 16;
  
  for(;;);
}

/*******************************************************************************
 * Function Name  : SVCHandler
 * Description    : This function handles SVCall exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SVC_Handler(void)
{
  Uart1_Printf("SVC Call\n");
}

/*******************************************************************************
 * Function Name  : DebugMonitor
 * Description    : This function handles Debug Monitor exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DebugMon_Handler(void)
{
  Uart1_Printf("DebugMon Call\n");
}

/*******************************************************************************
 * Function Name  : PendSVC
 * Description    : This function handles PendSVC exception.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void PendSV_Handler(void)
{
  Uart1_Printf("PendSV Call\n");
  SCB->ICSR = 1<<27;
}

/*******************************************************************************
 * Function Name  : SysTickHandler
 * Description    : This function handles SysTick Handler.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int SysTick_Flag = 0;

void SysTick_Handler(void)
{
  SysTick_Flag = 1;
}

/*******************************************************************************
 * Function Name  : WWDG_IRQHandler
 * Description    : This function handles WWDG interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int WWDG_Time = 0;

void WWDG_IRQHandler(void)
{
  WWDG->CR = WWDG_Time;
  WWDG->SR = 0;
  NVIC_ClearPendingIRQ((IRQn_Type)0);
  
  /* 시험용 추가 코드 */
  Uart1_Printf("@\n");
}

/*******************************************************************************
 * Function Name  : PVD_IRQHandler
 * Description    : This function handles PVD interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void PVD_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TAMPER_IRQHandler
 * Description    : This function handles Tamper interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int Tamper_Occur = 0;

void TAMPER_IRQHandler(void)
{
  Tamper_Occur = 1;
  
  Peri_Set_Bit(PWR->CR, 8);
  Peri_Set_Bit(BKP->CSR, 0);
  Peri_Set_Bit(BKP->CSR, 1);
  Peri_Clear_Bit(PWR->CR, 8);
  
  NVIC_ClearPendingIRQ((IRQn_Type)2);
}

/*******************************************************************************
 * Function Name  : RTC_IRQHandler
 * Description    : This function handles RTC global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int RTC_alarm_flag = 0;
volatile int RTC_overflow_flag = 0;
volatile int RTC_second_flag = 0;

void RTC_IRQHandler(void)
{
  if(Peri_Check_Bit_Set(RTC->CRL, 0))
  {
    Peri_Clear_Bit(RTC->CRL,0);
    RTC_second_flag = 1;
  }
  
  if(Peri_Check_Bit_Set(RTC->CRL, 1))
  {
    Peri_Clear_Bit(RTC->CRL,1);
    RTC_alarm_flag = 1;
  }
  
  if(Peri_Check_Bit_Set(RTC->CRL, 2))
  {
    Peri_Clear_Bit(RTC->CRL,2);
    RTC_overflow_flag = 1;
  }
  
  NVIC_ClearPendingIRQ((IRQn_Type)3);
}

/*******************************************************************************
 * Function Name  : FLASH_IRQHandler
 * Description    : This function handles Flash interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void FLASH_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : RCC_IRQHandler
 * Description    : This function handles RCC interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RCC_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI0_IRQHandler
 * Description    : This function handles External interrupt Line 0 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI0_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI1_IRQHandler
 * Description    : This function handles External interrupt Line 1 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI2_IRQHandler
 * Description    : This function handles External interrupt Line 2 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI3_IRQHandler
 * Description    : This function handles External interrupt Line 3 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI3_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI4_IRQHandler
 * Description    : This function handles External interrupt Line 4 request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI4_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel1_IRQHandler
 * Description    : This function handles DMA1 Channel 1 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel2_IRQHandler
 * Description    : This function handles DMA1 Channel 2 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel3_IRQHandler
 * Description    : This function handles DMA1 Channel 3 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel3_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel4_IRQHandler
 * Description    : This function handles DMA1 Channel 4 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel4_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel5_IRQHandler
 * Description    : This function handles DMA1 Channel 5 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel5_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel6_IRQHandler
 * Description    : This function handles DMA1 Channel 6 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel6_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : DMA1_Channel7_IRQHandler
 * Description    : This function handles DMA1 Channel 7 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void DMA1_Channel7_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : ADC1_2_IRQHandler
 * Description    : This function handles ADC1 and ADC2 global interrupts requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int      ADC1_value = 0;
volatile int      ADC1_flag  = 0;
void ADC1_2_IRQHandler(void)
{
    NVIC_ClearPendingIRQ((IRQn_Type)18);
    Macro_Clear_Bit(ADC1->SR,1);
    ADC1_flag = 1;
    ADC1_value = ADC1->DR;
    Uart1_Printf("ADC IT\n");
    NVIC_ClearPendingIRQ((IRQn_Type)18);
}

/*******************************************************************************
 * Function Name  : USB_HP_CAN_TX_IRQHandler
 * Description    : This function handles USB High Priority or CAN TX interrupts
 *                  requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USB_HP_CAN_TX_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : USB_LP_CAN_RX0_IRQHandler
 * Description    : This function handles USB Low Priority or CAN RX0 interrupts
 *                  requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USB_LP_CAN_RX0_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : CAN_RX1_IRQHandler
 * Description    : This function handles CAN RX1 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void CAN_RX1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : CAN_SCE_IRQHandler
 * Description    : This function handles CAN SCE interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void CAN_SCE_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : EXTI9_5_IRQHandler
 * Description    : This function handles External lines 9 to 5 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM1_BRK_IRQHandler
 * Description    : This function handles TIM1 Break interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_BRK_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM1_UP_IRQHandler
 * Description    : This function handles TIM1 overflow and update interrupt
 *                  request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_UP_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM1_TRG_COM_IRQHandler
 * Description    : This function handles TIM1 Trigger and commutation interrupts
 *                  requests.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_TRG_COM_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM1_CC_IRQHandler
 * Description    : This function handles TIM1 capture compare interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM1_CC_IRQHandler(void)
{  

}

/*******************************************************************************
 * Function Name  : TIM2_IRQHandler
 * Description    : This function handles TIM2 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int TIM2_Expired =0;
void TIM2_IRQHandler(void)
{
	Peri_Clear_Bit(TIM2->SR, 0);
	NVIC_ClearPendingIRQ((IRQn_Type)28);
	TIM2_Expired=1;
}

/*******************************************************************************
 * Function Name  : TIM3_IRQHandler
 * Description    : This function handles TIM3 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void TIM3_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : TIM4_IRQHandler
 * Description    : This function handles TIM4 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int TIM4_Expired = 0;
volatile int led_cnt = 0;

extern int led_cnt_value;

void TIM4_IRQHandler(void)
{
  Peri_Clear_Bit(TIM4->SR, 0);
  NVIC_ClearPendingIRQ((IRQn_Type)30);
  TIM4_Expired = 1;
	if(led_cnt==led_cnt_value){
		TIM4_Repeat_Interrupt_Enable(0, 300);
		led_cnt=0;
	}
}

/*******************************************************************************
 * Function Name  : I2C1_EV_IRQHandler
 * Description    : This function handles I2C1 Event interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C1_EV_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : I2C1_ER_IRQHandler
 * Description    : This function handles I2C1 Error interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C1_ER_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : I2C2_EV_IRQHandler
 * Description    : This function handles I2C2 Event interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C2_EV_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : I2C2_ER_IRQHandler
 * Description    : This function handles I2C2 Error interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void I2C2_ER_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : SPI1_IRQHandler
 * Description    : This function handles SPI1 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SPI1_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : SPI2_IRQHandler
 * Description    : This function handles SPI2 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void SPI2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : USART1_IRQHandler
 * Description    : This function handles USART1 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
volatile int Uart1_Rx_In = 0;
volatile int Uart1_Rx_Data = 0;

void USART1_IRQHandler(void)
{
	Uart1_Rx_Data = (char)USART1->DR;
  NVIC_ClearPendingIRQ((IRQn_Type)37);
  Uart1_Rx_In = 1;
}

/*******************************************************************************
 * Function Name  : USART2_IRQHandler
 * Description    : This function handles USART2 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USART2_IRQHandler(void)
{
  Invalid_ISR();
}

/*******************************************************************************
 * Function Name  : USART3_IRQHandler
 * Description    : This function handles USART3 global interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USART3_IRQHandler(void)
{
  Invalid_ISR();
}


int GetKeyValue(){
	if(Peri_Check_Bit_Set(EXTI->PR,13)){
		return 0;
	}
  if(Peri_Check_Bit_Set(EXTI->PR,14)){
		return 1;
	}
  if(Peri_Check_Bit_Set(EXTI->PR,15)){
		return 2;
	}
	return 0;
}


/*******************************************************************************
 * Function Name  : EXTI15_10_IRQHandler
 * Description    : This function handles External lines 15 to 10 interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
#define BASE  (500) //msec
enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
enum note{N16=BASE/4, N8=BASE/2, N4=BASE, N2=BASE*2, N1=BASE*4};
extern const int song1[][2];
extern const int song2[][2]; 
extern const char * note_name[];

extern void buzzer_beep_stop();

extern int lcd_status;
extern char CLIENT_SOCKET;
extern int client_cnt;
extern unsigned char ID_buf[1500];


extern volatile unsigned int call_flag;
extern volatile unsigned int fire_flag;
extern volatile unsigned int quake_flag;
extern int buzzer_tone;
extern unsigned char clientID[1500];
int lcd_mod=0;
		// 0 : 기본, 1 : 클라이언트->서버 통화모드(calling..), 2 : 클라->서버 통화 연결됨(connected)
void EXTI15_10_IRQHandler(void)
{
	int Key_Value=	GetKeyValue();
	
	// 기본모드
	if(lcd_mod==0){
		switch(Key_Value){
			// LCD 화면 전환 (택배현황, NOTICE, LOCK/OPEN 현황
		case 0:
			lcd_status++;
			if(lcd_status>2)
				lcd_status=0;
			lcd_refresh(lcd_status);
			break;
		case 1:
//			TIM2_Interrupt_Enable(1, 200);
//			buzzer_tone=D2;
			break;
		case 2:
			break;
		}
	}
	// 전화가 걸려 옴 (calling) || 전화를 검
	else if(lcd_mod==1){
		switch(Key_Value){
		case 0:	// 전화 받기
			call_flag =1;
			led_cnt=1000;
			lcd_status=6;
			lcd_refresh(lcd_status);
			lcd_mod=2;
			buzzer_beep_stop();
			LED_Off();
			TIM4_Repeat_Interrupt_Enable(0,0);
			break;
		case 1:	// 전화 끊기 (연결을 하는 도중 calling)
			call_flag=2;
			buzzer_beep_stop();
			LED_Off();
			lcd_status=-1;
			Lcd_Clr_Screen(WHITE);
			Lcd_Printf(5,70, BLACK, WHITE, 1, 1, "%s", "DISCONNECTED...");
			lcd_mod=0;

			break;
		case 2:
			break;
		}		
	}
	// 통화 중 모드 (connected)
	else if(lcd_mod==2){
		switch(Key_Value){
		case 0:
			break;
		case 1:	// 통화 중 끊기 
			call_flag =2;
			lcd_status=-1;
			Lcd_Clr_Screen(WHITE);
			Lcd_Printf(5,70, BLACK, WHITE, 1, 1, "%s", "DISCONNECTED...");
			lcd_mod=0;
			buzzer_beep_stop();
			TIM4_Repeat_Interrupt_Enable(0,0);
			LED_Off();
			break;
		case 2:
			break;
		}
	}
	else if(lcd_mod==3){		// 화재 경보 발생 
		switch(Key_Value){
		case 0:		// 경보 발생 상황에서 0번 키 입력 시 경보 종료
			lcd_status=-1;
			lcd_refresh(lcd_status);
			lcd_mod=0;
			fire_flag=1;
			buzzer_beep_stop();
			TIM4_Repeat_Interrupt_Enable(0,0);
			LED_Off();
			break;
		case 1:	
			if(lcd_status==7)
				lcd_status=8;
			else if(lcd_status==8)
				lcd_status=7;
			
			lcd_refresh(lcd_status);
			
			break;
		case 2:	
			break;
		}
	}
	
	else if(lcd_mod==4){		// 지진 경보 발생 
		switch(Key_Value){ 
		case 0:		// 경보 발생 상황에서 0번 키 입력 시 경보 종료
			lcd_status=-1;
			lcd_refresh(lcd_status);
			lcd_mod=0;
			quake_flag=1;
			buzzer_beep_stop();
			TIM4_Repeat_Interrupt_Enable(0,0);
			LED_Off();
			break;
		case 1:	
			if(lcd_status==3)
				lcd_status=4;
			else if(lcd_status==4)
				lcd_status=3;
			lcd_refresh(lcd_status);
			break;
		case 2:	
			break;
		}
	}
	
	//	 Key_Value = Macro_Extract_Area(EXTI->PR, 0x7, 13);
		
		
		//	 Interrupt Clear
		EXTI->PR = 0x7<<13;
  NVIC_ClearPendingIRQ((IRQn_Type)40);
}


/*******************************************************************************
 * Function Name  : RTCAlarm_IRQHandler
 * Description    : This function handles RTC Alarm interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void RTCAlarm_IRQHandler(void)
{
  /* For STOP Mode Test Only */
  Peri_Set_Bit(PWR->CR,0);
  Set_Clock_72MHz_HSE();

  Uart1_Printf("EXTI Alarm Wakeup!\n");
  Peri_Set_Bit(EXTI->PR, 17);
  NVIC_ClearPendingIRQ((IRQn_Type)41);
}

/*******************************************************************************
 * Function Name  : USBWakeUp_IRQHandler
 * Description    : This function handles USB WakeUp interrupt request.
 * Input          : None
 * Output         : None
 * Return         : None
 *******************************************************************************/
void USBWakeUp_IRQHandler(void)
{
  Invalid_ISR();
}

/******************* (C) COPYRIGHT 2008 STMicroelectronics *****END OF FILE****/

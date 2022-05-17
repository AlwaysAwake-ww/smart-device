#include "device_driver.h"
#include "st7735.h"
#define BLACK	0x0000
#define WHITE	0xffff
#define BLUE	0x001f
#define GREEN	0x07E0
#define RED		0xf800
#include <string.h>
#define BASE  (500) //msec

RCC_ClocksTypeDef RCC_ClockFreq;
extern volatile int TIM4_Expired;

extern volatile int led_cnt;
extern volatile int TIM2_Expired;

int buzzer_value=0;
int buzzer_index=0;
int buzzer_flag=0;
int buzzer_tone=0;

int buzzer_duration;
enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
enum note{N16=BASE/4, N8=BASE/2, N4=BASE, N2=BASE*2, N1=BASE*4};
const int song1[][2] = {{G1,N4},{G1,N4},{E1,N8},{F1,N8},{G1,N4},{A1,N4},{A1,N4},{G1,N2},{G1,N4},{C2,N4},{E2,N4},{D2,N8},{C2,N8},{D2,N2}};
const int song2[][2] = {{C1,N4},{D1,N4},{E1,N4},{E1,N4},{C1,N4},{D1,N4},{E1,N4},{E1,N4},{C1,N4},{E1,N4},{G1,N8},{A1,N8},{G1,N4}};  
const char * note_name[] = {"C1", "C1#", "D1", "D1#", "E1", "F1", "F1#", "G1", "G1#", "A1", "A1#", "B1", "C2", "C2#", "D2", "D2#", "E2", "F2", "F2#", "G2", "G2#", "A2", "A2#", "B2"};

////////////////////////////////////////////////
// Costom Value 
////////////////////////////////////////////////

////////////////////////////////////////////////
// Costom Function
////////////////////////////////////////////////

void TIM3_Buzzer_Costom_Beep(int tone);
void Buzzer_Delay_Start(int time);
void Buzzer_Delay_Check(){
	
	if(Peri_Check_Bit_Clear(TIM2->SR, 0)){
		Peri_Clear_Bit(TIM2->CR1, 0);
		Peri_Clear_Bit(TIM2->DIER, 0);
		Peri_Clear_Bit(RCC->APB1ENR, 0);
		Peri_Set_Bit(TIM3->EGR,0);
		Peri_Clear_Bit(TIM3->CR1,0);	
	}
}


void buzzer_beep(){
						
}

void buzzer_beep_stop(){
	TIM2_Interrupt_Enable(0,0);
	Peri_Clear_Bit(TIM3->CR1,0);
	Peri_Set_Bit(TIM3->EGR,0);
}
////////////////////////////////////////////////
int main(void){
	

  RCC_GetClocksFreq(&RCC_ClockFreq);
	
	
  Uart1_Init(115200);
  Uart1_Printf("Ethernet TCP Server Test\n");
	LED_Init();
  Ethernet_Init();
	Key_ISR_Enable(1);
	Lcd_Init();
	TIM3_Buzzer_Init();
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(15,25, BLACK,WHITE,2,2,"%s", "SERVER");
	Lcd_Printf(40,60, BLACK,WHITE,1,1,"%s", "START");
  for(;;)
  {
	  loopback_tcps(0, 5000);
		loopback_tcps(1, 5001);
		loopback_tcps(2, 5002);
		
		if(TIM4_Expired)
    {
			static int d=1;
			LED_Display(d^=0x3);
      TIM4_Expired = 0;
			led_cnt++;
		}
		
		
		// 何历狼 林扁
		if(TIM2_Expired==1){
			if(buzzer_value==1){
				TIM3_Buzzer_Costom_Beep(buzzer_tone);
				// 何历 柯
				send_buzzer_signal();
				buzzer_value=0;
			}
			else if(buzzer_value==0){
				Peri_Clear_Bit(TIM3->CR1,0);
				Peri_Set_Bit(TIM3->EGR,0);
				// 何历 坷橇
				send_buzzer_signal();
				buzzer_value=1;
			}
			TIM2_Expired=0;
		}
		
		
		
		//		else if(TIM2_Expired){
		////			Uart1_Printf("TIM2_Expired ==1 !!\n");
		//			buzzer_index++;
		//			if(buzzer_index>sizeof(song1)/(sizeof(song1[0]))){
		//				buzzer_index=0;
		//			}
		//				
		//			TIM3_Buzzer_Beep(song1[buzzer_index][0], N16);	
		//			
		//			TIM2_Expired=0;
		//			
		//		}
  }
}

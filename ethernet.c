#include "stm32f10x.h"
#include "socket.h"	// Just include one header for WIZCHIP
#include "udf.h"
#include <stdlib.h>
#include <stdio.h>
#include "st7735.h"
#include <string.h>
/* Private define ------------------------------------------------------------*/

/*
 @brief Peer IP register address
*/
#define Sn_DIPR0(ch)                    (0x000C08 + (ch<<5))
#define Sn_DIPR1(ch)                    (0x000D08 + (ch<<5))
#define Sn_DIPR2(ch)                    (0x000E08 + (ch<<5))
#define Sn_DIPR3(ch)                    (0x000F08 + (ch<<5))
/**
 @brief Peer port register address
 */
#define Sn_DPORT0(ch)                   (0x001008 + (ch<<5))
#define Sn_DPORT1(ch)                   (0x001108 + (ch<<5))


#define ETH_DATA_BUF_SIZE   (512) 
#define ADD_IFC_DEBUG 		(1)
#define MAX_SOCK_NUM 		(8)
#define TX_RX_MAX_BUF_SIZE 	(2048)


#define tick_second 1

///////////////////////////////////////////////////////////////////
// function define
///////////////////////////////////////////////////////////////////
void quake_warning_lcd();
void quake_status_lcd();
void fire_status_lcd();
void fire_warning_lcd();
void delArrCheck(int *arr, int floor, int num);
void print_delArr(int *arr);
void pop_delArr(int *arr, int num);
void notice_lcd();
void client_call_lcd();
void office_status_lcd();
void print_del_lcd();
void led_blink(int time);
void lcd_refresh(int num);
void client_call(int time);
void send_connected_status(char c);

extern void buzzer_beep_stop();

///////////////////////////////////////////////////////////////////
/* Private macro -------------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void Reset_W5500(void);
void W5500_CS(unsigned char val);

void W5500_IRQ_Init(void);

void Ethernet_Config(void);

void Ethernet_Init(void);
unsigned char SPI2_SendByte(unsigned char byte);

unsigned int time_return(void);
void Stringto_Ipaddr(unsigned char*ip, char*str_ip);
/* Private variables ---------------------------------------------------------*/
unsigned char ETH_Data_buff[ ETH_DATA_BUF_SIZE ];

unsigned char TX_BUF[TX_RX_MAX_BUF_SIZE]; // TX Buffer for applications
unsigned char RX_BUF[TX_RX_MAX_BUF_SIZE]; // RX Buffer for applications

unsigned char ch_status[MAX_SOCK_NUM] = { 0, }; /** 0:close, 1:ready, 2:connected */

unsigned short any_port = 1000;

unsigned int presentTime;

unsigned int my_time;






///////////////////////////////////
__IO uint32_t Timer2_Counter;

///////////////////////////////////
// Default Network Configuration //
///////////////////////////////////
wiz_NetInfo gWIZNETINFO = 
{ 
	.mac = {0x00, 0x08, 0xdc,0x1d, 0x4c, 0x53},
	.ip = {192, 168, 0, 100},
	.sn = {255,255,255,0},
	.gw = {192, 168, 0, 1},
	.dns = {0,0,0,0},
	.dhcp = NETINFO_STATIC
};

wiz_NetTimeout Net_timout={0x02,0x7D0};

uint8_t Dest_IP[4] = {192, 168, 0, 200}; //DST_IP Address
uint16_t Dest_PORT = 6000;

void Ethernet_Init(void)
{
	SPI_InitTypeDef SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// clock configure SPI2
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE);	
	
	
	/* Configure SPI2 pins: SCK, MISO and MOSI ---------------------------------*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// W_CSn, W_RST
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_4 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable , ENABLE);
	
	W5500_CS(CS_HIGH);
	
	/* SPI2 Config -------------------------------------------------------------*/
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	//	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_16;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI2, &SPI_InitStructure);
	
	/* Enable SPI2 */
	SPI_Cmd(SPI2, ENABLE);
	
	Reset_W5500(); // reset
	
	TIM_Delay_ms(1000);
	
	Ethernet_Config(); // configure
}


// Connected to Data Flash
void W5500_CS(unsigned char val)
{
	if (val == CS_LOW) 
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_9);
		GPIO_ResetBits(GPIOB, GPIO_Pin_12);		
	}
	else if (val == CS_HIGH)
	{
		GPIO_SetBits(GPIOB, GPIO_Pin_9);
		GPIO_SetBits(GPIOB, GPIO_Pin_12);
	}
}


void Reset_W5500(void)
{
	GPIO_ResetBits(GPIOB, GPIO_Pin_4);
	//	Delay_us(2);
	TIM_Delay_ms(400);	
	GPIO_SetBits(GPIOB, GPIO_Pin_4);
	//	Delay_ms(150);  
	TIM_Delay_ms(400);
}

char W5500_Get_Rdy(void)
{
	//	GPIO_ResetBits(GPIOB, GPIO_Pin_5);
	return GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_7);
}


void Ethernet_Config(void)
{
	
	unsigned char tmpstr[6];
	
	ctlnetwork(CN_SET_NETINFO, (void*)&gWIZNETINFO);
	ctlnetwork(CN_GET_NETINFO, (void*)&gWIZNETINFO);
	
	ctlnetwork(CN_SET_TIMEOUT, (void*)&Net_timout);
	ctlnetwork(CN_GET_TIMEOUT, (void*)&Net_timout);
	
	// Display Network Information
	ctlwizchip(CW_GET_ID,(void*)tmpstr);
	Uart1_Printf("=== %s NET CONF ===\n",(char*)tmpstr);
	Uart1_Printf("MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",gWIZNETINFO.mac[0],
							 gWIZNETINFO.mac[1],gWIZNETINFO.mac[2],gWIZNETINFO.mac[3],
							 gWIZNETINFO.mac[4],gWIZNETINFO.mac[5]);
	
	Uart1_Printf("Version :%X\n", getVERSIONR());
	Uart1_Printf("======================\n");
	
}



void W5500_IRQ_Init(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef EXTI_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0xF;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* Configure Key Button GPIO Pin as input floating (Key Button EXTI Line) */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* Connect Key Button EXTI Line to Key Button GPIO Pin */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource5);
	
	/* Configure Key Button EXTI Line to generate an interrupt on falling edge */  
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);	
}


unsigned char SPI2_SendByte(unsigned char byte)
{
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) == RESET){}
	SPI_I2S_SendData(SPI2, byte);
	while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == RESET){}
	return (unsigned char)SPI_I2S_ReceiveData(SPI2);
}

extern volatile int Uart1_Rx_In;
extern volatile int Uart1_Rx_Data;

unsigned short RSR_len;
unsigned short received_len;
unsigned char * txbuf = TX_BUF;
unsigned char * rxbuf = RX_BUF;
int index=0;

///////////////////////////////////
/// Custom Value //////////////////
///////////////////////////////////
#define BASE  (500) //msec
#define DELARR_MAX 8	//  최대 택배함
#define DEL_X 8
#define DEL_Y 2	
#define OFFICE_MAX  8
#define BLACK	0x0000
#define WHITE	0xffff
#define BLUE	0x001f
#define GREEN	0x07E0
#define RED		0xf800

enum key{C1, C1_, D1, D1_, E1, F1, F1_, G1, G1_, A1, A1_, B1, C2, C2_, D2, D2_, E2, F2, F2_, G2, G2_, A2, A2_, B2};
enum note{N16=BASE/4, N8=BASE/2, N4=BASE, N2=BASE*2, N1=BASE*4};
extern const char * note_name[];



enum LCD_STATUS {MAIN, DELIVERY, FIRE_WARNING};
static int delArr[DELARR_MAX]={0,};
static int officeArr[OFFICE_MAX]={1002,2904,3207,1234,5678,7890,9876 ,8765};
int door_status_arr[OFFICE_MAX] = {0, };
int fire_status_arr[OFFICE_MAX] = {0, };
int quake_status_arr[OFFICE_MAX] = {0, };
static char door_status[2] = {'X','O'};
extern int led_cnt;
extern int lcd_mod;
extern int buzzer_tone;
int led_cnt_value;
int lcd_status=-1;		// 현재 LCD의 상태 정보 
int lcdArr[5];
int client_cnt=0;
int fIndex=0;
int nIndex=0;
int quake_cnt=0;
int quake_stat_flag=0;

volatile unsigned int call_flag=0;
volatile unsigned int fire_flag=0;
volatile unsigned int quake_flag=0;
char CLIENT_SOCKET;
unsigned char noticebuf[1500];
unsigned char clientID[1500];
unsigned char ID_buf[1500];
//////////////////////////////////
unsigned char ret;

/////////////////////////////////////////////////////
/// Costom Fucntion /////////////////////////////////
/////////////////////////////////////////////////////

void quake_check(char *c){
	int i;
	int num=atoi(c+2);
	int v = c[1]-'0';
	for(i=0;i<OFFICE_MAX;i++){
		if(officeArr[i]==num){
			if(v==1 && quake_status_arr[i]==0){
				quake_status_arr[i]=1;
				quake_cnt++;
			}
			else if(v==0 && quake_status_arr[i]==1){
				quake_status_arr[i]=0;
				quake_cnt--;
			}
		}
	}
	
//	for(i=0;i<OFFICE_MAX;i++){
//		if(quake_status_arr[i]==1)
//			cnt++;
//	}
	
}

void quake_warning_lcd(){
	Lcd_Clr_Screen(RED);
	Lcd_Printf(30,30, WHITE,BLACK,1,1,"%s", " EARTHQUAKE ");
	Lcd_Printf(0,65, WHITE,BLACK,2,2,"%s", "!WARNING!");
}

void quake_status_lcd(){
	int i;
	int num=atoi(ID_buf+1);
	int xArr[8]={2,42,82,122,2,42,82,122};
	int yArr[8]={25,25,25,25,85,85,85,85};
	
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(0,0, BLACK,WHITE,1,1,"%s", "Earthquake Status");
	Lcd_Draw_Hline(15,0,160,BLACK);
	Lcd_Draw_Hline(74,0,160,BLACK);
	Lcd_Draw_Vline(40,15,128,BLACK);
	Lcd_Draw_Vline(80,15,128,BLACK);
	Lcd_Draw_Vline(120,15,128,BLACK);
	
	// 사무실 번호
	Lcd_Printf(2,25, BLACK,WHITE,1,1,"%d", officeArr[0]);
	Lcd_Printf(42,25, BLACK,WHITE,1,1,"%d", officeArr[1]);
	Lcd_Printf(82,25, BLACK,WHITE,1,1,"%d", officeArr[2]);
	Lcd_Printf(122,25, BLACK,WHITE,1,1,"%d", officeArr[3]);
	Lcd_Printf(2,85, BLACK,WHITE,1,1,"%d", officeArr[4]);
	Lcd_Printf(42,85, BLACK,WHITE,1,1,"%d", officeArr[5]);
	Lcd_Printf(82,85, BLACK,WHITE,1,1,"%d", officeArr[6]);
	Lcd_Printf(122,85, BLACK,WHITE,1,1,"%d", officeArr[7]);
	
	for(i=0;i<OFFICE_MAX;i++){
		if(quake_status_arr[i]==1){
			Lcd_Printf(xArr[i],yArr[i], WHITE,BLACK,1,1,"%d", officeArr[i]);
		}
	}
}


void fire_warning_lcd(){
	Lcd_Clr_Screen(RED);
	Lcd_Printf(10,20, WHITE,RED,2,2,"%s", " FIRE  ");
	Lcd_Printf(0,65, WHITE,RED,2,2,"%s", "!WARNING!");
}




void fire_status_lcd(){
	int i;
	int num=atoi(ID_buf+1);
	int xArr[8]={2,42,82,122,2,42,82,122};
	int yArr[8]={25,25,25,25,85,85,85,85};
	
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(0,0, BLACK,WHITE,1,1,"%s", "Fire Status");
	Lcd_Draw_Hline(15,0,160,BLACK);
	Lcd_Draw_Hline(74,0,160,BLACK);
	Lcd_Draw_Vline(40,15,128,BLACK);
	Lcd_Draw_Vline(80,15,128,BLACK);
	Lcd_Draw_Vline(120,15,128,BLACK);
	
	// 사무실 번호
	Lcd_Printf(2,25, BLACK,WHITE,1,1,"%d", officeArr[0]);
	Lcd_Printf(42,25, BLACK,WHITE,1,1,"%d", officeArr[1]);
	Lcd_Printf(82,25, BLACK,WHITE,1,1,"%d", officeArr[2]);
	Lcd_Printf(122,25, BLACK,WHITE,1,1,"%d", officeArr[3]);
	Lcd_Printf(2,85, BLACK,WHITE,1,1,"%d", officeArr[4]);
	Lcd_Printf(42,85, BLACK,WHITE,1,1,"%d", officeArr[5]);
	Lcd_Printf(82,85, BLACK,WHITE,1,1,"%d", officeArr[6]);
	Lcd_Printf(122,85, BLACK,WHITE,1,1,"%d", officeArr[7]);
	
	for(i=0;i<OFFICE_MAX;i++){
		if(fire_status_arr[i]==1){
			Lcd_Printf(xArr[i],yArr[i], WHITE,RED,1,1,"%d", officeArr[i]);
		}
	}
}

int checkOffice(char * c){
	int i;
	int num = atoi(c+1);
	for(i=0; i<OFFICE_MAX;i++){
		if(officeArr[i]==num)
		return 0;
	}
	return 1;
}

void delArrCheck(int *arr, int floor, int num){
	int i;
	for(i=0;i<DELARR_MAX;i++){
		if(arr[i]==0){
			arr[i]= floor*100 + num;
			return;
		}
	}
}

void print_delArr(int *arr){
	int i;
	for(i=0; i<DELARR_MAX;i++){
		if(!(i%8))
			Uart1_Printf("\n");
		Uart1_Printf("[%d]", arr[i]);
	}
}

void pop_delArr(int *arr, int num){
	int i;
	int cnt=0;
	for(i=0; i<DELARR_MAX;i++){
		if(arr[i]==num){
			arr[i]=0;
			cnt++;
		}
	}
	
	if(cnt==0)
		Uart1_Printf("\n%d층 %d층의 택배가 없습니다.", num/100, num%100);
	else
		Uart1_Printf("%\n%d층 %d호 택배 전달 완료.", num/100, num%100) ;
}

void notice_lcd(){
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(0,0, BLACK,WHITE,2,2,"%s", "  NOTICE  ");
	Lcd_Printf(20,74, BLACK, WHITE, 1, 1, "%s", noticebuf+1);
	if(lcd_status!=1)
		lcd_status=1;
}

void client_call_lcd(){
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(0,25, BLACK,WHITE,2,2,"%s", "CONNECTING.. ");
	Lcd_Printf(30,70, BLACK, WHITE, 2, 2, "%s", clientID+1);
}

void office_status_lcd(){
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(0,0, BLACK,WHITE,1,1,"%s", "Office Status");
	Lcd_Draw_Hline(15,0,160,BLACK);
	Lcd_Draw_Hline(74,0,160,BLACK);
	Lcd_Draw_Vline(40,15,128,BLACK);
	Lcd_Draw_Vline(80,15,128,BLACK);
	Lcd_Draw_Vline(120,15,128,BLACK);
	
	// 사무실 번호
	Lcd_Printf(2,25, BLACK,WHITE,1,1,"%d", officeArr[0]);
	Lcd_Printf(42,25, BLACK,WHITE,1,1,"%d", officeArr[1]);
	Lcd_Printf(82,25, BLACK,WHITE,1,1,"%d", officeArr[2]);
	Lcd_Printf(122,25, BLACK,WHITE,1,1,"%d", officeArr[3]);
	Lcd_Printf(2,85, BLACK,WHITE,1,1,"%d", officeArr[4]);
	Lcd_Printf(42,85, BLACK,WHITE,1,1,"%d", officeArr[5]);
	Lcd_Printf(82,85, BLACK,WHITE,1,1,"%d", officeArr[6]);
	Lcd_Printf(122,85, BLACK,WHITE,1,1,"%d", officeArr[7]);
	
	// 사무실 문 잠김 여부
	Lcd_Printf(2,43, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[0]]);
	Lcd_Printf(42,43, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[1]]);
	Lcd_Printf(82,43, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[2]]);
	Lcd_Printf(122,43, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[3]]);
	Lcd_Printf(2,103, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[4]]);
	Lcd_Printf(42,103, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[5]]);
	Lcd_Printf(82,103, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[6]]);
	Lcd_Printf(122,103, BLACK,WHITE,1,1,"%c", door_status[door_status_arr[7]]);
}

void print_del_lcd(){			// LCD 표현
	Lcd_Clr_Screen(WHITE);
	Lcd_Printf(0,0, BLACK,WHITE,1,1,"%s", "Delivery Status");
	Lcd_Draw_Hline(15,0,160,BLACK);
	Lcd_Draw_Hline(74,0,160,BLACK);
	Lcd_Draw_Vline(40,15,128,BLACK);
	Lcd_Draw_Vline(80,15,128,BLACK);
	Lcd_Draw_Vline(120,15,128,BLACK);
	
	Lcd_Printf(5,40, BLACK,WHITE,1,1,"%d", delArr[0]);
	Lcd_Printf(45,40, BLACK,WHITE,1,1,"%d", delArr[1]);
	Lcd_Printf(85,40, BLACK,WHITE,1,1,"%d", delArr[2]);
	Lcd_Printf(125,40, BLACK,WHITE,1,1,"%d", delArr[3]);
	Lcd_Printf(5,100, BLACK,WHITE,1,1,"%d", delArr[4]);
	Lcd_Printf(45,100, BLACK,WHITE,1,1,"%d", delArr[5]);
	Lcd_Printf(85,100, BLACK,WHITE,1,1,"%d", delArr[6]);
	Lcd_Printf(125,100, BLACK,WHITE,1,1,"%d", delArr[7]);
}


void led_blink(int time){
	led_cnt_value=time;
	TIM4_Repeat_Interrupt_Enable(1, 300);
}

void lcd_refresh(int num){
	switch(num){

	case -1:						// WELCOME
		Lcd_Clr_Screen(WHITE);
	Lcd_Printf(15,25, BLACK,WHITE,2,2,"%s", "SERVER");
	Lcd_Printf(40,60, BLACK,WHITE,1,1,"%s", "START");
		break;
		
	case 0: 						// 택배
		print_del_lcd();
		break;
		
	case 1:							// 공지사항
		notice_lcd();
		break;
		
	case 2:							// 사무실 문 잠김 현황
		office_status_lcd();
		break;
	case 3:
		quake_status_lcd();
		break;		
	case 4:
		quake_warning_lcd();
		break;		
	case 5:
		client_call_lcd();		
		break;
	case 6:					
		Lcd_Clr_Screen(WHITE);
		Lcd_Printf(5,70, BLACK, WHITE, 2, 2, "%s", "CONNECTED...");
		break;
	case 7:
		fire_warning_lcd();
		break;
	case 8:
		fire_status_lcd(); // 화재 위치 정보 
		break;
	}
}

void client_call(int time){
	lcd_status=5;
	lcd_refresh(lcd_status);
	led_blink(time);
	TIM2_Interrupt_Enable(1, 500);
	buzzer_tone=C1;
}

void send_buzzer_signal(){
	int i;
	char send_id[10];
	strcpy(send_id, clientID);
	send_id[0]=')';
	for(i=0;i<client_cnt;i++){
		send(i, send_id, 10);
	}
}


///////////////////////////////////////////////////////////////////////////
void loopback_tcps(char s, unsigned short port)
{
	switch (getSn_SR(s))
	{
	case SOCK_ESTABLISHED:              /* if connection is established */
		if(ch_status[s]==1)
		{
			client_cnt++;
			Uart1_Printf("\r\n%d : Connected",s);
			Uart1_Printf("\r\n Peer IP : %d.%d.%d.%d", IINCHIP_READ(Sn_DIPR0(s)),  IINCHIP_READ(Sn_DIPR1(s)), IINCHIP_READ(Sn_DIPR2(s)), IINCHIP_READ(Sn_DIPR3(s)) );
			Uart1_Printf("\r\n Peer Port : %d", ( (unsigned short)(IINCHIP_READ(Sn_DPORT0(s)))<<8) +(unsigned short)IINCHIP_READ( Sn_DPORT1(s)) ) ;
			Uart1_Printf("\r\n[send]: ");
			ch_status[s] = 2;
		}

		// uart 입력을 받으면 
		if(Uart1_Rx_In)
		{
			Uart1_Rx_In = 0;
			// 문자 입력
			if(Uart1_Rx_Data!='\r')
			{
				txbuf[index++]=Uart1_Rx_Data;
				Uart1_Printf("%c",Uart1_Rx_Data);
			}
			// 입력 완료 후 엔터 칠 경우
			else
			{
				int i;
				txbuf[index]='\0';
				// 입력 데이터에 따른 메세지 구분
				
				if(txbuf[0]=='@'){	// @ : 택배 입력
					fIndex=(((int)txbuf[1]-'0')*10)+((int)txbuf[2]-'0');
					nIndex=(((int)txbuf[3]-'0')*10)+((int)txbuf[4]-'0');
					Uart1_Printf("\n%d층 %d호 택배 등록 완료 ",fIndex, nIndex);
					delArrCheck(delArr, fIndex, nIndex);
					for(i=0;i<client_cnt;i++)
						send(i, txbuf, index);
					if(lcd_status==0)
						print_del_lcd();
					Uart1_Printf("\n[send]: ");
					index=0;
				}
				else if(txbuf[0]=='%'){	// 택배 검색
					print_delArr(delArr);
					Uart1_Printf("\n[send]: ");
					index=0;
				}
				else if(txbuf[0]=='^'){	// 택배 전달 (보관함에서 뺌)
					fIndex=(((int)txbuf[1]-'0')*10)+((int)txbuf[2]-'0');
					nIndex=(((int)txbuf[3]-'0')*10)+((int)txbuf[4]-'0');
					pop_delArr(delArr, fIndex*100 + nIndex);
					for(i=0;i<client_cnt;i++)
						send(i, txbuf, index);
					if(lcd_status==0)
						print_del_lcd();
					Uart1_Printf("\n[send]: ");
					index=0;
				}
				
				else if(txbuf[0]=='$'){			// 서버->사무실 호수 호출
					int check=checkOffice(txbuf);
					if(check){
						return;
					}
					lcd_mod=2;
					strcpy(clientID, txbuf);
					client_call(0);

					for(i=0;i<client_cnt;i++)
						send(i, txbuf, index);
					Uart1_Printf("\n[send]: ");
					index=0;
				}
				
				else if(txbuf[0]=='#'){			// 공지사항 전달
					for(i=0;i<client_cnt;i++)
						send(i, txbuf, index);
					//led_blink(6);
					for(i=0; i<index;i++){
						noticebuf[i]=txbuf[i];
					} 
					strcpy(noticebuf, txbuf);
					notice_lcd();

					Uart1_Printf("\n[send]: ");
					index=0;
				}
								
				
				else if(txbuf[0]=='*'){			// 사무실 문 잠금 작업 수행
					int num;
					for(i=0;i<client_cnt;i++){
						send(i, txbuf, index);
					}
					num = atoi(txbuf+2);
					if(txbuf[1]=='0'){
						for(i=0; i<OFFICE_MAX;i++){
							if(officeArr[i]==num){
								door_status_arr[i]=0;
							}
						}
					}
					
					else if(txbuf[1]=='1'){
						for(i=0; i<OFFICE_MAX;i++){
							if(officeArr[i]==num){
								door_status_arr[i]=1;
							}
						}						
					}
					lcd_status=2;
					lcd_refresh(lcd_status);
					index=0;
				}
				
				
				else{
					for(i=0;i<client_cnt;i++)
						send(i, txbuf, index);
					Uart1_Printf("\n[send]: ");
					index=0;
				}
			}
		}
		
		
		if ((RSR_len = getSn_RX_RSR(s)) > 0)        /* check Rx data */
		{
			if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */                                                                           /* the data size to read is MAX_BUF_SIZE. */
			received_len = recv(s, rxbuf, RSR_len);      /* read the received data */
			rxbuf[RSR_len]='\0';	
			Uart1_Printf("\r[recv]: %s\n",rxbuf);
			Uart1_Printf("[send]: ");
			
			if(rxbuf[0]=='#'){	// 사무실->서버 호출
				strcpy(clientID, rxbuf);
				lcd_mod=1;
				client_call(1000);
			}
			
			else	if(rxbuf[0]=='!'){	// 사무실->서버 통화 연결
				led_cnt==led_cnt_value;
				lcd_mod=2;
				buzzer_beep_stop();
				Lcd_Clr_Screen(WHITE);
				Lcd_Printf(5,70, BLACK, WHITE, 2, 2, "%s", "CONNECTED...");
			}
			
			else	if(rxbuf[0]=='?'){	// 사무실->서버 통화 종료 (클라에서 끊음)
				lcd_mod=0;
				buzzer_beep_stop();
				Lcd_Clr_Screen(WHITE);
				Lcd_Printf(5,70, BLACK, WHITE, 1, 1, "%s", "DISCONNECTED...");
			}
			
			else	if(rxbuf[0]=='*'){	// 사무실 문 잠김 체크 
				int i,num;
				if(rxbuf[1]=='0'){
					num = atoi(rxbuf+2);
					for(i=0; i<OFFICE_MAX;i++){
						if(officeArr[i]==num){
							door_status_arr[i]=0;
						}
					}		
				}
				lcd_status=2;
				lcd_refresh(lcd_status);
			}
			else	if(rxbuf[0]=='&'){	// 화재 경보 
				int i;
				TIM2_Interrupt_Enable(1, 150);
				led_blink(1000);
				buzzer_tone=G2;
				strcpy(ID_buf, rxbuf);
				lcd_mod=3;
				lcd_status=7;
				lcd_refresh(lcd_status);
				for(i=0;i<OFFICE_MAX;i++){
					if(officeArr[i]==atoi(rxbuf+1)){
						fire_status_arr[i]=1;
					}
				}
				for(i=0;i<client_cnt;i++)
					send(i, rxbuf, 5);

			}
			
			else	if(rxbuf[0]=='~'){	// 지진 경보 
				strcpy(ID_buf, rxbuf);
				quake_check(ID_buf);
				lcd_refresh(lcd_status);
			}
			rxbuf[0]='0';
		}
		
		if(quake_stat_flag==0 && quake_cnt>1){
			int i;
			TIM2_Interrupt_Enable(1, 350);
			led_blink(1000);
			buzzer_tone=G1_;
			lcd_mod=4;
			lcd_status=4;
			lcd_refresh(lcd_status);
			for(i=0;i<client_cnt;i++)
				send(i, "~", 5);
			quake_stat_flag=1;
		}
		
		// 1 : 받는 경우
		if(call_flag==1){
			int i;
			led_cnt=1000;
			strcpy(ID_buf, "!\0");
			strcat(ID_buf, clientID+1);
			for(i=0;i<client_cnt;i++)
				send(i, ID_buf, 10);
			call_flag=0;
		}
		
		// 2 : 끊는 경우
		else if(call_flag==2){
			int i;
			led_cnt=1000;
			strcpy(ID_buf, "?\0");
			strcat(ID_buf, clientID+1);
			for(i=0;i<client_cnt;i++)
				send(i, ID_buf, 10);
			call_flag=0;			
		}
		
		// 화재 경보 끄는 경우
		if(fire_flag==1){
			int i;
			buzzer_beep_stop();
			led_cnt=1000;
			for(i=0;i<client_cnt;i++)
				send(i, "(", 2);
			fire_flag=0;
			for(i=0; i<OFFICE_MAX;i++)
				fire_status_arr[i]=0;
		}
		
		// 지진 경보 끄는 경우 
		if(quake_flag==1){
			int i;
			buzzer_beep_stop();
			led_cnt=1000;
			for(i=0;i<client_cnt;i++)
				send(i, "(", 2);
			quake_flag=0;
			for(i=0; i<OFFICE_MAX;i++)
				quake_status_arr[i]=0;
			quake_stat_flag=0;
			quake_cnt=0;
		}
		
		break;
	case SOCK_CLOSE_WAIT:                              /* If the client request to close */
		Uart1_Printf("\r\n%d : CLOSE_WAIT", s);
		if ((RSR_len = getSn_RX_RSR(s)) > 0)     /* check Rx data */
		{
			if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;  /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */                                                                     /* the data size to read is MAX_BUF_SIZE. */
			received_len = recv(s, rxbuf, RSR_len);     /* read the received data */
		}
		disconnect(s);		
		ch_status[s] = 0;
		break;
	case SOCK_CLOSED:                                       /* if a socket is closed */
		if(!ch_status[s])
		{
			Uart1_Printf("\r\n%d : Loop-Back TCP Server Started. port : %d", s, port);
			ch_status[s] = 1;
		}

		Uart1_Printf("\r\n%d : CLOSE_0001\n", s);		
		ret  = socket(s,( Sn_MR_TCP ),port,0x00);
		
		if(ret != s)    /* reinitialize the socket */
		{
			Uart1_Printf("\r\n%d : Fail to create socket.",s);
			ch_status[s] = 0;
		}
		break;
	case SOCK_INIT:   /* if a socket is initiated */
		
		listen(s);
		Uart1_Printf("\r\n%x :LISTEN socket %d ",getSn_SR(s), s);
		break;
	default:
		break;
	}
}


void Stringto_Ipaddr(unsigned char*ip, char*str_ip)
{
	int i;
	int str_offset=0;
	unsigned char temp1;
	unsigned char temp2=0;
	unsigned char digit=0;

	for(i=0;i<4;)
	{
		temp1 = *(str_ip + str_offset);		
		if( temp1 == '\0')
		{
			ip[i] = temp2;
			break;
		}
		else if( temp1 != '.')
		{
			if (digit == 0)
			{
				temp2 = temp1-'0';
				digit++;
			}
			else if(digit == 1)
			{
				temp2 *= 10;
				temp2 += (temp1-'0');
				digit++;
			}
			else
			{
				temp2 *= 10;
				temp2 += (temp1-'0');
			}
			str_offset++;
		}
		else
		{
			ip[i++] = temp2;
			
			temp2 = 0;
			digit = 0;
			str_offset++;
		}
	
	}


}
void loopback_udp(char s, unsigned short port)
{
	unsigned short RSR_len;
	unsigned char * txbuf = TX_BUF;
  unsigned char * rxbuf = RX_BUF;
	unsigned int destip = 0;
	unsigned short destport;
	
	switch (getSn_SR(s))
	{
	case SOCK_UDP:
		if ((RSR_len = getSn_RX_RSR(s)) > 0)         /* check Rx data */
		{
			if (RSR_len > TX_RX_MAX_BUF_SIZE) RSR_len = TX_RX_MAX_BUF_SIZE;   /* if Rx data size is lager than TX_RX_MAX_BUF_SIZE */
			
			/* the data size to read is MAX_BUF_SIZE. */
			received_len = recvfrom(s, rxbuf, RSR_len, (unsigned char*)&destip, &destport);       /* read the received data */
			rxbuf[RSR_len]='\0';
			Uart1_Printf("\r[recv]: %s\n",rxbuf);
			Uart1_Printf("[send]: ");
			Uart1_Printf("destip %x, destport %d \n", destip, destport);
			
			//if(sendto(s, data_buf, received_len,(unsigned char*)&destip, destport) == 0) /* send the received data */
			//{
			//	Uart1_Printf("\a\a\a%d : System Fatal Error.", s);
			//}
			
		}
		if(Uart1_Rx_In)
		{
			Uart1_Rx_In = 0;
			
			if(Uart1_Rx_Data!='\r')
			{
				txbuf[index++]=Uart1_Rx_Data;
				Uart1_Printf("%c",Uart1_Rx_Data);
			}
			else
			{
				txbuf[index]='\0';
				if(sendto(s, txbuf, index,(unsigned char*)Dest_IP, Dest_PORT) == 0) /* send the received data */
				{
					Uart1_Printf("\a\a\a%d : System Fatal Error.", s);
				}
				Uart1_Printf("\n[send]: ");
				index=0;
			}
		}
		break;
	case SOCK_CLOSED:                                               /* if a socket is closed */
		Uart1_Printf("\r\n%d : Loop-Back UDP Started. port :%d", s, port);
		if(socket(s,( Sn_MR_UDP ),port,0x00)== 0)    /* reinitialize the socket */
			Uart1_Printf("\a%d : Fail to create socket.",s);
		break;
	}
}


/*******************************************************************************
* Function Name  : time_return
* Description    :
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

unsigned int time_return(void)
{
//    extern unsigned int my_time;
    return my_time;
}

void Timer2_ISR(void)
{
  if (Timer2_Counter++ > 1000) { // 1m x 1000 = 1sec
    Timer2_Counter = 0;
    my_time++;
  }
}


/*
	 ===============================================================================
Name        : Taximetro.c
Author      : Grupo 1
Description : Un taximetro hecho para el integrador de Digitales 3
===============================================================================
*/

#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_exti.h"
#include "lpc17xx_systick.h"


/*---------------------------------------------------Configs----------------------------------------------------------------------------------------*/
void confGPIO(void); // Prototipo de la funcion de conf. de puertos
void configADC(void);
void config_timer(void);
void confExtInt(void); //Prototipo de la funcion de conf. de interrupciones externas
void change_state(void);
void config_timer_1(void);
void confUART(void);
void confDMA(void);
/*---------------------------------------------------main----------------------------------------------------------------------------------------*/
void rutina_1(void); // poner un nombre mas representativo
void rutina_2(void);
void rutina_3(void);
void bucle(void);
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Utilidades----------------------------------------------------------------------------------------*/
void actualizar_mensaje(void);
uint8_t get_TeclaMatrical(void);
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------DEFINES----------------------------------------------------------------------------------------*/
#define De_uS_mS(num) (num*100000)
#define De_uS_S(num) (num*1000000)

#define OUTPUT 1
#define INPUT 0
#define IN_TECLADO 0XF
#define OUT_TECLADO (0XF << 4)

#define CHANNEL_0 0
#define LEDS_RED (1 << 4)
#define LEDS_GREEN (1 << 5)
#define BUZZER (1 << 6)

#define TIMEMUESTREO 1
#define F_MUESTREO 200000
#define CLKPWR_PCLKSEL_CCLK_DIV_8 3

#define EDGE_RISING 0
#define LENGTH 16

#define VELOCIDAD_MAX 40
#define RESOLUCION 4096
/* Estados del taximetro*/
#define LIBRE 1
#define OCUPADO 2
#define STOP  3

#define DMA_SIZE 7

#define VALOR_FICHA 9
#define DISTANCIA_FICHA 200
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------Variables Globales-------------------------------------------------------------------------*/
GPDMA_LLI_Type DMA_LLI_Struct;

uint8_t static anti_rebote=0;
uint8_t static LED_ON_OFF=1;
uint8_t static modo = LIBRE;
uint16_t static tarifa = 0;
uint16_t static distancia = 0;
uint8_t static car_state = 0;
volatile uint16_t ADC0Value = 0;

uint8_t const teclado_matricial[LENGTH] = {	1,2,3,0xA, \
	4,5,6,0XB, \
		7,8,9,0xC, \
		0XE,0,0XE,0XD
};
uint8_t mensaje[10] = {"\rM   $0000"};

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------Programa-------------------------------------------------------------------------*/
int main(void)
{
	confGPIO();
	confExtInt();
	configADC();
	config_timer();
	config_timer_1();
	confUART();
	confDMA();

	bucle();

	return 0;
}
void bucle(void)
{
	while(1)
	{
		switch (modo)
		{
			case LIBRE:
				rutina_1(); //cambiar nombre de estas funciones
				break;

			case OCUPADO:
				rutina_2();
				break;

			case STOP:
				rutina_3();
				break;
		}

	}
}

/* Pasa del estado ocupado o en stop al estado libre */
void rutina_1(void)
{
	change_state();
	TIM_Cmd(LPC_TIM0, DISABLE);
	TIM_Cmd(LPC_TIM1, DISABLE);

	tarifa = 0;
	distancia=0;

	actualizar_mensaje();
	while (modo == LIBRE)
	{
	}
	return;
}

/* Pasa del estado ocupado o en stop al estado libre */
void rutina_2(void)
{
	change_state();
	TIM_ResetCounter(LPC_TIM0);
	TIM_Cmd(LPC_TIM0, ENABLE);

	actualizar_mensaje();
	while (modo == OCUPADO)
	{
		if (distancia > DISTANCIA_FICHA)
		{
			tarifa += VALOR_FICHA;
			distancia = 0;
			actualizar_mensaje();
		}
	}
	return;
}

void rutina_3(void)
{
	TIM_Cmd(LPC_TIM0, DISABLE);
	TIM_Cmd(LPC_TIM1, DISABLE);

	actualizar_mensaje();
	while (modo == STOP)
	{

	}
	return;
}

/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Configs----------------------------------------------------------------------------------------*/

void confGPIO(void)
{
	uint8_t i;
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum = PINSEL_FUNC_0;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLDOWN;
	PinCfg.Portnum = PINSEL_PORT_2;

	/* entradas */

	for (i = 0; i< 4; i++)
	{
		PinCfg.Pinnum = i;
		PINSEL_ConfigPin(&PinCfg); //configura los pines p2.0 a p2.3 como gpio, con pull-down y modo normal
	}
	GPIO_SetDir(PINSEL_PORT_2, IN_TECLADO, INPUT); // configura los pines p2.0 a p2.3 como entradas

	/* salidas */

	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	for (i = 4; i< 8; i++)
	{
		PinCfg.Pinnum = i;
		PINSEL_ConfigPin(&PinCfg); //configura los pines p2.4 a p2.7 como gpio, con pull-up y modo normal
	}

	GPIO_SetDir(PINSEL_PORT_2, OUT_TECLADO, OUTPUT); // configura los pines p2.4 a p2.7 como salids

	/* LEDS */
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum = PINSEL_PORT_0;
	PinCfg.Pinnum = 4;

	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Pinnum = 5;
	PINSEL_ConfigPin(&PinCfg);

	PinCfg.Pinnum = 6;
	PINSEL_ConfigPin(&PinCfg);

	GPIO_SetDir(PINSEL_PORT_0, (LEDS_RED | LEDS_GREEN | BUZZER), OUTPUT);
	FIO_ByteSetValue(2, 0, OUT_TECLADO);

	GPIO_ClearValue(PINSEL_PORT_0, (BUZZER));

	return;
}

void configADC(void)
{
	PINSEL_CFG_Type conf_pin;
	conf_pin.Funcnum = PINSEL_FUNC_1;            //PIN EN MODO AD0.0
	conf_pin.Portnum = PINSEL_PORT_0;            //PUERTO 0
	conf_pin.Pinnum = 23;                        //PIN 23
	conf_pin.Pinmode = PINSEL_PINMODE_TRISTATE;    //
	PINSEL_ConfigPin(&conf_pin);

	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_ADC, CLKPWR_PCLKSEL_CCLK_DIV_8);
	///CONFIGURACION ADC///

	ADC_Init(LPC_ADC, F_MUESTREO); //ENCIENDO ADC
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0,ENABLE); //POR CANAL 0
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);    //START CON MATCH, DEBE MUESTREAR CADA 1 SEG
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_RISING);
	ADC_BurstCmd(LPC_ADC, DISABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADGINTEN, SET);

	ADC_GlobalGetStatus(LPC_ADC, 1);
	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

void config_timer(void)
{
	TIM_TIMERCFG_Type    struct_config;
	TIM_MATCHCFG_Type    struct_match;

	struct_config.PrescaleOption    =    TIM_PRESCALE_USVAL;
	struct_config.PrescaleValue     =    De_uS_mS(1);

	struct_match.MatchChannel       =    1;
	struct_match.IntOnMatch         =    DISABLE; //deshabilitamos las interrupciones por timer
	struct_match.ResetOnMatch       =    ENABLE;//resetea el contador del timer cuando se produce un match
	struct_match.StopOnMatch        =    DISABLE; //no detiene el contador del timer cuando se produce un match
	struct_match.ExtMatchOutputType =    TIM_EXTMATCH_TOGGLE;
	struct_match.MatchValue         =    5;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &struct_config); //se prende el timer0, se configura la division del clock del periferico, y se
	//configura el timer como modo temporizador y ademas se retesea el contador y se lo saca del reset
	TIM_ConfigMatch(LPC_TIM0, &struct_match); //carga todas las configuraciones del struct_match en ls registros correspondientes

	TIM_ResetCounter(LPC_TIM0);
	TIM_Cmd(LPC_TIM0, DISABLE); //habilita el contador del timer y prescaler

	return;
}

void config_timer_1(void)
{
	TIM_TIMERCFG_Type    struct_config;
	TIM_MATCHCFG_Type    struct_match;

	struct_config.PrescaleOption    =    TIM_PRESCALE_USVAL;
	struct_config.PrescaleValue     =    De_uS_S(1);

	struct_match.MatchChannel       =    0;
	struct_match.IntOnMatch         =    ENABLE; //deshabilitamos las interrupciones por timer
	struct_match.ResetOnMatch       =    ENABLE;//resetea el contador del timer cuando se produce un match
	struct_match.StopOnMatch        =    DISABLE; //no detiene el contador del timer cuando se produce un match
	struct_match.ExtMatchOutputType =    TIM_EXTMATCH_NOTHING;
	struct_match.MatchValue         =    60;

	TIM_Init(LPC_TIM1, TIM_TIMER_MODE, &struct_config); //se prende el timer1, se configura la division del clock del periferico, y se
	//configura el timer como modo temporizador y ademas se retesea el contador y se lo saca del reset
	TIM_ConfigMatch(LPC_TIM1, &struct_match); //carga todas las configuraciones del struct_match en ls registros correspondientes

	TIM_ResetCounter(LPC_TIM1);
	TIM_Cmd(LPC_TIM1, DISABLE); //habilita el contador del timer y prescaler

	TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);

	NVIC_EnableIRQ(TIMER1_IRQn);

	return;
}

void confExtInt(void)
{
	GPIO_IntCmd(PINSEL_PORT_2, IN_TECLADO, EDGE_RISING);
	GPIO_ClearInt(PINSEL_PORT_2, IN_TECLADO);

	NVIC_EnableIRQ(EINT3_IRQn);

	return;
}

void confUART(void)
{
	PINSEL_CFG_Type PinCfg;
	//configuraci�n pin de Tx y Rx
	PinCfg.Funcnum = PINSEL_FUNC_1;
	PinCfg.OpenDrain = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode = PINSEL_PINMODE_PULLUP;
	PinCfg.Pinnum = PINSEL_PIN_2;
	PinCfg.Portnum = PINSEL_PORT_0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = PINSEL_PIN_3;
	PINSEL_ConfigPin(&PinCfg);

	UART_CFG_Type UARTConfigStruct;
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	//configuraci�n por defecto:
	UART_ConfigStructInit(&UARTConfigStruct);
	//inicializa perif�rico
	UART_Init(LPC_UART0, &UARTConfigStruct);
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	//Inicializa FIFO
	UART_FIFOConfig(LPC_UART0, &UARTFIFOConfigStruct);
	//Habilita transmisi�n
	UART_TxCmd(LPC_UART0, ENABLE);
}
void confDMA(void)
{
	//Prepare DMA link list item structure
	DMA_LLI_Struct.SrcAddr= (uint32_t)mensaje;
	DMA_LLI_Struct.DstAddr= (uint32_t)&LPC_UART0->THR;
	DMA_LLI_Struct.NextLLI= (uint32_t)&DMA_LLI_Struct;
	DMA_LLI_Struct.Control= sizeof(mensaje)
				| 	(2<<12)
				| 	(1<<26) //source increment
				;

	/* GPDMA block section -------------------------------------------- */
	/* Disable GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);
	/* Initialize GPDMA controller */
	GPDMA_Init();
	// Setup GPDMA channel --------------------------------
	GPDMA_Channel_CFG_Type GPDMACfg;
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// Source memory
	GPDMACfg.SrcMemAddr = (uint32_t)mensaje;
	// Destination memory
	GPDMACfg.DstMemAddr = 0;
	// Transfer size
	GPDMACfg.TransferSize = sizeof(mensaje);
	// Transfer width
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	// Source connection - unused
	GPDMACfg.SrcConn = 0;
	// Destination connection - unused
	GPDMACfg.DstConn = GPDMA_CONN_UART0_Tx;
	// Linker List Item - unused
	GPDMACfg.DMALLI = (uint32_t)&DMA_LLI_Struct;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg);

	// Enable GPDMA channel 0
	GPDMA_ChannelCmd(0, ENABLE);
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Utilidades----------------------------------------------------------------------------------------*/
void change_state(void)
{
	if (modo == LIBRE)
	{
		GPIO_SetValue(PINSEL_PORT_0, (LEDS_GREEN));
		GPIO_ClearValue(PINSEL_PORT_0, (LEDS_RED));
	}
	if (modo == OCUPADO)
	{
		GPIO_SetValue(PINSEL_PORT_0, (LEDS_RED));
		GPIO_ClearValue(PINSEL_PORT_0, (LEDS_GREEN));
	}

	return;
}

uint16_t Convertir_Distancia(uint16_t adc_value)
{
	uint16_t velocidad = 0;
	velocidad = (adc_value * VELOCIDAD_MAX) / RESOLUCION;
	if (velocidad > 0)
	{
		car_state = 1;
		TIM_Cmd(LPC_TIM1, DISABLE);
	}
	else
	{
		if (car_state != STOP)
		{
			TIM_Cmd(LPC_TIM1, ENABLE);
			car_state = STOP;
		}
	}
	return velocidad;
}

uint_fast16_t potencia(uint8_t numero, uint_fast8_t potencia)
{
    uint16_t resultado = numero;
    while (potencia > 1)
    {
        resultado = resultado * numero;
        potencia--;
    }
    if(potencia==0){
    	resultado=1;
    }
    return resultado;
}

void actualizar_mensaje(void)
{
	switch (modo) {
		case LIBRE:
			mensaje[1]=(uint8_t)'L';
			break;
		case OCUPADO:
			mensaje[1]=(uint8_t)'O';
				break;
		case STOP:
			mensaje[1]=(uint8_t)'S';
			break;
	}
	uint16_t temp=tarifa;
	uint8_t temp1=0;
	for(uint8_t i=4;i>0;i--)
	{
		temp1=temp/(potencia(10, i-1));

		mensaje[sizeof(mensaje)-i]=temp1 + '0';

		temp-=temp1*(potencia(10, i-1));
	}
}
uint8_t get_TeclaMatrical(void)
{
	uint8_t fila=0,	columna=0;

	while (fila != 4)
	{
		if (GPIO_ReadValue(2) & (1 << fila))
			break;

		fila++;
	}
	if (fila >= 4)
		return 4;

	while (columna != 4)
	{
		LPC_GPIO2->FIOPIN0 = ~(1 << (4+columna));

		if (!((FIO_ByteReadValue(2,0)) & IN_TECLADO))
			break;

		columna++;
	}
	if (columna >= 4)
		return 4;

	FIO_ByteSetValue(2, 0,OUT_TECLADO);

	return (fila*4 + columna);
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/
/*---------------------------------------------------Handlers----------------------------------------------------------------------------------------*/

void EINT3_IRQHandler(void)
{
	GPIO_SetValue(PINSEL_PORT_0, (BUZZER));
	uint8_t tecla = get_TeclaMatrical();
	for(uint64_t i=0; i<500000 ; i++){}
	GPIO_ClearValue(PINSEL_PORT_0, (BUZZER));
	for(uint64_t i=0; i<500000 ; i++){}

	tecla=teclado_matricial[tecla];
	(tecla > 0 && tecla <= 3) ? (modo = tecla) : (modo = modo) ;
	FIO_ByteSetMask(0, 0, (11<4), DISABLE);
	if(tecla==4)
	{
		if(LED_ON_OFF)
		{
			FIO_ByteClearValue(0, 0, (0b11<<4));
			LED_ON_OFF=0;
		}
		else
		{
			change_state();
			LED_ON_OFF=1;
		}
	}

	GPIO_ClearInt(PINSEL_PORT_2, IN_TECLADO);
	return;
}

void TIMER1_IRQHandler(void)
{
	tarifa += VALOR_FICHA;
	actualizar_mensaje();
	TIM_ClearIntPending(LPC_TIM1, TIM_MR0_INT);

	return;
}

void ADC_IRQHandler(void)
{

	ADC0Value = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);

	distancia += Convertir_Distancia(ADC0Value) * TIMEMUESTREO;

	ADC_GlobalGetStatus(LPC_ADC, 1);

	return;
}
/*---------------------------------------------------------------------------------------------------------------------------------------------------*/



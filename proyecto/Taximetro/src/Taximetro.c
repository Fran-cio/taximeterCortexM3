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




void confGPIO(void); // Prototipo de la funcion de conf. de puertos
void configADC(void);
void config_timer(void);
void confExtInt(void); //Prototipo de la funcion de conf. de interrupciones externas
void change_state(uint8_t state);
void config_timer_1(void);


void rutina_1(void); // poner un nombre mas representativo
void rutina_2(void);
void rutina_3(void);

//void confTimer(void);
uint8_t get_TeclaMatrical(void);


#define OUTPUT 1
#define INPUT 0
#define IN_TECLADO 0XF
#define OUT_TECLADO (0XF << 4)

#define F_MUESTREO 200000
#define CHANNEL_0 0
#define LEDS_RED (1 << 4)
#define LEDS_GREEN (1 << 5)
#define BUZZER (1 << 6)


#define TIMEMUESTREO 1

#define EDGE_RISING 0
#define LENGTH 16

#define VELOCIDAD_MAX 40
#define RESOLUCION 4096
/* Estados del taximetro*/
#define LIBRE 1
#define OCUPADO 2
#define STOP  3

uint8_t modo = LIBRE;
uint16_t tarifa = 0;
uint16_t distancia = 0;
uint8_t car_state = 0;
volatile uint16_t ADC0Value = 0;

uint8_t teclado_matricial[LENGTH] = {1,2,3,0xA, \
									4,5,6,0XB, \
									7,8,9,0xC, \
									0XE,0,0XE,0XD
								  };

int main(void)
{
	confGPIO();
	confExtInt();
	configADC();
	config_timer();
	config_timer_1();


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
    return 0;
}
/* Pasa del estado ocupado o en stop al estado libre */
void rutina_1(void)
{
	change_state(LIBRE);
	TIM_Cmd(LPC_TIM0, DISABLE);
	TIM_Cmd(LPC_TIM1, DISABLE);

	tarifa = 0;

	while (modo == LIBRE)
	{
	}
	return;
}
/* Pasa del estado ocupado o en stop al estado libre */
void rutina_2(void)
{
	change_state(OCUPADO);
	TIM_ResetCounter(LPC_TIM0);
	TIM_Cmd(LPC_TIM0, ENABLE);

	tarifa = 0;
	while (modo == OCUPADO)
	{
		if (distancia > 200)
		{
			tarifa += 9;
			distancia = 0;
		}
	}
	return;
}
void rutina_3(void)
{
	TIM_Cmd(LPC_TIM0, DISABLE);
	TIM_Cmd(LPC_TIM1, DISABLE);

	while (modo == STOP)
	{

	}
	return;
}

void change_state(uint8_t state)
{
	if (state == LIBRE)
	{
		GPIO_SetValue(PINSEL_PORT_0, (LEDS_GREEN));
		GPIO_ClearValue(PINSEL_PORT_0, (LEDS_RED));
	}
	if (state == OCUPADO)
	{
		GPIO_SetValue(PINSEL_PORT_0, (LEDS_RED));
		GPIO_ClearValue(PINSEL_PORT_0, (LEDS_GREEN));
	}

	return;
}
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

	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_ADC, 3);
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCAD, ENABLE);
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
	CLKPWR_SetPCLKDiv(CLKPWR_PCLKSEL_TIMER0, CLKPWR_PCLKSEL_CCLK_DIV_4);
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCTIM0, ENABLE);

    TIM_TIMERCFG_Type    struct_config;
    TIM_MATCHCFG_Type    struct_match;

    struct_config.PrescaleOption    =    TIM_PRESCALE_USVAL;
    struct_config.PrescaleValue     =    1000;

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
	CLKPWR_ConfigPPWR(CLKPWR_PCONP_PCTIM1, ENABLE);

    TIM_TIMERCFG_Type    struct_config;
    TIM_MATCHCFG_Type    struct_match;

    struct_config.PrescaleOption    =    TIM_PRESCALE_USVAL;
    struct_config.PrescaleValue     =    1000000;

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

    return;
}

void confExtInt(void)
{
	GPIO_IntCmd(PINSEL_PORT_2, IN_TECLADO, EDGE_RISING);
	GPIO_ClearInt(PINSEL_PORT_2, IN_TECLADO);

	NVIC_EnableIRQ(EINT3_IRQn);

	return;
}
void EINT3_IRQHandler(void)
{
	GPIO_SetValue(PINSEL_PORT_0, (BUZZER));
	uint8_t tecla = get_TeclaMatrical();
	for(uint64_t i=0; i<100000 ; i++){}
	GPIO_ClearValue(PINSEL_PORT_0, (BUZZER));

	(tecla >= 0 && tecla < 3) ? (modo = teclado_matricial[tecla]) : (modo = modo) ;

	GPIO_ClearInt(PINSEL_PORT_2, IN_TECLADO);

	return;
}
uint16_t Convertir_Distancia(uint16_t adc_value)
{
	uint16_t velocidad = 0;
	if (adc_value > 0)
	{
		velocidad = (adc_value * VELOCIDAD_MAX) / RESOLUCION;
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
void ADC_IRQHandler(void)
{

	ADC0Value = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);

	distancia += Convertir_Distancia(ADC0Value) * TIMEMUESTREO;

	ADC_GlobalGetStatus(LPC_ADC, 1);

	return;
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

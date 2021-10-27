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

void confGPIO(void); // Prototipo de la funcion de conf. de puertos
//void confADC(void);
void confExtInt(void); //Prototipo de la funcion de conf. de interrupciones externas
//void confTimer(void);


#define OUTPUT 1
#define INPUT 0
#define IN_TECLADO 0XF
#define OUT_TECLADO (0XF << 4)
#define LEDS ((1<<4) || (1 << 5))
#define EDGE_RISING 0



int main(void)
{

    return 0;
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

	GPIO_SetDir(PINSEL_PORT_0, LEDS, OUTPUT);

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



}
uint8_t get_TeclaMatrical(void)
{

}
/*

void confPin(void)
{
	PINSEL_CFG_Type conf_pin;
	conf_pin.Funcnum = PINSEL_FUNC_1;
	conf_pin.OpenDrain = PINSEL_PINMODE_NORMAL;
	conf_pin.Pinmode = PINSEL_PINMODE_TRISTATE;
	conf_pin.Portnum = PINSEL_PORT_0;
	conf_pin.Pinnum = 25; //canal 2
	PINSEL_ConfigPin(&conf_pin);

	conf_pin.Funcnum = PINSEL_FUNC_3;
	conf_pin.Portnum = PINSEL_PORT_1;
	conf_pin.Pinnum = 31; //canal 5
	PINSEL_ConfigPin(&conf_pin);

}
void configADC()
{
	/* configuracion del pin p0.23 analogico

	PinCfg.Funcnum = PINSEL_FUNC_1;// AD0.0 ANALOGICO
	PinCfg.Pinmode = PINSEL_PINMODE_TRISTATE; //ni pull-up ni pull-down
	PinCfg.Pinnum = 23;
	PINSEL_ConfigPin(&PinCfg);
	ADC_Init(LPC_ADC, FMUESTREO); //se prende el periferico adc, lo pone en modo operativo, divide el clock para conseguir una frecuencia de muestreo 200k

	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0,ENABLE); //selecciona el canal0

	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);//start conversion cuando el timer sea igual al valor de match1  del timer0
    ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_FALLING);//configuramos que el start del match sea por flanco de bajada
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE); //configura interrupcion del canal 0

	NVIC_EnableIRQ(ADC_IRQn); //habilitamos la interrupcion por adc EN EL NVIC

	return;
}*/


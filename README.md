# Trabajo Integrador de Digital 3
Este trabajo corresponde al grupo 1 de la materia electronica digital 3. Consiste en un taximetro con modos implemtado con ADC, DMA, UART y teclado matricial.

## Integrantes
* Francisco Ciordia Cantarella
* Andres Orionte
* Facundo Carrizo

# Requerimentos
* La tarifa consiste en $ 90 al momento de iniciar el viaje.
* $9 por cada 200 metros de recorrido o minuto de espera en caso de estar el vehículo detenido.
* Boton de Agrego de ficha por bulto.
* 3 modos, PARADO, LIBRE y OCUPADO.
* Poder apagar las led sin importar el modo.

> Valor ficha: $9

# Desarrollo
## Flujo
Posterior a las configuraciones de todos los perifericos que se va a usar.
Tenemos 3 modos:
 1. Taximetro parado, muestra un P en el modo y led en verde.

<p align="center">
 <img width="33%" src="https://github.com/Fran-cio/Tp_digital_3-Taximetro/blob/main/Diagrama%20de%20Flujo/rutina_1.png?raw=true"> 
</p>

 2. Taximetro en funcionamiento, el adc muestrea y aumenta, se prende el led rojo.
 
En el modo 2, se pone el led en rojo,vamos a interrumpir cada muestra de ADC, el valor muestreado se convierte a velocidad y de velocidad se multiplica por el tiempo de muestreo (*1 segundo*), esto se suma a la variable global **distancia**, dentro del main, al salir de la interrupcion, dentro de la rutina del modo 2, chequeamos si la variable global **distancia** es mayor que 200, si es asi, se aumenta en 9 (*valor ficha*) la variable global **tarifa**. Si el ADC muestrea 0, significa que el vehiculo esta detenido, entonces comienza a contar el timer 1, este hace match cada 1 minuto, si hace match, aumenta la varible global **tarifa** una ficha. Si el adc detecta que el el valor muestreado es distinto de 0, apaga el timer 1 y hace lo que comente antes. Ademas, la rutina de modo 2, formatea la informacion para que el dma la envie al uart y esta haga la muestra de informacion.
 
 <p align="center">
 <img width="50%" src="https://github.com/Fran-cio/Tp_digital_3-Taximetro/blob/main/Diagrama%20de%20Flujo/rutina_2.png?raw=true"> 
</p>

 3. Modo detenido, muestra la informacion pero el adc no muestrea entonces la distancia no se esta contando y el led parpadea.
 
 El modo 3, a diferencia del modo 2, debe dejar de muestrear y cambiar el valor de la variable global **tarifa**, el led parpadea. 
 
 <p align="center">
 <img width="33%" src="https://github.com/Fran-cio/Tp_digital_3-Taximetro/blob/main/Diagrama%20de%20Flujo/rutina_3.png?raw=true"> 
</p>





En todo momento se puede apagar el LED, con la tecla 4.
Se puede agregar 5 fichas con la tecla D.


## Configuraciones
### GPIO
**Salida:** 	2.4,2.5,2.6,2.7 (*Teclado matricial*); 0.4,0.5 (*Led de O/L*); 0.6 (*Buzzer*)

**Entradas:** 	2.0,2.1,2.2,2.3 (*Teclado matricial*) (Todas pull-down)

### PINSEL
**Func 0:** 	2.0,2.1,2.2,2.3,2.4,2.5,2.6,2.7 (*Teclado matricial*); 0.4,0.5 (*Led de O/L*); 0.6 (*Buzzer*)

**Func 1:**	0.23 (*ADC 0.0*); 0.2,0.3 (*UART 0.0*)

## Perifericos

### ADC
Configurado cada muestrear cada 1 segundo, con las interrupciones activadas, cada vez que interrumpa, va a ser convertido el valor muestreado a velocidad y por el tiempo de muestreo (1 segundo aproposito) obtenemos la distancia recorrida. Esta distancia se suma a la distancia total, si es mayor que 200, la tarifa aumenta 1 ficha. Si el ADC muestrea 0, prendemos el TIMER 1, sino, lo apagamos. La velocidad maxima muestreada es hasta 40 m/s. Consideramos que mas que eso es innecesario *(e irresponsable)*.

### UART
Mostrar en pantalla la informacion de manera generica

### DMA
Enviar la informacion donde este la tarifa y el modo al UART para mostrar la info en la pantalla mediante un LLI.

### Teclado Matricial
Se mandan 1 por los pines 2.4,2.5,2.6,2.7 (*columnas*) y se activan las interrupciones por flaco de subida en los pines 2.0,2.1,2.2,2.3 (*filas*); los cuales estan conectados a a pulldown. Al apretarse un boton, en la rutina de interrupcion, observamos cual es el pin en 1, seguido de esto, guardamos este valor como la fila ingresada, seguido de esto, barremos con un 0 las columnas, hasta encontrar donde el valor en las columnas es 0. Ahi guardamos el valor de columna, entonces haciendo la cuenta *4.fila+columna* , obtenemos la coordenada dentro del teclado contando desde arriba a la derecha hasta abajo a la izquierda. Con la tecla 1,2,3 elegimos el modo respectivo, ademas, con la tecla 4 se apagan los leds y con la D agregamos a la tarifa el precio por bulto.

 <p align="center">
 <img width="90%" src="https://github.com/Fran-cio/Tp_digital_3-Taximetro/blob/main/Diagrama%20de%20Flujo/EINT3.jpg?raw=true"> 
</p>

### Timer 1
Configurado para contar 1 minuto y si se cumple, aumenta la tarifa una ficha.

### Timer 0
Configurado para hacer toggle en el pin MAT0.1 cada 2hz o cada 0,5s con tal de obtener la frec de muestreo de 1s.

### Systick
Se configura en el modo 3 para hacer parpadear el LED rojo.

> ### Caracteristicas extras.
> * Un led que indica ocupacion (verde/rojo) que se activa con un boton.
> * Un buzzer que indica cuando se apreto un boton.
> * Posibilidad de apagar el led.

Para acceder al diagrama de flujo completo, haga click [aqui](https://raw.githubusercontent.com/Fran-cio/Tp_digital_3-Taximetro/a705a70d15fb5f503a1ddd39fb206571771cd181/Diagrama%20de%20Flujo/Diagrama%20de%20flujo.drawio.svg "Diagrama de flujo").


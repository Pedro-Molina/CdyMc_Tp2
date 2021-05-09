/*
 * tp2.c
 *
 * Created: 3/5/2021 18:01:06
 * Author : molin
 */ 


#include <avr/io.h>
#include <stdio.h>
#include "avr/interrupt.h"
#include "lcd.h"
#include "mef.h"
#include "reloj.h"
#include "keypad.h"
#define F_CPU 8000000UL
#include <stdint.h>
#include <util/delay.h>
#include <stdbool.h>


#define COTA 3 //para que el reloj se actualice cada 1 seg hay que poner este valor en 30, pero  a fines practicos de la simulacion se recomienda setearlo en 3

ISR (TIMER0_COMPA_vect);
void Execute_Tasks ();
void TIMER2_init();

static int contador =0;
static bool MEF_flag =0;

int main(void)
{
	
	LCDinit();
	MEF_init();
	KEYPAD_init();
	TIMER2_init();
	RELOJ_init(16,20,0);
		
	sei();  //habilitar interrupciones
		
		while(1)
		{
			Execute_Tasks(); // ejecutar tareas
			//Sleep(); // por si quisieramos tener un modo sleep pero por ahora no podemos ya que es reloj es sincrónico
		}
		
		
		
    //}
}
//Llama la funcion que actualiza el estado de la MEF cada vez que el flag este en true.
void Execute_Tasks ()
{
	if (MEF_flag) {
		MEF_flag = false;
		MEF_Update(); // MEF
	}
}
//Inicializa timer2
void TIMER2_init()
{
	//TIMER 2
	
	ASSR |= (1 << EXCLK); //timer sincronico
	
	//configuramos el timer en en ctc
	
	TCNT2 = 0; //limpiar registro que guarda la cuenta
	
	OCR2A =255; // ciclos hasta generar el overflow
	
	TCCR2A = 0x02;  //WGM00 = 0  - WGM01 = 1      //////WGM02 = 0 por defecto
	TCCR2B = 0x07;	 // cs02 =0  cs01 = 0   cs00 = 1    ===> 1024 preescalador
	
	TIMSK2 = (1 << OCIE2A); //habilitas la mascara
	
	/* 
	como tenemos 8 Mhz de frecuencia en el micro pero con 1024 de preescalador ----> F = 8Mhz / 1024 = 7812.5 hz 
	por lo que tendremos P = 32.64mS pero recien en la suma numro 255 se generará una interrupción por lo que 
	tiempo= (1.28x10^-4) * 255 seg = 32.64 ms
	*/
}
//Rutina de interrupcion 
//Se ejecuta cada  32.64 ms, actualizando el valor del flag de la MEF y el reloj digital
ISR (TIMER2_COMPA_vect) 
{
	MEF_flag = true;
	if (++contador == COTA)
	{
		contador = 0;
		RELOJ_tickReloj();
	}
}




	
	






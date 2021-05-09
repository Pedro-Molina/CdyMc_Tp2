#include "keypad.h"
#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <stdio.h>


/*
 configuro:
	*Los A B C D del LCD como entrada ,los primeros 4 bits del puerto D como salida del microcontrolador
	*Los 1 2 3 4 del LCD como salida y los  ultimos 4 bits del puerto D como entrada al microcontrolador con pull-up activo
*/
void KEYPAD_init(){
		DDRD = 0b00001111;	//configuro los A B C D del LCD como entrada ,los primeros 4 bits del puerto D como salida del microcontrolador
		PORTD = 0b11110000;	//configuro los 1 2 3 4 del LCD como salida y los  ultimos 4 bits del puerto D como entrada al microcontrolador con pull-up activo
}
/* 
ESTA FUNCION SE ENCARGA DE HACER UN BARRIDO DE UN TECLADO MATRICIAL 4X4 
RETORNA 1 SI SE ENCONTRO UNA TECLA PRESIONADA
RETORNA 0 SI NO SE ENCONTRÓ NINGUNA TECLA PRESIONADA
DE ENCONTRARSE UNA TECLA PRESIONADA SE DEVUELVE POR EL PARÁMETRO PASADO POR REFERENCIA 'TECLA'
*/
static uint8_t KEYPAD_Scan (uint8_t * tecla)
{
	
	uint8_t valores[4] = {0b11111110,0b11111101,0b11111011,0b11110111}; // los 4 BITS de entrada en 1 para activar la resistencia de pull up
	uint8_t volatile fila = 0, columna = 0;
	uint8_t matriz[4][4]={{'1','2','3','A'},{'4','5','6','B'},{'7','8','9','C'},{'*','0','#','D'}};
	bool encontreTeclaPresionada=false;
	
	PORTD = 0b11110000;
	if((PIND & 0b11110000) !=0b11110000) //reviso si hay una tecla presionada en todo el keypad
	{
		for  (uint8_t i = 0; (i < 4)&(!encontreTeclaPresionada) ; i++) // reviso fila por fila
		{
			PORTD = valores[i];
			if((PIND & 0b11110000) != 0b11110000)//si alguno de los primeros 4 bits es distinto significa que hay una tecla presionada en esta fila
			{
				fila = i;
				encontreTeclaPresionada = true;
				switch (PIND & 0b11110000) //me fijo a que columna pertenece la tecla presionada
				{
					case 0b01110000:
					// fila A
					columna = 3;
					break;

					case 0b10110000:
					// fila B
					columna = 2;
					break;
					
					case 0b11010000:
					// fila C
					columna = 1;
					break;
					
					case 0b11100000:
					// fila D
					columna = 0;
					break;
				}
				*tecla= matriz[fila][columna];
				return 1;
			}
			
		}
	}
	return 0;
	
}
/*
 FUNCION QUE SIRVE PARA UN TECLADO MATRICIAL. SE ENCARGA DE DEVOLVER LA TECLA PRESIONADA TENIENDO EN CUENTA DOBLE VERIFICACION
 PARA CUANDO TENEMOS UN PEQUEÑO PERIODO DE INESTABILIDAD AL PRESIONAR UN BOTON O POR RUIDO EXTERNO. Y ADEMAS VERIFICA QUE NO HAYA UNA MULTIPLE
 DETECCION SI NOSOTROS MANTENEMOS PRESIONADA UNA TECLA
*/
uint8_t KEYPAD_Update (uint8_t *pkey)
{
	static uint8_t Old_key;
	uint8_t Key;
	static uint8_t Last_valid_key=0xFF; // no hay tecla presionada
	
	if(!KEYPAD_Scan(&Key)) {
		Old_key=0xFF; // no hay tecla presionada
		Last_valid_key=0xFF;
		return 0;
	}
	if(Key==Old_key) { //2da verificación
		if(Key!=Last_valid_key){ //evita múltiple detección
			*pkey=Key;
			Last_valid_key = Key;
			return 1;
		}
	}
	Old_key=Key; //1era verificación
	return 0;
}

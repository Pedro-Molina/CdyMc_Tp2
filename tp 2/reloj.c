#include <avr/io.h>
#include <stdio.h>
#include "lcd.h"
#include "keypad.h"
#include "mef.h"
#include "reloj.h"
#include <stdbool.h>

	
struct Hora{
	int horas;
	int minutos;
	int segundos;
	
};
typedef struct Hora Hora;


static	Hora hora; //hora actual del reloj

static	bool visibilidad = false; // booleano que define si el reloj debe ser mostrado por pantalla o no 

/*
	Se encarga de iniciar en una hora en específico el reloj, si no envía un numero valido para un campo en especifico se inicializa en cero
*/
void RELOJ_init(int horas,int minutos,int segundos)
{
	if(horas < 23 || horas > 0)
	{
		hora.horas = horas;
	}
	else
	{
		hora.horas = 0;
	}
	if(minutos < 59 || minutos > 0)
	{
		hora.minutos=minutos;
	}
	else
	{
		hora.minutos=0;
	}
	if(segundos < 59 || segundos > 0)
	{
		hora.segundos=segundos;
	}
	else
	{
		hora.segundos=0;
	}
	
}
void RELOJ_setHoras (int horas)
{
	hora.horas=horas;
}
int RELOJ_getHoras()
{
	return hora.horas;
}
void RELOJ_setMinutos (int minutos)
{
	hora.minutos=minutos;
}
int RELOJ_getMinutos()
{
	return hora.minutos;
}
void RELOJ_setSegundos(int segundos)
{
	hora.segundos=segundos;
}
int RELOJ_getSegundos(int segundos)
{
	return hora.segundos;
}
void RELOJ_setVisibilidadReloj(bool b)
{
	visibilidad = b;
}
bool RELOJ_getVisibilidadReloj()
{
	return visibilidad;
}
/* se encarga de aumentar en 1 el valor interno del reloj y de mostrarlo por pantalla si es necesario*/
void RELOJ_tickReloj ()
{
	if(++hora.segundos ==60){
		hora.segundos=0;
		if(++hora.minutos ==60){
			hora.minutos=0;
			if(++hora.horas){
				hora.horas=0;
			}
		}
	}
	if(visibilidad == true){
		RELOJ_actualizarHora(4,0);
	}
	
}
/* 
actualiza el reloj por pantalla campo por campo
*/
void RELOJ_actualizarHora(int columna,int fila){
	RELOJ_actualizarHoras(columna,fila);
	LCDGotoXY(columna+2,fila);
	LCDsendChar(':');
	RELOJ_actualizarMinutos(columna,fila);
	LCDGotoXY(columna+5,fila);
	LCDsendChar(':');
	RELOJ_actualizarSegundos(columna,fila);
}
/*
actualiza los segundos por pantalla
*/
void RELOJ_actualizarSegundos(int columna,int fila){

	
	LCDGotoXY(columna+6,fila);
	
	LCDsendChar((hora.segundos)/(10) + '0');
	
	LCDGotoXY(columna+7,fila);
	
	LCDsendChar(hora.segundos % 10 + '0');
}
/*
actualiza los minutos por pantalla
*/
void RELOJ_actualizarMinutos(int columna,int fila){

	
	LCDGotoXY(columna+3,fila);
	
	LCDsendChar((hora.minutos)/(10) + '0');
	
	LCDGotoXY(columna+4,fila);
	
	LCDsendChar(hora.minutos % 10 + '0');
}
/*
actualiza los horas por pantalla
*/
void RELOJ_actualizarHoras(int columna,int fila){
	
	LCDGotoXY(columna,fila);
	
	LCDsendChar((hora.horas)/(10) + '0');
	
	LCDGotoXY(columna+1,fila);
	
	LCDsendChar(hora.horas % 10 + '0');
}
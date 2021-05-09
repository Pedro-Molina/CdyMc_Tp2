#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>

void RELOJ_init(int horas,int minutos,int segundos);
void RELOJ_setHoras(int horas);
int RELOJ_getHoras();
void RELOJ_setMinutos(int minutos);
int RELOJ_getMinutos();
void RELOJ_setSegundos(int segundos);
int RELOJ_getSegundos();
void RELOJ_setVisibilidadReloj(bool);
bool RELOJ_getVisibilidadReloj();
void RELOJ_tickReloj ();
void RELOJ_actualizarHora(int,int);
void RELOJ_actualizarSegundos(int,int);
void RELOJ_actualizarMinutos(int,int);
void RELOJ_actualizarHoras(int,int);


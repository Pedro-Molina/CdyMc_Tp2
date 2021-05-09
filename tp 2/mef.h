
#include <avr/io.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum {OPEN,CLOSED,DENEGATED,CHANGE_KEY,NEW_KEY,NEW_KEY_SUCCESSFUL,CHANGE_SECONDS,CHANGE_MINUTES,CHANGE_HOURS}STATE;
typedef enum {HORA,MINUTOS,SEGUNDOS}TIPOHORA;

void MEF_init();	
void MEF_Update();
STATE MEF_Choose_State();
bool MEF_Read_Password(uint8_t key);
bool MEF_correct_password(uint8_t * key);
bool MEF_claveCorrecta();
bool MEF_nuevaClave();
void MEF_guardarNuevaClave(uint8_t * nueva);
void MEF_clearFila(uint8_t fila);
void MEF_cambiarMinutosYSegundos(TIPOHORA tiempo);
void MEF_cambiarHora();
bool MEF_verificarNumero (uint8_t key);

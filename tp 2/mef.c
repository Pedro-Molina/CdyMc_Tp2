#include <avr/io.h>
#include <stdio.h>
#include "mef.h"
#include "lcd.h"
#include "keypad.h"
#include "reloj.h"
#include <stdbool.h>

#define COTA_Open 12 //para que esté abierto 3 segundos se debe poner en 92 pero a fines practicos de la simulacion se recomienda setear en 12
#define COTA_Denegated 8 //para que esté abierto 2 segundos se debe poner en 61 pero a fines practicos de la simulacion se recomienda setear en 8
#define COTA_New_Key 8 //para que esté abierto 2 segundos se debe poner en 61 pero a fines practicos de la simulacion se recomienda setear en 8

static int State_call_count;
static uint8_t current_key[4] = {'0','8','5','2'};
static STATE System_State;
static uint8_t sOPEN[] = "OPEN";
static uint8_t sCLOSED[] = "CLOSED";
static uint8_t sDENEGATED[] = "DENEGATED";
static uint8_t sNEW_KEY1[] = "INGRESE NUEVA";
static uint8_t sNEW_KEY2[] = "CLAVE:";
static uint8_t sCHANGE_KEY1[] = "INGRESE CLAVE";
static uint8_t sCHANGE_KEY2[] = "ACTUAL:";
static uint8_t sKEY_SUCCESFULLY_CHANGED1[] = "FIN  INGRESO";
static uint8_t sKEY_SUCCESFULLY_CHANGED2[] = "NUEVA  CLAVE";
static uint8_t sASTERISKS[]="****";
static uint8_t sCHANGE_SECONDS[]="CAMBIAR SEGUNDOS";
static uint8_t sCHANGE_MINUTES[]="CAMBIAR MINUTOS";
static uint8_t sCHANGE_HOURS[]="CAMBIAR HORAS";	
	
//Inicializa la cerradura
void MEF_init()
{
	System_State =CLOSED;
	State_call_count = 0;
	
}	
//Actualiza el estado de la cerradura
void MEF_Update()
{
	State_call_count++;
	
	switch (System_State)
	{
		case CLOSED:
		LCDclr();
		RELOJ_setVisibilidadReloj(true);
		LCDGotoXY(5,1);
		LCDstring(sCLOSED,6);
		
		System_State = MEF_Choose_State();		
		State_call_count = 0;
		
		case OPEN:
		LCDclr();
		
		//imprimo los asteriscos de nuevo para que se mantengan en pantalla
		LCDGotoXY(6,0);
		LCDstring(sASTERISKS,4);
		
		//escribo el estado OPEN
		LCDGotoXY(6,1);
		LCDstring(sOPEN,4);
		if (State_call_count > COTA_Open){
			System_State = CLOSED;
			State_call_count=0;
		}
		
		break;
		
		case DENEGATED:
		LCDclr();
		LCDGotoXY(4,1);
		LCDstring(sDENEGATED,9);
		if (State_call_count > COTA_Denegated){
			System_State = CLOSED;
			State_call_count = 0;
		}
		break;
		
		case CHANGE_KEY:
		LCDclr();
		LCDGotoXY(0,0);
		LCDstring(sCHANGE_KEY1,13);
		LCDGotoXY(0,1);
		LCDstring(sCHANGE_KEY2,7);
		
		if(MEF_claveCorrecta())
		{
			System_State = NEW_KEY;
		}
		else
		{
			System_State = DENEGATED;
		}
		State_call_count = 0;
		break;
		
		case NEW_KEY:
		LCDclr();
		LCDGotoXY(0,0);
		LCDstring(sNEW_KEY1,13);
		LCDGotoXY(0,1);
		LCDstring(sNEW_KEY2,6);
		
		if(MEF_nuevaClave())
		{
			System_State = NEW_KEY_SUCCESSFUL;
		}
		else
		{
			System_State = CLOSED;
		}
		State_call_count = 0;
		//espero nueva clave
		//si llega un # cancelo
		//si persiono D termina de ingresar la nueva contrasna y voy al estado new key succesfully
		
		break;
		
		case NEW_KEY_SUCCESSFUL:
		LCDclr();
		LCDGotoXY(2,0);
		LCDstring(sKEY_SUCCESFULLY_CHANGED1,12);
		LCDGotoXY(2,1);
		LCDstring(sKEY_SUCCESFULLY_CHANGED2,12);
		if (State_call_count > COTA_New_Key)
		{
			System_State = CLOSED;
			State_call_count = 0;
		}
		//se muestra que cambie la contrasena correctamente
		break;
		
		case CHANGE_SECONDS:
		LCDclr();
		LCDGotoXY(0,0);
		LCDstring(sCHANGE_SECONDS,16);
		MEF_cambiarMinutosYSegundos(SEGUNDOS);
		System_State = CLOSED;
		State_call_count = 0;
		break;
		
		case CHANGE_MINUTES:
		LCDclr();
		LCDGotoXY(0,0);
		LCDstring(sCHANGE_MINUTES,15);
		MEF_cambiarMinutosYSegundos(MINUTOS);
		System_State = CLOSED;
		State_call_count = 0;
		break;
		
		case CHANGE_HOURS:
		LCDclr();
		LCDGotoXY(0,0);
		LCDstring(sCHANGE_HOURS,12);
		MEF_cambiarHora();
		System_State = CLOSED;
		State_call_count = 0;
		break;
		
		default:break;
	}
}
//Se encaraga de decidir cual es el estado siguiente dependiendo de la primer tecla ingresada en el modo "CERRADO"
//Devuelve el estado correspondiente
STATE MEF_Choose_State(){
	uint8_t tecla;
	
	bool espero = true;
	
	while(espero){
		if (KEYPAD_Update(&tecla))
		{
			espero = false;
			RELOJ_setVisibilidadReloj(false); //no quiero que se muestre mas el reloj
			
			
			MEF_clearFila(0);
		}
	}
	
	switch(tecla){
		case 'A':
		return CHANGE_HOURS;
		break;
		
		case 'B':
		return CHANGE_MINUTES;
		break;
		
		case 'C':
		return CHANGE_SECONDS;
		break;
		
		case 'D':
		return CHANGE_KEY;
		break;
		
		default: break;
	}
	//si llego aca es porque estoy leyendo una contraseña y en el siguiente metodo leo los 3 digitos faltantes
	if(MEF_Read_Password(tecla))
	{
		return OPEN;
	}
	else
	{
		return DENEGATED;
	}
}
//Se encarga de la lectura de la clave cuando la cerradura se encuentra en el estado"CERRADO"
//Se le envia por parametro la primer tecla ya que solo se ingresara a esta funcion si la primer tecla ingresada no es una "A","B" o "C"
//Devuelve true si la clave es igual a la clave actual, de lo contrario devuelve false.
bool MEF_Read_Password(uint8_t primeraTecla)
{
	
	uint8_t dig =1, tecla;
	uint8_t key[4];
	
	key[0]= primeraTecla; //guardo el primer digito en mi vector de digitos ingresados
	LCDGotoXY(5 + dig,0);
	LCDsendChar('*');
	
	while (dig < 4){
		if (KEYPAD_Update(&tecla))
		{
			key[dig] = tecla;
			dig++;
			LCDGotoXY(5 + dig,0);
			LCDsendChar('*');
		}
		
	}
	
	
	
	return MEF_correct_password (key);
	
}
//Compara la clave actual con la clave enviada por parametro
//Devuelve true si las claves son iguales, de otra forma devuelve false
bool MEF_correct_password(uint8_t * key){
	
	uint8_t i;
	bool comparacion=true;
	for (i=0; i<4; i++)
	{
		if(key[i] != current_key[i]){
			comparacion =false;
			break;
		}
	}
	
	return comparacion;
}
//Controla el ingreso del la clave actual para cambiar la contraseña,
//Si es correcta devuelve true, sino devuelve false.
bool MEF_claveCorrecta()
{
	
	uint8_t dig =0, tecla;
	uint8_t key[4];
	
	while (dig < 4){
		if (KEYPAD_Update(&tecla))
		{
			key[dig] = tecla;
			dig++;
			LCDGotoXY(8 + dig,1);
			LCDsendChar('*');
		}
		
	}
	
	
	
	return MEF_correct_password (key);
	
}
//Controla el ingreso de la nueva clave
//Devuelve true si se ingresaron los 4 digitos y se presiono la 'D', false si se presiona '#'
bool MEF_nuevaClave()
{
	uint8_t dig =0, tecla = '0';
	uint8_t key[4];
	
	
	while ((dig < 5) || (tecla != 'D')){
		if (KEYPAD_Update(&tecla))
		{
			if(tecla != '#'){
				if (dig <= 3){
					key[dig] = tecla;
					LCDGotoXY(8 + dig,1);
					LCDsendChar('*');
				}
			}else
			return false;
			
			dig++;
		}
	}
	
	MEF_guardarNuevaClave(key);
	return true;
}
//Guarda la nueva clave
void MEF_guardarNuevaClave(uint8_t * nueva)
{
	uint8_t i ;
	for(i =0 ; i < 4 ; i++)
	{
		current_key[i] = nueva[i];
	}
}
void MEF_clearFila (uint8_t fila){ 
	for(uint8_t i = 0 ; i<16 ;i++){
		LCDGotoXY(i,fila);
		LCDsendChar(' ');
	}
}

/*
El formato que tienen los minutos y segundos es lo mismo por lo que para modificarlos podemos usar una misma función.
La función recibe un parámetro que le indica si debe modificar los minutos o segundos.
Primero limpia la segunda fila del lcd y luego imprime el valor del reloj en ese momento.
Para saber con qué tecla se van a confirmar los cambios, nos fijamos qué parámetro recibimos y guardamos la tecla en la variable comparación.
En el primer while leemos el primer valor que el usuario ingresa. Como estamos modificando los minutos o segundos, 
el primer valor solo puede ser un número del 0 al 5, o un ‘#’ en caso de que desee cancelar la operación. 
El while se quedará iterando hasta que ingrese uno de estos valores, los valores erróneos serán ignorados.
En el segundo while se lee el segundo dígito, en el caso de que el usuario no haya ingresado un ‘#’ en el anterior.
 Este dígito solo puede ser un número del 0 al 9, o un ‘#’. El while se quedará iterando hasta que ingrese uno de estos valores, los valores erróneos serán ignorados.
En el tercer while se lee el tercer dígito, en el caso de que el usuario no haya ingresado un ‘#’ en el anterior. 
Este dígito solo puede ser la tecla con la que se confirman los cambios, ‘B’ o  ‘C’ dependiendo de qué campo está modificando, o un ‘#’. 
El while se quedará iterando hasta que ingrese uno de estos valores, los valores erróneos serán ignorados.
Por último, si nunca ingrese un ‘#’, guardo el nuevo valor ingresado en el campo del reloj que deseo modificar.
*/

void MEF_cambiarMinutosYSegundos(TIPOHORA tiempo){
	uint8_t tecla, numero, comparacion=' ';
	bool cancelar=true, digito1=true, digito2=true, digito3=true;
	MEF_clearFila(1);
	RELOJ_actualizarHora(0,1);
	switch (tiempo)
	{
		case MINUTOS:
		LCDGotoXY(3,1);
		comparacion='B';
		break;
		case SEGUNDOS:
		LCDGotoXY(6,1);
		comparacion='C';
		break;
		default:break;
	}
	LCDcursorOnBlink();
	while (digito1){
		if (KEYPAD_Update(&tecla)){
			if (MEF_verificarNumero(tecla)){
				if (tecla < ('6')){
					numero=(tecla-'0')*10; //Convierto el valor de ascii a entero y lo multiplico por 10
					digito1=false;
					LCDsendChar(tecla);
				}
				}else{
				if (tecla=='#'){
					cancelar=false;
					digito1=false;
					digito2=false;
					digito3=false;
				}
			}
		}
	}

	while(digito2){
		if (KEYPAD_Update(&tecla)){
			if (MEF_verificarNumero(tecla)){
				numero+=(tecla-'0'); //Convierto el valor de ascii a entero
				digito2=false;
				LCDsendChar(tecla);
				}else{
				if (tecla=='#'){
					cancelar=false;
					digito2=false;
					digito3=false;
				}
			}
		}
	}

	while (digito3){
		if (KEYPAD_Update(&tecla)){
			if (tecla==comparacion){
				digito3=false;
				}else{
				if (tecla=='#'){
					cancelar=false;
					digito3=false;
				}
			}
		}
	}

	if (cancelar){
		switch (tiempo)
		{
			case MINUTOS:
			RELOJ_setMinutos(numero);
			break;
			case SEGUNDOS:
			RELOJ_setSegundos(numero);
			break;
			
			default:break;
		}
	}
	LCDcursorOFF();
}

/*
Esta función permite cambiar el campo hora del reloj. Se debe tener en cuenta que el formato de la hora solo permite un valor numérico entre 0 y 24.
Primero limpia la segunda fila del lcd y luego imprime el valor del reloj en ese momento.
En el primer while leemos el primer valor que el usuario ingresa. Como estamos modificando la hora, el primer valor solo puede ser un número del 0 al 2, 
o un ‘#’ en caso de que desee cancelar la operación. El while se quedará iterando hasta que ingrese uno de estos valores, los valores erróneos serán ignorados.
En el segundo while se lee el segundo dígito, en el caso de que el usuario no haya ingresado un ‘#’ en el anterior. Este dígito solo puede ser un número del 0 al 9 
si el primer dígito ingresado fué menor a 2, o un número del 0 al 3 si el primer dígito fué un 2, o un ‘#’. El while se quedará iterando hasta que ingrese uno de
 estos valores, los valores erróneos serán ignorados.
En el tercer while se lee el tercer dígito, en el caso de que el usuario no haya ingresado un ‘#’ en el anterior. Este dígito solo puede ser la tecla ‘A’ 
con la que se confirman los cambios, o un ‘#’. El while se quedará iterando hasta que ingrese uno de estos valores, los valores erróneos serán ignorados.
Por último, si nunca ingrese un ‘#’, guardo el nuevo valor ingresado en el campo hora del reloj.
*/

void MEF_cambiarHora (){
	uint8_t tecla, numero;
	bool cancelar=true, digito1=true, digito2=true, digito3=true;
	MEF_clearFila(1);
	RELOJ_actualizarHora(0,1);
	LCDGotoXY(0,1);
	LCDcursorOnBlink();
	while (digito1){
		if (KEYPAD_Update(&tecla)){
			if (MEF_verificarNumero(tecla)){
				if (tecla < ('3')){
					numero=(tecla-'0')*10; //Convierto el valor de ascii a entero y lo multiplico por 10
					digito1=false;
					LCDsendChar(tecla);
				}
				}else{
				if (tecla=='#'){
					cancelar=false;
					digito1=false;
					digito2=false;
					digito3=false;
				}
			}
		}
	}
	
	while(digito2){
		if (KEYPAD_Update(&tecla)){
			if (MEF_verificarNumero(tecla)){
				if (numero<20){
					numero+=(tecla-'0'); //Convierto el valor de ascii a entero
					digito2=false;
					LCDsendChar(tecla);
					}else{
					if (tecla<'4'){
						numero+=(tecla-'0'); //Convierto el valor de ascii a entero
						digito2=false;
						LCDGotoXY(1,1);
						LCDsendChar(tecla);
					}
				}
				}else{
				if (tecla=='#'){
					cancelar=false;
					digito2=false;
					digito3=false;
				}
			}
		}
	}
	
	while (digito3){
		if (KEYPAD_Update(&tecla)){
			if (tecla=='A'){
				digito3=false;
				}else{
				if (tecla=='#'){
					cancelar=false;
					digito3=false;
				}
			}
		}
	}
	
	if (cancelar){
		RELOJ_setHoras(numero);
	}
	LCDcursorOFF();
}
bool MEF_verificarNumero (uint8_t key){
	
	if ((key!='A')&&(key!='B')&&(key!='C')&&(key!='D')&&(key!='*')&&(key!='#')){
		return true;
		}else{
		return false;
	}
}

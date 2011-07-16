#include <stdio.h>
#include "TDA/COLAC.H"
#include "TDA/PILAC.H"
#include "TDA/SECUEN.H"
#include "TDA/SECUEN2.H"
#include "TDA/SECUEN3.H"

// TODO: verificar letras repetidas
int obteneralfabeto(Tsecuencia sec, TcolaC *alfabeto) {
	char cespeciales[] = "()*+{}[]";
	char c;
	int i;

	sleerprim(&sec, &c);

	while(!sfin(&sec) && !cllenac(alfabeto)) {
		for(i=0; i<(sizeof(cespeciales)-1); i++) {
			if(cespeciales[i] == c)
				return 1; // encontro caracter no permitido
		}
		cponerc(alfabeto, c);
		sleersig(&sec, &c);
	}
	if(!sfin(&sec))
		return 2; // secuencia no llego a fin y cola llena
	
	return 0;
}

int obtenerexpresion(Tsecuencia2 sec, TpilaC *regexp, TcolaC *alfabeto) {
	int j;
	char cespeciales[]="()+*";
	char pabierto = 0;
	char antpabierto = 0; // el anterior es un parentesis abierto
	char antmodificador = 0; // el anterior es un modificador
	TcolaC aux; // auxiliar para alfabeto
	char c, c2;
	char encontrado;
	
	ccrearc(&aux);
	sleerprim2(&sec, &c);

	if(c=='+' || c=='*')
		return 1;
		
	while(!sfin(&sec)) {
		for(j=0; j<(sizeof(cespeciales)-1) && c!=cespeciales[j]; j++);
		if(j==(sizeof(cespeciales)-1)) {
			encontrado=0;
			while(!cvaciac(alfabeto) && !cllenac(&aux)) {
				csacarc(alfabeto, &c2);
				cponerc(&aux, c2);
				if(c2==c)
					encontrado=1;
			}
			if(!encontrado) {
				puts("Caracter no perteneciente al alfabeto...");
				return 2;
			}
			while(!cvaciac(&aux) && !cllenac(alfabeto)) {
				csacarc(&aux, &c2);
				cponerc(alfabeto, c2);
			}
		}
		
		switch(c) {
		case '(':
			if(!pabierto) {
				pabierto=1;
				antmodificador=0;
				antpabierto=1;
			} else {
				return 3;
			}
			break;
		case ')':
			if(pabierto) {
				pabierto=0;
				antmodificador=0;
			} else {
				return 4;
			}
			break;
		case '*':
		case '+':
			if(!antmodificador && !antpabierto)
				antmodificador=1;
			else
				return 5;
			break;
		default:
			antmodificador=0;
		}
		
		if(c!='(')
			antpabierto=0;

		if(!pllenac(regexp))
			pponerc(regexp, c);
		sleersig2(&sec, &c);
	}
	
	if(pabierto)
		return 6;
	
	return 0;
}

void procesar(TpilaC *regexp, TpilaC *sarta){
	char clausura=0;
	char ignorar=0;
	TpilaC aux;
	char c;

	pcrearc(&aux);
	
	while(!pvaciac(regexp) && !pllenac(&aux)) {
		psacarc(regexp, &c);
		pponerc(&aux, c);
		switch(c) {
		case '*':
			clausura=1;
			break;
		case '+':
			break;
		case '(':
			clausura=0;
			ignorar=0;
			break;
		case ')':
			if(clausura)
				ignorar=1;
			break;
		default:
			if(!ignorar) {
				if(clausura)
					clausura=0;
				else
					if(!pllenac(sarta))
						pponerc(sarta, c);
			}
		}
	}
	while(!pvaciac(&aux) && !pllenac(regexp)) {
		psacarc(&aux, &c);
		pponerc(regexp, c);
	}
}

int main(int argc, char **argv){
	TcolaC alfabeto;
	TpilaC expresion;
	TpilaC sarta;
	Tsecuencia sec1; // contiene el alfabeto
	Tsecuencia2 sec2; // contiene la expresion regular
	Tsecuencia3 sec3; // contiene la sarta mas corta (salida)
	char c, c2;

	ccrearc(&alfabeto);
	pcrearc(&expresion);
	pcrearc(&sarta);

	if(obteneralfabeto(sec1, &alfabeto)) {
		puts("Error al obtener alfabeto");
		return 1;
	}

	if(obtenerexpresion(sec2, &expresion, &alfabeto)) {
		puts("Error al obtener expresion");
		return 2;
	}

	procesar(&expresion, &sarta);

	sleerprim3(&sec3, &c2);
	while(!sfin(&sec3)) {
		if(!pvaciac(&sarta)) {
			psacarc(&sarta, &c);
			if(c!=c2) {
				puts("No concuerdan.");
				return 3;
			}
		} else {
			puts("No concuerdan: sarta mas corta (1)");
			return 4;
		}
		sleersig3(&sec3, &c2);
	}
	if(!pvaciac(&sarta)) {
			puts("No concuerdan: sarta mas corta (2)");
			return 5;
	}

	puts("Concuerda correctamente.");

	return 0;
}


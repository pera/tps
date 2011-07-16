/*
 * Recuperacion de Trabajo Practico de Estructura de Datos I (2011)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "tfecha.h"
#include "tlistald.h"
#include "tdato.h"

const int Costo=68;

typedef enum codigo {
	alta=1,
	baja,
	modificar,
	listar,
	salir
} codigo;

/*
 * pre: -
 * pos: S sera un puntero a un string sii fueron ingresados caracteres
 *      imprimibles, retornando S en caso exitoso o 0 en error
 * nota: S debe apuntar a NULL, esto es para evitar memory leaks
 *
 */
char *obtenerTexto(char **s, unsigned int size) {
	int i;

	if(*s!=NULL){
		return 0;
	}

	*s=(char*)malloc(size*sizeof(char));
	if (!fgets(*s, size, stdin)) {
		free(*s);
		*s=NULL;
		return 0;
	}
	// TODO (flush) falta verificar EOF (ctrl+d)
	for (i = 0; (*s)[i]!='\0' && (*s)[i]!='\n'; i++);
	if ((*s)[i]=='\0') while (getchar()!='\n');

	for (i=0; (*s)[i]!='\0'; i++) {
		if (!(isprint((*s)[i]))) {
			if ((*s)[i]=='\n') {
				(*s)[i]='\0';
				break;
			} else {
				free(*s);
				*s=NULL;
				return 0;
			}
		}
	}
	if (i==0) {
		free(*s);
		*s=NULL;
		return 0;
	}

	return *s;
}

/*
 * pre: -
 * pos: si fue ingresado un numero, N valdra este y se retornara 1, en caso de
 *      error la rutina retornara 0 y N valdra tambien 0
 *
 */
int obtenerNumero(unsigned long int *n, int max) {
	const int size=9;
	int i;
	char *s;

	*n=0;

	s=(char*)malloc(size*sizeof(char));
	if(!fgets(s, size+1, stdin)) {
		free(s);
		return 0;
	}
	// TODO (flush) falta verificar EOF (ctrl+d)
	for (i = 0; s[i]!='\0' && s[i]!='\n'; i++);
	if (s[i]=='\0') while (getchar()!='\n');

	for (i=0; s[i]!='\0'; i++) {
		if (isdigit(s[i]) && i<max) {
			*n=(*n)*10+(s[i]-'0');
		} else {
			if (s[i]=='\n') {
				s[i]='\0';
				break;
			} else {
				free(s);
				*n=0;
				return 0;
			}
		}
	}
	if (i==0) {
		free(s);
		*n=0;
		return 0;
	}

	free(s);

	return 1;
}

codigo obtenerAccion() {
    int respuesta='\n';
    codigo accion=0;

    do {
		while (respuesta!='\n') respuesta=getchar();
        printf("\nIndique [a]lta, [b]aja, [m]odificar, [l]istar o [s]alir: ");
        respuesta=getchar();
		switch (respuesta) {
			case 'a':
				accion=alta;
				break;
			case 'b':
				accion=baja;
				break;
			case 'm':
				accion=modificar;
				break;
			case 'l':
				accion=listar;
				break;
			case 's':
				accion=salir;
				break;
		}
    } while (!accion);

	while (respuesta!='\n') respuesta=getchar();

    return accion;
}

int obtenerConfirmacion(const char *msg) {
    int respuesta='\n';
    int accion=-1;

    do {
		while (respuesta!='\n') respuesta=getchar();
        printf(msg);
        respuesta=getchar();
		switch (respuesta) {
			case 's':
				accion=1;
				break;
			case 'n':
				accion=0;
				break;
		}
    } while (accion<0);

	while (respuesta!='\n') respuesta=getchar();

    return accion;
}

void ingresarDatos(tdato *h) {
	char *string=NULL;
	int i, error;

	do {
		printf("Ingrese el nombre y apellido: ");
	} while (!obtenerTexto(&string, 30));
	strcpy((char*)h->nombre_apellido, string);
	
	do {
		printf("Ingrese numero de documento: ");
	} while (!obtenerNumero(&h->documento, 8));

	free(string);
	string=NULL;
	do {
		printf("Ingrese el hotel: ");
	} while (!obtenerTexto(&string, 20));
	strcpy((char*)h->hotel, string);

	do {
		printf("Ingrese numero de habitacion: ");
	} while (!obtenerNumero((unsigned long int*)&h->habitacion, 4));

	do {
			free(string);
			string=NULL;
		do {
			printf("Ingrese fecha de registracion: ");
		} while(!obtenerTexto(&string, 11));
		FConvertirFLegibleTFecha(string,&h->registracion,&error);
		error|=!FValida(h->registracion);
	} while (error);

	for (i = 0; i < 31; i++) {
		h->gastos_diarios[i].fecha=0;
		h->gastos_diarios[i].total_del_dia=0;
	}

	sprintf(h->clave, "%20s%4i", h->hotel, h->habitacion);
}

int encontrarClave(tlista *l) {
	char *string=NULL;
	int existe;
	tdato aux;

	do {
		printf("Ingrese el hotel: ");
	} while (!obtenerTexto(&string, 20));
	strcpy(aux.hotel, string);
	free(string);

	do {
		printf("Ingrese numero de habitacion: ");
	} while (!obtenerNumero((unsigned long int*)&aux.habitacion, 4));

	sprintf(aux.clave, "%20s%4i", aux.hotel, aux.habitacion);

	LBuscarOrdenado(l, aux.clave, &existe);

	return existe;
}

void tratarCambio(tlista *l, tdato *h, tfecha factual) {
	char *string=NULL;
	int existe;

	do {
		do {
			printf("Ingrese nuevo hotel: ");
		} while (!obtenerTexto(&string, 20));
		strcpy(h->hotel, string);
		free(string);
		string=NULL;

		do {
			printf("Ingrese nueva habitacion: ");
		} while (!obtenerNumero((unsigned long int*)&h->habitacion, 4));

		sprintf(h->clave, "%20s%4i", h->hotel, h->habitacion);

		LBuscarOrdenado(l, h->clave, &existe);
		if (existe)
			puts("La habitacion se encuentra ocupada");
	} while (existe);

	h->registracion=factual;
}

tfecha obtenerFechaActual(tlista *l) {
	char *string=NULL;
	int error;
	tfecha factual;

	do {
		do {
			printf("Ingrese fecha actual: ");
		} while(!obtenerTexto(&string, 11));
		FConvertirFLegibleTFecha(string,&factual,&error);
		error|=!FValida(factual);
		free(string);
		string=NULL;
	} while (error);

	return factual;
}

void listarDatos(tlista *l) {
	tdato d, hotel_ant;
	tfecha factual;
	int gastos;
	int i;

	factual=obtenerFechaActual(l);

	hotel_ant.hotel[0]='\0';

	if (!LVacia(*l)) {
		LPpio(l);
		while (!LFin(*l)) {
			LInfo(l, &d);
			if (strcmp(d.hotel,hotel_ant.hotel)!=0)
				printf("\n%s:\n", d.hotel);
			gastos=0;
			for (i = 0; i < 31; i++) {
				if (d.gastos_diarios[i].fecha)
					gastos+=d.gastos_diarios[i].total_del_dia;
			}
			printf("%4i\t%8li\t%i\t%i$\n", d.habitacion, d.documento, gastos, gastos+(Costo*FDiferenciaDias(factual,d.registracion)));
			strcpy(hotel_ant.hotel,d.hotel);
			LSig(l);
		}
	}
}

void leerMaestro(tlista *l, FILE *maestro) {
	tdato hospedaje;

	fread(&hospedaje, sizeof(hospedaje), 1, maestro);
	while (!feof(maestro) && !ferror(maestro)) {
		LInsertarOrden(l, hospedaje, 'A');
		fread(&hospedaje, sizeof(hospedaje), 1, maestro);
	}
}

void escribirMaestro(tlista *l, FILE *maestro) {
	tdato hospedaje;

	LPpio(l);
	while (!LFin(*l)) {
		LInfo(l, &hospedaje);
		fwrite(&hospedaje, sizeof(hospedaje), 1, maestro);
		LSig(l);
	}
}

int main(int argc, char *argv[]) {
	FILE *maestro;
	tlista lista;
	tdato hospedaje;
	codigo accion;
	int existe;
	tclave clave_aux;
	tfecha factual;

	LCrear(&lista);

	/* El archivo maestro puede no ser portable entre plataformas */
	if (!(maestro=fopen("maestro.bin","rb"))) {
		if (!(maestro=fopen("maestro.bin","wb"))) {
			puts("Error al crear archivo maestro.");
			return 1;
		}
	} else {
		leerMaestro(&lista, maestro);
		fclose(maestro);
		if (!(maestro=fopen("maestro.bin","wb"))) {
			puts("Error al abrir archivo maestro.");
			return 1;
		}
	}

	while (accion!=salir) {
		accion=obtenerAccion();
		switch (accion) {
			case alta:
				ingresarDatos(&hospedaje);
				sprintf(clave_aux, "%20s%4i", hospedaje.hotel, hospedaje.habitacion);
				LBuscarOrdenado(&lista, clave_aux, &existe);
				if (existe)
					puts("La habitacion se encuentra ocupada");
				else
					LInsertarOrden(&lista, hospedaje, 'A');
				break;
			case baja:
				if (!LVacia(lista)) {
					if (encontrarClave(&lista)) {
						LInfo(&lista, &hospedaje);
						for (;;) {
							factual=obtenerFechaActual(&lista);
							if (!FMayor(hospedaje.registracion,factual)) break;
							puts(" error: la fecha indicada es anterior a la registracion");
						}
						printf("El saldo a pagar es de: %i$\n", Costo*FDiferenciaDias(factual,hospedaje.registracion));
						LBorrarActual(&lista);
					} else {
						puts("La habitacion no esta ocupada");
					}
				} else {
					puts("No hay personas registradas...");
				}
				break;
			case modificar:
				if (!LVacia(lista)) {
					if (encontrarClave(&lista)) {
						LInfo(&lista, &hospedaje);
						for (;;) {
							factual=obtenerFechaActual(&lista);
							if (!FMayor(hospedaje.registracion,factual)) break;
							puts(" error: la fecha indicada es anterior a la registracion");
						}
						printf("El saldo a pagar es de: %i$\n", Costo*FDiferenciaDias(factual,hospedaje.registracion));
						LBorrarActual(&lista);
						tratarCambio(&lista, &hospedaje, factual);
						LInsertarOrden(&lista, hospedaje, 'A');
					} else {
						puts("No hay personas registradas...");
					}
				} else {
					puts("No hay personas registradas...");
				}
				break;
			case listar:
				listarDatos(&lista);
				break;
			case salir:
				if (obtenerConfirmacion("Desea guardar los cambios? (s/n): "))
					escribirMaestro(&lista, maestro);
				puts("");
				break;
		}
	}

	fclose(maestro);
	return 0;
}


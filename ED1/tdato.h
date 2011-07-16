/*
 * Si CLAVE_CADENA se encuentra definida entonces la implementacion usara una
 * clave alfanumerica. En caso contrario la clave sera numerica.
 *
 */
#ifndef TDATO
#define TDATO

#define CLAVE_CADENA
#define CLAVE_SIZE 20+4+1 // hotel + habitacion + \0

#ifdef CLAVE_CADENA
	typedef char tclave[CLAVE_SIZE];
#else
	typedef long int tclave;
#endif

typedef struct tgastos {
	tfecha fecha;
	short int total_del_dia;
} tgastos;

typedef struct tdato {
	tclave clave; /* NO ELIMINAR */

	char nombre_apellido[30];
	unsigned long int documento;
	char hotel[20];
	unsigned short int habitacion; // 0-9999
	tfecha registracion;
	tgastos gastos_diarios[31];
} tdato;

#endif


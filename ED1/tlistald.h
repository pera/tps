/*
 * Implementacion del TDA de lista doblemente enlazada lineal
 *
 */

#ifndef TLISTA
#define TLISTA

#include <stdlib.h>
#include "tdato.h"

#ifdef CLAVE_CADENA
 #include <string.h>
#endif

typedef struct nodo {
	tdato info;
	struct nodo* ant;
	struct nodo* sig;
} nodo;

typedef struct tlista {
	nodo* cab;
	nodo* actual;
} tlista;

void CrearNodoD(nodo** nuevo, tdato x) {
	*nuevo=malloc(sizeof(nodo));
	(*nuevo)->info=x;
	(*nuevo)->sig=NULL;
	(*nuevo)->ant=NULL;
}

/*
 * pre: Que la lista exista
 * pos: Verdadero si la lista esta vacia
 *
 */
int LVacia(tlista l) {
	return l.cab==NULL;
}

/*
 * pre: Que la pila no este vacia
 * pos: Verdadero si actual = siguiente(ultimo)
 *
 */
int LFin(tlista l) {
	return l.actual==NULL;
}

/*
 * pre: Que la lista exista
 * pos: Verdadero si la lista esta llena
 *
 */
int LLlena(tlista l) {
	return 0; // Se presupone memoria infinita
}

/*
 * pre: Que la pila no este vacia
 * pos: Actual = Primer Elemento
 *
 */
void LPpio(tlista* l) {
	l->actual=l->cab;
}

/*
 * pre: Actual = algun nodo de la lista
 * pos: X = Actual
 *
 */
void LInfo(tlista* l, tdato* x) {
	*x=l->actual->info;
}

/*
 * pre: Que la lista no este vacia
 * pos: Actual = siguiente(actual)
 *
 */
void LSig(tlista* l) {
	l->actual=l->actual->sig;
}

/*
 * pos: Lista creada
 *
 */
void LCrear(tlista* l) {
	l->cab=NULL;
}

/*
 * pre: Que lista no este vacia y Actual = algun nodo de la lista
 * pos: Actual = X
 *
 */
void LModificar(tlista* l, tdato x) {
	l->actual->info=x;
}

/*
 * pre: Que la lista no este llena
 * pos: Ppio lista = X
 *
 */
void LInsertarPpio(tlista* l, tdato x) {
	nodo* nue=NULL;

	CrearNodoD(&nue, x);
	if (l->cab==NULL) {
		nue->sig=NULL; // << s/nue/NULL/
		nue->ant=NULL; // << s/nue/NULL/
	} else {
		l->cab->ant=nue;
		nue->sig=l->cab;
	}

	l->cab=nue;
}

/*
 * pre: Que la lista no este llena
 * pos: Fin lista = X
 *
 */
void LInsertarFin(tlista* l, tdato x) {
	nodo* nue=NULL;
	nodo* t;

	CrearNodoD(&nue, x);
	if (l->cab==NULL) {
		l->cab=nue;
	} else {
		t=l->cab;
		while (t->sig!=NULL)
			t=t->sig;
		nue->ant=t;
		t->sig=nue;
	}
}

/*
 * pre: Que la lista este ordenada y no este llena
 * pos: X esta en la lista
 *
 */
void LInsertarOrden(tlista* l, tdato x, char torden) {
	nodo* nue=NULL;
	nodo* t;
	int tmp;

	CrearNodoD(&nue, x);
	if (l->cab==NULL) {
		l->cab=nue;
	} else {
		tmp=strncmp(l->cab->info.clave, x.clave,CLAVE_SIZE);
#ifdef CLAVE_CADENA
		if ( (torden=='A' && tmp>0)  || (torden=='D' && tmp<0) ) {
#else
		if ( (torden=='A' && l->cab->info.clave>x.clave) || (torden=='D' && l->cab->info.clave<x.clave) ) {
#endif
			nue->sig=l->cab;
			l->cab->ant=nue;
			l->cab=nue;
		} else {
			t=l->cab;
#ifdef CLAVE_CADENA
			while (t->sig!=NULL && ((torden=='A' && (strncmp(t->sig->info.clave,x.clave,CLAVE_SIZE)<0)) || (torden=='D' && (strncmp(t->sig->info.clave,x.clave,CLAVE_SIZE)>0))) )
#else
			while (t->sig!=NULL && ((torden=='A' && t->sig->info.clave<x.clave) || (torden=='D' && t->sig->info.clave>x.clave)) )
#endif
				t=t->sig;
			if (t->sig==NULL) {
				nue->ant=t;
				t->sig=nue;
			} else {
				nue->ant=t;
				nue->sig=t->sig;
				t->sig->ant=nue;
				t->sig=nue;
			}
		}
	}
}

/*
 * pre: Que la lista no este vacia y Actual = algun nodo de la lista
 * pos: Elemento actual no esta en la lista
 *
 */
void LBorrarActual(tlista* l) {
	nodo* aux=l->actual;

	if (l->actual==l->cab) {
		if (l->actual->sig!=NULL)
			l->cab->sig->ant=NULL;
		l->cab=l->cab->sig;
	} else {
		if (l->actual->sig!=NULL)
			l->actual->sig->ant=l->actual->ant;
		l->actual->ant->sig=l->actual->sig;
	}
	l->actual=l->actual->sig;
	free(aux);
}

/*
 * pre: Que la lista no este vacia
 * pos: UltimoElem(lista) = anterior(x)
 *
 */
void LBorrarFin(tlista* l) {
	nodo* aux;
	nodo* t;

	if (l->cab->sig==NULL) {
		aux=l->cab;
		l->cab=NULL;
	} else {
		t=l->cab;
		while (t->sig!=NULL)
			t=t->sig;
		t->ant->sig=NULL;
		aux=t;
	}
	free(aux);
}

/*
 * pre: Que la lista no este vacia
 * pos: PrimerElem(lista) = siguiente(x)
 *
 */
void LBorrarPpio(tlista* l) {
	nodo* aux=l->cab;

	if (l->cab->sig!=NULL)
		l->cab->sig->ant=NULL;
	l->cab=l->cab->sig;
	free(aux);
}

/*
 * pre: Que la lista exista y este ordenada
 * pos: Existe = Verdadero si existe un Tdato con la clave = clave
 *
 */
void LBuscarOrdenado(tlista* l, tclave buscado, int *existe) {
	l->actual=l->cab;

	if (l->actual!=NULL)
#ifdef CLAVE_CADENA
		while (l->actual->sig!=NULL && (strncmp(l->actual->info.clave,buscado,CLAVE_SIZE)<0))
#else
		while (l->actual->sig!=NULL && l->actual->info.clave<buscado)
#endif
			l->actual=l->actual->sig;

#ifdef CLAVE_CADENA
	*existe=(l->actual!=NULL && strcmp(l->actual->info.clave, buscado)==0);
#else
	*existe=(l->actual!=NULL && l->actual->info.clave==buscado);
#endif
}

#endif


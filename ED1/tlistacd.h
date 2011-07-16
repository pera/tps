/*
 * Implementacion del TDA de lista doblemente enlazada circular
 *
 */

#ifndef TLISTA
#define TLISTA

#include <stdlib.h>
#include "tdato.h"

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
	return l.actual==l.cab;
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
		nue->sig=nue;
		nue->ant=nue;
	} else {
		nue->sig=l->cab;
		nue->ant=l->cab->ant;
		l->cab->ant->sig=nue;
		l->cab->ant=nue;
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

	CrearNodoD(&nue, x);
	if (l->cab==NULL) {
		nue->sig=nue;
		nue->ant=nue;
		l->cab=nue;
	} else {
		nue->sig=l->cab;
		nue->ant=l->cab->ant;
		l->cab->ant->sig=nue;
		l->cab->ant=nue;
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

	CrearNodoD(&nue, x);
	if (l->cab==NULL) {
		l->cab=nue;
		l->cab->sig=l->cab;
		l->cab->ant=l->cab;
	} else {
		if ( (torden=='A' && l->cab->info.clave>x.clave) || (torden='D' && l->cab->info.clave<x.clave) ) {
			nue->sig=l->cab;
			nue->ant=l->cab->ant;
			l->cab->ant=nue;
			l->cab=nue;
		} else {
			t=l->cab;
			while (t->sig!=NULL && ((torden='A' && t->sig->info.clave<x.clave) || (torden='D' && t->sig->info.clave>x.clave)) )
				t=t->sig;
			nue->ant=t;
			nue->sig=t->sig;
			t->sig->ant=nue;
			t->sig=nue;
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
		if (l->cab==l->cab->sig) {
			l->cab=NULL;
			l->actual=NULL;
		} else {
			l->cab->sig->ant=l->cab->ant;
			l->cab->ant->sig=l->cab->sig;
			l->cab=l->cab->sig;
			l->actual=l->cab;
		}
	} else {
		l->actual->sig->ant=l->actual->ant;
		l->actual->ant->sig=l->actual->sig;
		l->actual=l->actual->sig;
	}
	free(aux);
}

/*
 * pre: Que la lista no este vacia
 * pos: UltimoElem(lista) = anterior(x)
 *
 */
void LBorrarFin(tlista* l) {
	nodo* aux;

	if (l->cab->sig==l->cab) {
		aux=l->cab;
		l->cab=NULL;
	} else {
		aux=l->cab->ant;
		l->cab->ant->ant->sig=l->cab;
		l->cab->ant=l->cab->ant->ant;
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

	if (l->cab->sig==l->cab) {
		l->cab=NULL;
	} else {
		l->cab->sig->ant=l->cab->ant;
		l->cab->ant->sig=l->cab->sig;
		l->cab=l->cab->sig;
	}
	free(aux);
}

/*
 * pre: Que la lista exista y este ordenada
 * pos: Existe = Verdadero si existe un Tdato con la clave = clave
 *
 */
void LBuscarOrdenado(tlista* l, int *existe, tclave buscado) {
	l->actual=l->cab;

	if (l->actual!=NULL)
		while (l->actual->sig!=l->cab && l->actual->info.clave<buscado)
			l->actual=l->actual->sig;

	*existe=(l->actual!=NULL && l->actual->info.clave==buscado);
}

#endif


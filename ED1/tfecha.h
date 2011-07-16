/*
 * Implementacion del TDA tfecha (entero largo)
 *
 */

#ifndef TFECHA
#define TFECHA

#include <ctype.h>

typedef char flegible[11]; /* dd/mm/aaaa */
typedef unsigned long int tfecha; /* aaaammdd */

/*
 * pre: -
 * pos: retorna 1 si F es una fecha valida y 0 si no lo es
 *
 */
int FValida(tfecha f) {
	int a, m, d, b;
	a=(f/10000);
	m=(f/100)%100;
	d=(f/1)%100;
	b=(a%4==0 && a%100!=0) || (a%400==0);

	if(m>0 && m<13 && d>0 && d<32) {
		if( ((m==4 || m==6 || m==9 || m==11) && d>30) || (m==2 && d>(28+b)) )
			return 0;
	} else {
		return 0;
	}

	return 1;
}

/*
 * pre: -
 * pos: retorna 1 si F1 > F2
 *
 */
int FMayor(tfecha f1, tfecha f2) {
	return f1>f2;
}

/*
 * pre: F debe ser una fecha valida
 * pos: suma N dias a la fecha F
 *
 */
void FSumarDias(tfecha* f, int n) {
	static char daytab[2][13] = {
		{0,31,28,31,30,31,30,31,31,30,31,30,31},
		{0,31,29,31,30,31,30,31,31,30,31,30,31},
	};
	int a, m, d;
	a=(*f/10000);
	m=(*f/100)%100;
	d=(*f/1)%100;

	while(n>0) {
		if(d==daytab[(a%4==0 && a%100!=0) || (a%400==0)][m]) {
			if(m==12) {
				a++;
				d=m=1;
			} else {
				m++;
				d=1;
			}
		} else {
			d++;
		}
		n--;
	}

	*f=a*10000+m*100+d;
}

/*
 * pre: -
 * pos: toma FORIG en formato dd/mm/aaaa y los guarda en FSAL con formato
 *      aaaammdd, si FORIG no es valido entonces error=1
 *
 */
void FConvertirFLegibleTFecha(flegible forig, tfecha* fsal, int* error) {
	int i, a, m, d;
	a=m=d=0;
	*error=0;

	for(i=0; i<10; i++){
		if(i==2 || i==5) {
			if(forig[i]!='/'){
				*error=1;
				break;
			}
		} else {
			if(isdigit(forig[i])) {
				if(i<2) {
					d=d*10+(forig[i]-'0');
				} else {
					if(i<5) {
						m=m*10+(forig[i]-'0');
					} else {
						a=a*10+(forig[i]-'0');
					}
				}
			} else {
				*error=1;
				break;
			}
		}
	}
	*fsal=a*10000+m*100+d;
}

/*
 * pre: FORIG debe ser una fecha valida
 * pos: toma FORIG en formato aaaammdd y lo guarda en FSAL
 *
 */
void FConvertirTFechaFLegible(tfecha forig, flegible fsal) {
	int a, m, d;
	a=(forig/10000);
	m=(forig/100)%100;
	d=(forig/1)%100;

	fsal[0]='0'+(d/10)%10;
	fsal[1]='0'+(d/1)%10;
	fsal[2]='/';
	fsal[3]='0'+(m/10)%10;
	fsal[4]='0'+(m/1)%10;
	fsal[5]='/';
	fsal[6]='0'+(a/1000)%10;
	fsal[7]='0'+(a/100)%10;
	fsal[8]='0'+(a/10)%10;
	fsal[9]='0'+(a/1)%10;
	fsal[10]='\0';
}

/*
 * pre: F1 y F2 deben ser fechas validas
 * pos: retorna la cantidad de dias entre F1 y F2
 *
 */
unsigned int FDiferenciaDias(tfecha f1, tfecha f2) {
	static char daytab[2][13] = {
		{0,31,28,31,30,31,30,31,31,30,31,30,31},
		{0,31,29,31,30,31,30,31,31,30,31,30,31},
	};
	int a[2], m[2], d[2], i, dias, anios, z1, z2;
	a[0]=(f1/10000);
	a[1]=(f2/10000);
	m[0]=(f1/100)%100;
	m[1]=(f2/100)%100;
	d[0]=(f1/1)%100;
	d[1]=(f2/1)%100;

	for(i=1; i<m[0]; i++)
		d[0]+=daytab[(a[0]%4==0 && a[0]%100!=0) || (a[0]%400==0)][i];
	for(i=1; i<m[1]; i++)
		d[1]+=daytab[(a[1]%4==0 && a[1]%100!=0) || (a[1]%400==0)][i];

	if(a[0]==a[1]){
		dias=d[0]>d[1]?d[0]-d[1]:d[1]-d[0];
	} else {
		if(a[0]>a[1]) {
			z1=0;
			z2=1;
		} else {
			z1=1;
			z2=0;
		}
		dias=(((a[z2]%4==0 && a[z2]%100!=0) || (a[z2]%400==0))?366:365)-d[z2];
		anios=a[z1]-(a[z2]+1);
		for(i=0; i<anios; i++){
			a[z2]++;
			dias+=((a[z2]%4==0 && a[z2]%100!=0) || (a[z2]%400==0))?366:365;
		}
		dias+=d[z1];
	}

	return dias;
}

/*
 * pre: F debe ser una fecha valida
 * pos: retorna el dia de F
 *
 */
int FDia(tfecha f) {
	return (f/1)%100;
}

/*
 * pre: F debe ser una fecha valida
 * pos: retorna el mes de F
 *
 */
int FMes(tfecha f) {
	return (f/100)%100;
}

/*
 * pre: F debe ser una fecha valida
 * pos: retorna el anio de F
 *
 */
int FAnio(tfecha f) {
	return (f/10000);
}

#endif


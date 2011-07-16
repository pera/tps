/*
 * Trabajo practico de Algoritmos 1: La batalla de tanques
 * (primer semestre 2011)
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

#define WIDTH 13
#define HEIGHT 13

typedef struct posicion {
    int x;
    int y;
} p;

/*
 * Tipos de variaciones para casilleros y estado de tanque
 *
 */
enum variacion {
    nulo,
    bomba,
    freeze,
    noshoot
};

/*
 * Estructura de un tanque
 *
 */
struct tanque {
    char equipo;
    char tipo; /* 1: acorazado | 0: normal */
    char vida;
    enum variacion estado;
};

/*
 * Nuevo tipo c: estructura de casillero del tablero
 *
 */
typedef struct casillero {
    enum tipo {
        espacio,
        pared,
        tanque
    } tipo;
    enum {
        normal,
        contaminado
    } espacio;
    enum variacion variacion;
    struct tanque tanque;
} c;

/*
 * Limpia el stream del stdin (solo para getc)
 *
 */
void flushstream(int *s) {
    while (*s!='\n')
        *s=getchar();
}

/*
 * Inicializa el tablero:
 * todos los casilleros seran espacios normales salvo los tres del centro, los cuales seran contaminados
 *
 */
void iniciartablero(c tablero[HEIGHT][WIDTH]) {
    int i, j;

    for (i=0; i<HEIGHT; i++) {
        for (j=0; j<WIDTH; j++) {
            tablero[i][j].variacion=nulo;
            if (i==0 || i==HEIGHT-1 || j==0 || j==WIDTH-1) {
                tablero[i][j].tipo=pared;
            } else {
                tablero[i][j].tipo=espacio;
                if (i>=((HEIGHT/2)-1) && i<=((HEIGHT/2)+1) &&
                        j>=((WIDTH/2)-1) && j<=((WIDTH/2)+1))
                    tablero[i][j].espacio=contaminado;
                else
                    tablero[i][j].espacio=normal;
            }
        }
    }
}

/*
 * Agrega un tanque en el casillero dado por referencia
 *
 */
void ponertanque(c *casillero, enum variacion variacion, char equipo, char tipo, char vida) {
    casillero->tipo=tanque;
    casillero->tanque.equipo=equipo;
    casillero->tanque.tipo=tipo;
    casillero->tanque.vida=vida;
    casillero->tanque.estado=variacion;
}

/*
 * Agrega los tanques en el tablero en forma triangular
 *
 */
int generartanques(c tablero[HEIGHT][WIDTH]) {
    int i, j, k;
    int cant_tanques=0;

    for (k=5, i=1; k>0; i++, --k) {
        for (j=k; j>0; j--) {
            ponertanque(&tablero[i][j], nulo, 0, 0, 1);
            cant_tanques++;
        }
    }
    ponertanque(&tablero[1][1], nulo, 0, 1, 2);

    for (k=5, i=HEIGHT-2; k>0; i--, --k) {
        for (j=(WIDTH-1)-k; j<(WIDTH-1); j++) {
            ponertanque(&tablero[i][j], nulo, 1, 0, 1);
        }
    }
    ponertanque(&tablero[WIDTH-2][HEIGHT-2], nulo, 1, 1, 2);

    return cant_tanques;
}

/*
 * Cargar el archivo de paredes y los agrega en el tablero
 *
 */
int cargarparedes(c tablero[HEIGHT][WIDTH], char *archivo) {
    FILE *paredes;
    int x, y, n=0, i, j, k; /*i cant paredes, n total paredes*/
    char s[5]; // maximo 2 chars numericos +2 de control (\r\n o \n) +\0
    char s2[8]; // n,n nn,n n,nn nn,nn + \0 \r \n
    char p=2;

    if (!(paredes=fopen(archivo, "r"))) {
        puts("Error al cargar archivo de paredes (posiblemente nombre incorrecto)");
        fclose(paredes);
        return 1;
    }

    if(!fgets(s,sizeof(s),paredes)) {
        puts("Error: archivo vacio");
        fclose(paredes);
        return 1;
    }

#
    for(i=0; s[i]!='\0' && s[i]!='\n' && s[i]!='\r'; i++) {
        if(isdigit(s[i])) {
            if(i==2) {
                puts("error, numero de mas de 2 digitos");
                fclose(paredes);
                return 1;
            } else {
                n=n*10+(s[i]-'0');
            }
        } else {
            puts("error, no es numero");
            fclose(paredes);
            return 1;
        }
    }
    if(!i) {
        puts("Error de formato (primer linea)");
        fclose(paredes);
        return 1;
    }
    if(n) {
        if(n>11) {
            puts("Error, cantidad de paredes superior a la permitida");
            fclose(paredes);
            return 1;
        } else {
            i=n;
        }
    } else {
        fclose(paredes);
        return 0;
    }

    fgets(s2, sizeof(s2), paredes);
if(!feof(paredes)){//<<
    while (i>0) {//<<
        x=y=0;
#
        for(j=0; s2[j]!=','; j++) {
            if(s2[j]=='\0') {
                puts("error, fin de archivo inesperado");
                fclose(paredes);
                return 1;
            }
            if(isdigit(s2[j])) {
                if(j==2) {
                    puts("error, numero de mas de 2 digitos");
                    fclose(paredes);
                    return 1;
                } else {
                    x=x*10+(s2[j]-'0');
                }
            } else {
                puts("error, no es numero");
                fclose(paredes);
                return 1;
            }
        }
#
        k=j;
        for(++j; s2[j]!='\0' && s2[j]!='\n' && s2[j]!='\r'; j++) {
            if(isdigit(s2[j])) {
                if(j>k+2) {
                    puts("error, numero de mas de 2 digitos");
                    fclose(paredes);
                    return 1;
                } else {
                    y=y*10+(s2[j]-'0');
                }
            } else {
                puts("error, no es numero");
                fclose(paredes);
                return 1;
            }
        }

//--
        if (x<1 || x>WIDTH-1 || y<1 || y>HEIGHT-1) {
            puts("Error: coordenada incorrecta");
            fclose(paredes);
            return 3;
        }

        if (tablero[y][x].tipo!=espacio) {
            printf("Error: coordenada ocupada (%i,%i)\n", x, y);
            fclose(paredes);
            return 4;
        }
        tablero[y][x].tipo=pared;
        i--;

		if(feof(paredes))//<<
			break;//<<
        fgets(s2, sizeof(s2), paredes);
    }
}//<<
    if (i!=0) {
        if (p!=2) {
            puts("Error de formato");
            fclose(paredes);
            return 5;
        } else {
            puts("Fin de archivo inesperadoaa");
            fclose(paredes);
            return 6;
        }
    }
    if(!feof(paredes)) {
        puts("(Hay mas lineas de las indicadas)");
    }
    fclose(paredes);

    return 0;
}

/*
 * Carga el archivo de variaciones y setea los casilleros segun este
 *
 */
int cargarvariaciones(c tablero[HEIGHT][WIDTH], char *archivo) {
    FILE *variaciones;
    int x, y, n=0, i, j, k;
    char v; /* variacion */
    char s[5];
    char s2[10];
    int p=3;
    enum variacion estado_aux;

    if (!(variaciones=fopen(archivo, "r"))) {
        puts("Error al cargar archivo de variaciones (posiblemente nombre incorrecto)");
        return 1;
    }

    if(!fgets(s,sizeof(s),variaciones)) {
        puts("Error: archivo vacio");
        fclose(variaciones);
        return 1;
    }

#
    for(i=0; s[i]!='\0' && s[i]!='\n' && s[i]!='\r'; i++) {
        if(isdigit(s[i])) {
            if(i==2) {
                puts("error, numero de mas de 2 digitos");
                fclose(variaciones);
                return 1;
            } else {
                n=n*10+(s[i]-'0');
            }
        } else {
            puts("error, no es numero");
            fclose(variaciones);
            return 1;
        }
    }
    if(!i) {
        puts("Error de formato (primer linea)");
        fclose(variaciones);
        return 1;
    }
    if(n) {
        if(n>30) {
            puts("Error, cantidad de variaciones superior a la permitida");
            fclose(variaciones);
            return 1;
        } else {
            i=n;
        }
    } else {
        fclose(variaciones);
        return 0;
    }

    fgets(s2, sizeof(s2), variaciones);

if(!feof(variaciones)){//<<
    while ( p==3 && i>0) {
        estado_aux=nulo;
//--------------------


        x=y=0;
#
        for(j=0; s2[j]!=','; j++) {
            if(s2[j]=='\0') {
                puts("error, fin de archivo inesperado");
                fclose(variaciones);
                return 1;
            }
            if(isdigit(s2[j])) {
                if(j==2) {
                    puts("error, numero de mas de 2 digitos");
                    fclose(variaciones);
                    return 1;
                } else {
                    x=x*10+(s2[j]-'0');
                }
            } else {
                puts("error, no es numero");
                fclose(variaciones);
                return 1;
            }
        }
#
        k=j;
        for(++j; s2[j]!=','; j++) {
            if(s2[j]=='\0') {
                puts("error, fin de archivo inesperado");
                fclose(variaciones);
                return 1;
            }
            if(isdigit(s2[j])) {
                if(j>k+2) {
                    puts("error, numero de mas de 2 digitos");
                    fclose(variaciones);
                    return 1;
                } else {
                    y=y*10+(s2[j]-'0');
                }
            } else {
                puts("error, no es numero");
                fclose(variaciones);
                return 1;
            }
        }


        v=s2[++j];
        ++j;
        if(s2[j]!='\0' && s2[j]!='\n' && s2[j]!='\r') {
            puts("Error: formato de coordenada y varicion incorrecto");
            fclose(variaciones);
            return 1;
        }


//--------------------
        switch (v) {
        case 'B':
            estado_aux=bomba;
            break;
        case 'F':
            estado_aux=freeze;
            break;
        case 'N':
            estado_aux=noshoot;
            break;
        default:
            puts("Error: variacion no reconocida");
            return 2;
        }
        if (x<1 || x>WIDTH-1 || y<1 || y>HEIGHT-1)
            return 3;
        if (tablero[y][x].tipo==tanque) {
            tablero[y][x].tanque.estado=estado_aux;
            switch (estado_aux) {
            case nulo:
                break;
            case bomba:
                printf("/!\\ Tanque con bomba cuadrada en: %c,%c\n", (char)x+0x60, (char)y+0x60);
                break;
            case freeze:
                printf("/!\\ Tanque con estado freeze en: %c,%c\n", (char)x+0x60, (char)y+0x60);
                break;
            case noshoot:
                printf("/!\\ Tanque con estado noshoot en: %c,%c\n", (char)x+0x60, (char)y+0x60);
                break;
            }
        } else {
            if (tablero[y][x].tipo==espacio) {
                tablero[y][x].variacion=estado_aux;
            } else {
                puts("Error: variacion superpuesta en pared");
                fclose(variaciones);
                return 4;
            }
        }
        i--;
		if(feof(variaciones))//<<
			break;//<<
		fgets(s2, sizeof(s2), variaciones);
    }
}//<<
    if (i!=0) {
        if (p!=3) {
            puts("Error de formato");
            return 5;
        } else {
            puts("Fin de archivo inesperado");
            return 6;
        }
    }
//	printf(">>%i\n", p);
    fclose(variaciones);

    return 0;
}

/*
 * Mueve el tanque en la posicion dada por x1,y1 a la posicion x2,y2
 *
 */
int movertanque(c tablero[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, char turno) {
    c *casillero1;
    c *casillero2;

    if (x1<0 || x1>WIDTH-1 || y1<0 || y1>HEIGHT-1 || x2<0 || x2>WIDTH-1 || y2<0 || y2>HEIGHT-1)
        return 1; /* Error: coordenada incorrecta */

    /* verifico que la distancia sea de 1 */
    if (abs(x1-x2)!=1 && abs(y1-y2)!=1)
        return 2; /* Error: movimiento ilegal */

    casillero1=&tablero[y1][x1];
    casillero2=&tablero[y2][x2];

    if (casillero1->tipo!=tanque)
        return 3; /* Error: no es tanque */

    if (casillero1->tanque.equipo!=turno)
        return 4; /* Error: equipo incorrecto */

    if (casillero2->tipo!=espacio)
        return 5; /* Error: espacio bloqueado */

    if (casillero1->tanque.tipo==0 && casillero2->espacio==contaminado)
        return 6; /* Error: tanque normal intentando entrar en espacio contaminado */

    ponertanque(casillero2, casillero2->variacion, casillero1->tanque.equipo, casillero1->tanque.tipo, casillero1->tanque.vida);
    casillero1->tipo=espacio;
    casillero1->variacion=normal;

    switch (casillero2->variacion) {
    case nulo:
        break;
    case bomba:
        puts("/!\\ Se encontro una bomba cuadrada.");
        break;
    case freeze:
        puts("/!\\ Freeze, tanque inmovilizado por un turno.");
        break;
    case noshoot:
        puts("/!\\ Noshoot, tanque inhabilitado para disparar por un turno.");
        break;
    }

    return 0;
}

/*
 * Realiza la accion de disparar:
 * genera un disparo desde el tanque de la posicion x1,y1 al tanque en la posicion x2,y2
 *
 */
int disparar(c tablero[HEIGHT][WIDTH], int x1, int y1, int x2, int y2, char turno, int *cant_tanques_a, int *cant_tanques_b, int *puntos_a, int *puntos_b) {
    c *casillero1; /* donde estoy */
    c *casillero2; /* hacia donde quiero disparar */

    if (x1<0 || x1>WIDTH-1 || y1<0 || y1>HEIGHT-1 || x2<0 || x2>WIDTH-1 || y2<0 || y2>HEIGHT-1)
        return 1; /* Error: coordenada incorrecta */

    /* verifico que la distancia sea de 1 */
    if (abs(x1-x2)!=1 && abs(y1-y2)!=1)
        return 2; /* Error: disparo ilegal */

    casillero1=&tablero[y1][x1];
    casillero2=&tablero[y2][x2];

    if (casillero1->tipo!=tanque)
        return 3; /* Error: no es un tanque */

    if (casillero1->tanque.equipo!=turno)
        return 4; /* Error: equipo incorrecto */

    if (casillero2->tipo==tanque) {
        if (casillero1->tanque.equipo==casillero2->tanque.equipo)
            return 5; /* Error: friendly fire */

        casillero2->tanque.vida--;
        if (!casillero2->tanque.vida) {
            if (turno==0) {
                (*cant_tanques_b)--;
                if (casillero2->tanque.tipo==0)
                    puntos_a++;
                else
                    puntos_a+=2;
            } else {
                (*cant_tanques_a)--;
                if (casillero2->tanque.tipo==0)
                    puntos_b++;
                else
                    puntos_b+=2;
            }
            casillero2->tipo=espacio;
        }
    }


    return 0;
}

/*
 * Realiza la accion de la bomba cuadrada
 *
 */
int tirarbomba(c tablero[HEIGHT][WIDTH], int x, int y, char turno, int *cant_tanques_a, int *cant_tanques_b, int *puntos_a, int *puntos_b) {
    int i, j;
    c *casillero;

    if (x<0 || x>WIDTH-1 || y<0 || y>HEIGHT-1)
        return 1; /* Error: coordenada incorrecta */

    casillero=&tablero[y][x];

    if (casillero->tipo!=tanque)
        return 2; /* Error: no es un tanque */

    if (casillero->tanque.equipo!=turno);
    return 3; /* Error: equipo incorrecto */

    for (i=y-1; i<y+2; i++)
        for (j=x-1; j<x+2; j++)
            disparar(tablero, x, y, j, i, turno, cant_tanques_a, cant_tanques_b, puntos_a, puntos_b);

    return 0;
}

/*
 * Imprime en pantalla la coordenada segun la posicion del tablero
 *
 */
void mostrarcoordenada(int n, int max) {
    char c;

    if (n>0 && n<max-1)
        c='a'+n-1;
    else
        c=' ';

    printf("%c", c);
}

/*
 * Imprime en pantalla el tablero
 *
 */
void mostrartablero(c tablero[HEIGHT][WIDTH]) {
    int i, j;
    c *casillero;

    printf(" ");
    for (j=0; j<WIDTH; j++)
        mostrarcoordenada(j, WIDTH);
    puts("");

    for (i=0; i<HEIGHT; i++) {
        mostrarcoordenada(i, HEIGHT);
        for (j=0; j<WIDTH; j++) {
            casillero=&tablero[i][j];
            switch (casillero->tipo) {
            case pared:
                printf("#");
                break;
            case espacio:
                switch (casillero->variacion) {
                case nulo:
                    if (casillero->espacio==normal)
                        printf(".");
                    else
                        printf("-");
                    break;
                case bomba:
                    printf(".");
                    break;
                case freeze:
                    printf(".");
                    break;
                case noshoot:
                    printf(".");
                    break;
                }
                break;
            case tanque:
                if (casillero->tanque.equipo) {
                    if (casillero->tanque.tipo)
                        printf("A");
                    else
                        printf("T");
                } else {
                    if (casillero->tanque.tipo)
                        printf("a");
                    else
                        printf("t");
                }
                break;
            }
        }
        mostrarcoordenada(i, HEIGHT);
        puts("");
    }

    printf(" ");
    for (j=0; j<WIDTH; j++)
        mostrarcoordenada(j, WIDTH);
    puts("");
}

/*
 * Pide al usuario la accion que desea realizar (mover o disparar)
 *
 */
char obteneraccion() {
    int respuesta='\n';
    char accion;

    do {
        flushstream(&respuesta);
        printf("Desea [m]over o [d]isparar? ");
        respuesta=getchar();
    } while ((respuesta!='m') && (respuesta!='d'));

    if (respuesta=='m')
        accion=0;
    else
        accion=1;

    flushstream(&respuesta);

    return accion;
}

/*
 * Pide al usuario una coordenada
 *
 */
void obtenercoordenada(int *x, int *y) {
    int respuesta='\n';

    do {
        flushstream(&respuesta);
        printf("Ingrese X: ");
        respuesta=getchar();
    } while (!islower((char)respuesta));
    *x=respuesta-0x60;

    do {
        flushstream(&respuesta);
        printf("Ingrese Y: ");
        respuesta=getchar();
    } while (!islower((char)respuesta));
    *y=respuesta-0x60;

    flushstream(&respuesta);
}

int main(int argc, char *argv[]) {
    c tablero[HEIGHT][WIDTH];
    int x1, y1, x2, y2;
    int cant_tanques_a, cant_tanques_b;
    /* guardo los casilleros anteriores de ambos jugadores */
    p pos_ant_a;
    p pos_ant_b;
    char turno=0;
    char error;
    char finjuego=0;
    int puntos_a=0;
    int puntos_b=0;

    /* inicializo las posiciones anteriores en 0 (ubicacion ocupada por pared) */
    pos_ant_a.x=0;
    pos_ant_a.y=0;
    pos_ant_b.x=0;
    pos_ant_b.y=0;

    if (argc!=3) {
        printf("Modo de uso: %s archivo_paredes archivo_variaciones\n",argv[0]);
        return 1; /* Error: faltan parametros de entrada */
    }

    /* Inicio y cargo tablero */
    iniciartablero(tablero);
    const int ct=generartanques(tablero); /*ct: cantidad de tanques*/
    cant_tanques_b=cant_tanques_a=ct;
    puts("Cargando archivo de paredes...");
    if (cargarparedes(tablero, argv[1])!=0)
        return 2; /* Error: problema con el archivo de paredes */
    puts("Cargando archivo de variaciones...");
    if (cargarvariaciones(tablero, argv[2]))
        return 3; /* Error: problema con el archivo de variaciones */

    /* Loop principal */
    while (!(finjuego)) {
        puts("\n------------ NUEVO TURNO ------------");
        printf("[tanques de jugador A capturados: %i]\n", ct - cant_tanques_a);
        printf("[tanques de jugador B capturados: %i]\n\n", ct - cant_tanques_b);

        mostrartablero(tablero);

        printf("\nJUGADOR %c\n\n", turno+'A');
        puts("* INDIQUE TANQUE *");
        obtenercoordenada(&x1, &y1);
        if (turno==0) {
            if (pos_ant_a.x!=0) /* verifico que no sea el primer turno */
                if (!(pos_ant_a.x==x1 && pos_ant_a.y==y1)) /*verifica que la posicion anterior
               	   	   	   	   	   	   	   	   	   	   	   sea distinta que la seleccionada*/
                    tablero[pos_ant_a.y][pos_ant_a.x].tanque.estado=nulo;
        } else {
            if (pos_ant_b.x) /* verifico que no sea el primer turno */
                if (!(pos_ant_b.x==x1 && pos_ant_b.y==y1))
                    tablero[pos_ant_b.y][pos_ant_b.x].tanque.estado=nulo;
        }
        do {
            if (obteneraccion()) {
                switch (tablero[y1][x1].tanque.estado) {
                case bomba:
                    puts("/!\\ Se acciona bomba cuadrada.");
                    tirarbomba(tablero, x1, y1, turno, &cant_tanques_a, &cant_tanques_b, &puntos_a, &puntos_b);
                    error=0;
                    break;
                case noshoot:
                    puts("/!\\ Este tanque no puede disparar en este turno.");
                    error=0;
                    break;
                default:
                    puts("* INDIQUE DIRECCION *");
                    obtenercoordenada(&x2, &y2);
                    error=disparar(tablero, x1, y1, x2, y2, turno, &cant_tanques_a, &cant_tanques_b, &puntos_a, &puntos_b);
                }
                if (!(error))
                    tablero[y1][x1].tanque.estado=nulo;
            } else {
                switch (tablero[y1][x1].tanque.estado) {
                case freeze:
                    puts("/!\\ Este tanque no puede moverse en este turno.");
                    tablero[y1][x1].tanque.estado=nulo;
                    error=0;
                    break;
                default:
                    puts("* INDIQUE DIRECCION *");
                    obtenercoordenada(&x2, &y2);
                    error=movertanque(tablero, x1, y1, x2, y2, turno);
                    x1=x2;
                    y1=y2;
                }
            }
            if (error) {
                puts("\nLa accion indicada es ilegal...\n");
                puts("* INDIQUE TANQUE *");
                obtenercoordenada(&x1, &y1);
            }
        } while (error);

        if (	cant_tanques_a==0 || cant_tanques_b==0 || (turno==0 && x1==WIDTH-2 && y1==HEIGHT-2) || (turno==1 && x1==1 && y1==1) ) {
            finjuego=1;
            if (turno==0 && x1==WIDTH-2 && y1==HEIGHT-2) {
                if (tablero[y1][x1].tanque.tipo==0)
                    puntos_a+=3;
                else
                    puntos_a+=4;
            } else if (turno==1 && x1==1 && y1==1) {
                if (tablero[y1][x1].tanque.tipo==0)
                    puntos_b+=3;
                else
                    puntos_b+=4;
            }
        } else {
            /* necesito que x1 e y1 sean las coordenadas finales del tanque */
            if (turno==0) {
                pos_ant_a.x=x1;
                pos_ant_a.y=y1;
            } else {
                pos_ant_b.x=x1;
                pos_ant_b.y=y1;
            }
            turno^=1;
        }
    }
    /* Fin del loop principal */

    if (puntos_a==puntos_b)
        printf("\n\n Fin del juego, los jugadores han empatado!\n");
    else
        printf("\n\n Fin del juego, el ganador es el jugador %c!\n", puntos_a > puntos_b ? 'A' : 'B');

    printf("\n  [puntos de A: %i]", puntos_a);
    printf("\n  [puntos de B: %i]\n\n", puntos_b);

    return 0;
}

